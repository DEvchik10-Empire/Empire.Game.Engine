#include "Empire.Engine/Engine.Kernel/Kernel.Threading/Threading.Backend.hpp"

#include <windows.h>

#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"

// Приватная реализация Kernel.Threading: Windows.
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
                            };

                            using SetThreadDescriptionFunction = HRESULT (WINAPI*)(HANDLE, PCWSTR);

                            DWORD WINAPI ThreadMain(void* Context)
                            {
                                const StartData Data = *static_cast<StartData*>(Context);
                                KernelMemory::Deallocate(Context);

                                if (Data.Name[0] != '\0')
                                {
                                    HMODULE KernelLibrary = GetModuleHandleW(L"Kernel32.dll");
                                    auto SetDescription = KernelLibrary == nullptr
                                        ? nullptr
                                        : reinterpret_cast<SetThreadDescriptionFunction>(GetProcAddress(KernelLibrary, "SetThreadDescription"));
                                    if (SetDescription != nullptr)
                                    {
                                        WCHAR WideName[128]{};
                                        if (MultiByteToWideChar(CP_UTF8, 0, Data.Name, -1, WideName, 128) > 0)
                                        {
                                            const HRESULT NameStatus = SetDescription(GetCurrentThread(), WideName);
                                            static_cast<void>(NameStatus);
                                        }
                                    }
                                }

                                Data.Entry(Data.Context);
                                return 0;
                            }
                        }

                        KernelTypes::UInt32 GetHardwareThreadCount() noexcept
                        {
                            SYSTEM_INFO Info{};
                            GetSystemInfo(&Info);
                            return Info.dwNumberOfProcessors > 0 ? Info.dwNumberOfProcessors : 1;
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
                            KernelTypes::Size Index = 0;
                            if (Name != nullptr)
                            {
                                for (; Index + 1 < sizeof(Data->Name) && Name[Index] != '\0'; ++Index)
                                    Data->Name[Index] = Name[Index];
                            }
                            Data->Name[Index] = '\0';

                            HANDLE Thread = CreateThread(nullptr, 0, &ThreadMain, Data, 0, nullptr);
                            if (Thread == nullptr)
                            {
                                KernelMemory::Deallocate(Data);
                                return false;
                            }

                            OutHandle = static_cast<KernelTypes::UInt64>(reinterpret_cast<KernelTypes::UIntPtr>(Thread));
                            return true;
                        }

                        void JoinThread(KernelTypes::UInt64 Handle) noexcept
                        {
                            HANDLE Thread = reinterpret_cast<HANDLE>(static_cast<KernelTypes::UIntPtr>(Handle));
                            const DWORD WaitStatus = WaitForSingleObject(Thread, INFINITE);
                            const BOOL CloseStatus = CloseHandle(Thread);
                            static_cast<void>(WaitStatus);
                            static_cast<void>(CloseStatus);
                        }

                        void* CreateSemaphore() noexcept
                        {
                            return ::CreateSemaphoreW(nullptr, 0, MAXLONG, nullptr);
                        }

                        void DestroySemaphore(void* Semaphore) noexcept
                        {
                            if (Semaphore != nullptr)
                            {
                                const BOOL CloseStatus = CloseHandle(static_cast<HANDLE>(Semaphore));
                                static_cast<void>(CloseStatus);
                            }
                        }

                        void SignalSemaphore(void* Semaphore, KernelTypes::UInt32 Count) noexcept
                        {
                            const BOOL ReleaseStatus = ReleaseSemaphore(static_cast<HANDLE>(Semaphore), static_cast<LONG>(Count), nullptr);
                            static_cast<void>(ReleaseStatus);
                        }

                        void WaitSemaphore(void* Semaphore) noexcept
                        {
                            const DWORD WaitStatus = WaitForSingleObject(static_cast<HANDLE>(Semaphore), INFINITE);
                            static_cast<void>(WaitStatus);
                        }
                    }
                }
            }
        }
    }
}
