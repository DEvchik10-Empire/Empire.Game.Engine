#include "Empire.Engine/Engine.Kernel/Kernel.Threading/Threading.Backend.hpp"

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"

// Приватная реализация: Linux (pthread и sem_t)
namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelThreading
                {
                    namespace ThreadingBackend
                    {
                        namespace
                        {
                            struct StartData
                            {
                                ThreadEntry Entry;
                                void* Context;
                                char Name[16];
                            };

                            void* ThreadMain(void* Argument)
                            {
                                StartData Data = *static_cast<StartData*>(Argument);
                                KernelMemory::Deallocate(Argument);

                                if (Data.Name[0] != '\0')
                                {
                                    prctl(PR_SET_NAME, Data.Name, 0, 0, 0);
                                }

                                Data.Entry(Data.Context);
                                return nullptr;
                            }
                        }

                        KernelTypes::UInt32 GetHardwareThreadCount() noexcept
                        {
                            // Учитываем маску affinity (контейнеры, taskset), иначе общее число ядер
                            cpu_set_t Set;
                            if (sched_getaffinity(0, sizeof(Set), &Set) == 0)
                            {
                                int Count = CPU_COUNT(&Set);
                                if (Count > 0)
                                {
                                    return static_cast<KernelTypes::UInt32>(Count);
                                }
                            }

                            long Online = sysconf(_SC_NPROCESSORS_ONLN);
                            return Online > 0 ? static_cast<KernelTypes::UInt32>(Online) : 1;
                        }

                        KernelTypes::Bool StartThread(ThreadEntry Entry, void* Context, const KernelTypes::Char* Name, KernelTypes::UInt64& OutHandle) noexcept
                        {
                            StartData* Data = static_cast<StartData*>(KernelMemory::Allocate(sizeof(StartData)));
                            if (Data == nullptr)
                            {
                                return false;
                            }

                            Data->Entry = Entry;
                            Data->Context = Context;
                            Data->Name[0] = '\0';
                            if (Name != nullptr)
                            {
                                // Имя потока в Linux не длиннее 15 символов
                                int Index = 0;
                                for (; Index < 15 && Name[Index] != '\0'; ++Index)
                                {
                                    Data->Name[Index] = Name[Index];
                                }
                                Data->Name[Index] = '\0';
                            }

                            pthread_t Thread;
                            if (pthread_create(&Thread, nullptr, ThreadMain, Data) != 0)
                            {
                                KernelMemory::Deallocate(Data);
                                return false;
                            }

                            OutHandle = static_cast<KernelTypes::UInt64>(Thread);
                            return true;
                        }

                        void JoinThread(KernelTypes::UInt64 Handle) noexcept
                        {
                            pthread_join(static_cast<pthread_t>(Handle), nullptr);
                        }

                        void* CreateSemaphore() noexcept
                        {
                            sem_t* Semaphore = static_cast<sem_t*>(KernelMemory::Allocate(sizeof(sem_t)));
                            if (Semaphore == nullptr)
                            {
                                return nullptr;
                            }

                            if (sem_init(Semaphore, 0, 0) != 0)
                            {
                                KernelMemory::Deallocate(Semaphore);
                                return nullptr;
                            }

                            return Semaphore;
                        }

                        void DestroySemaphore(void* Semaphore) noexcept
                        {
                            if (Semaphore != nullptr)
                            {
                                sem_destroy(static_cast<sem_t*>(Semaphore));
                                KernelMemory::Deallocate(Semaphore);
                            }
                        }

                        void SignalSemaphore(void* Semaphore, KernelTypes::UInt32 Count) noexcept
                        {
                            for (KernelTypes::UInt32 Index = 0; Index < Count; ++Index)
                            {
                                sem_post(static_cast<sem_t*>(Semaphore));
                            }
                        }

                        void WaitSemaphore(void* Semaphore) noexcept
                        {
                            while (sem_wait(static_cast<sem_t*>(Semaphore)) != 0 && errno == EINTR)
                            {
                            }
                        }
                    }
                }
            }
        }
    }
}
