#include "Empire.Engine/Engine.Kernel/Kernel.Threading/Threading.Backend.hpp"

#include <dispatch/dispatch.h>
#include <pthread.h>
#include <pthread/qos.h>
#include <sys/sysctl.h>

#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"

// Приватная реализация Kernel.Threading: macOS.
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
                                KernelTypes::Char Name[64];
                                qos_class_t QualityOfServiceClass;
                                int RelativePriority;
                            };

                            void* ThreadMain(void* Context)
                            {
                                const StartData Data = *static_cast<StartData*>(Context);
                                KernelMemory::Deallocate(Context);

                                if (Data.Name[0] != '\0')
                                {
                                    const int NameStatus = pthread_setname_np(Data.Name);
                                    static_cast<void>(NameStatus);
                                }

                                // Сохраняем QoS вызывающего потока, как Jolt JobSystemThreadPool на Apple,
                                // чтобы ожидание задач не приводило к priority inversion.
                                const int QoSStatus = pthread_set_qos_class_self_np(Data.QualityOfServiceClass, Data.RelativePriority);
                                static_cast<void>(QoSStatus);

                                Data.Entry(Data.Context);
                                return nullptr;
                            }
                        }

                        KernelTypes::UInt32 GetHardwareThreadCount() noexcept
                        {
                            int Count = 0;
                            size_t CountSize = sizeof(Count);
                            if (sysctlbyname("hw.logicalcpu", &Count, &CountSize, nullptr, 0) == 0 && Count > 0)
                                return static_cast<KernelTypes::UInt32>(Count);
                            return 1;
                        }

                        KernelTypes::Bool StartThread(
                            ThreadEntry Entry,
                            void* Context,
                            const KernelTypes::Char* Name,
                            KernelTypes::UInt64& OutHandle
                        ) noexcept
                        {
                            StartData* Data = static_cast<StartData*>(KernelMemory::Allocate(sizeof(StartData), alignof(StartData)));
                            if (Data == nullptr)
                                return false;

                            Data->Entry = Entry;
                            Data->Context = Context;
                            Data->QualityOfServiceClass = qos_class_self();
                            Data->RelativePriority = 0;
                            KernelTypes::Size Index = 0;
                            if (Name != nullptr)
                            {
                                for (; Index + 1 < sizeof(Data->Name) && Name[Index] != '\0'; ++Index)
                                    Data->Name[Index] = Name[Index];
                            }
                            Data->Name[Index] = '\0';

                            pthread_t Thread{};
                            if (pthread_create(&Thread, nullptr, &ThreadMain, Data) != 0)
                            {
                                KernelMemory::Deallocate(Data);
                                return false;
                            }

                            OutHandle = static_cast<KernelTypes::UInt64>(reinterpret_cast<KernelTypes::UIntPtr>(Thread));
                            return true;
                        }

                        void JoinThread(KernelTypes::UInt64 Handle) noexcept
                        {
                            pthread_t Thread = reinterpret_cast<pthread_t>(static_cast<KernelTypes::UIntPtr>(Handle));
                            const int JoinStatus = pthread_join(Thread, nullptr);
                            static_cast<void>(JoinStatus);
                        }

                        void* CreateSemaphore() noexcept
                        {
                            return static_cast<void*>(dispatch_semaphore_create(0));
                        }

                        void DestroySemaphore(void* Semaphore) noexcept
                        {
                            if (Semaphore != nullptr)
                                dispatch_release(static_cast<dispatch_semaphore_t>(Semaphore));
                        }

                        void SignalSemaphore(void* Semaphore, KernelTypes::UInt32 Count) noexcept
                        {
                            dispatch_semaphore_t NativeSemaphore = static_cast<dispatch_semaphore_t>(Semaphore);
                            for (KernelTypes::UInt32 Index = 0; Index < Count; ++Index)
                            {
                                const long SignalStatus = dispatch_semaphore_signal(NativeSemaphore);
                                static_cast<void>(SignalStatus);
                            }
                        }

                        void WaitSemaphore(void* Semaphore) noexcept
                        {
                            const long WaitStatus = dispatch_semaphore_wait(
                                static_cast<dispatch_semaphore_t>(Semaphore),
                                DISPATCH_TIME_FOREVER
                            );
                            static_cast<void>(WaitStatus);
                        }
                    }
                }
            }
        }
    }
}
