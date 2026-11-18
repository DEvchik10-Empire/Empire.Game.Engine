#include "Empire.Engine/Engine.Kernel/Kernel.Threading/Threading.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Threading/Threading.Backend.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

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
                    KernelTypes::UInt32 GetHardwareThreadCount() noexcept
                    {
                        return ThreadingBackend::GetHardwareThreadCount();
                    }

                    Thread::~Thread()
                    {
                        KernelAssert::DebugCheck(!Joinable, "Thread: уничтожение не присоединённого потока");
                    }

                    Thread::Thread(Thread&& Other) noexcept
                        : Handle(Other.Handle), Joinable(Other.Joinable)
                    {
                        Other.Handle = 0;
                        Other.Joinable = false;
                    }

                    Thread& Thread::operator=(Thread&& Other) noexcept
                    {
                        if (this != &Other)
                        {
                            KernelAssert::DebugCheck(!Joinable, "Thread: присваивание поверх не присоединённого потока");
                            Handle = Other.Handle;
                            Joinable = Other.Joinable;
                            Other.Handle = 0;
                            Other.Joinable = false;
                        }

                        return *this;
                    }

                    KernelTypes::Bool Thread::Start(Entry EntryPoint, void* Context, const KernelTypes::Char* Name) noexcept
                    {
                        KernelAssert::DebugCheck(!Joinable, "Thread::Start: поток уже запущен");

                        Joinable = ThreadingBackend::StartThread(EntryPoint, Context, Name, Handle);
                        return Joinable;
                    }

                    void Thread::Join() noexcept
                    {
                        if (Joinable)
                        {
                            ThreadingBackend::JoinThread(Handle);
                            Handle = 0;
                            Joinable = false;
                        }
                    }

                    Semaphore::Semaphore()
                    {
                        OsSemaphore = ThreadingBackend::CreateSemaphore();
                        KernelAssert::Check(OsSemaphore != nullptr, "Semaphore: не удалось создать семафор ОС");
                    }

                    Semaphore::~Semaphore()
                    {
                        ThreadingBackend::DestroySemaphore(OsSemaphore);
                    }

                    void Semaphore::Release(KernelTypes::UInt32 Number)
                    {
                        KernelAssert::DebugCheck(Number > 0, "Semaphore::Release: Number должно быть больше 0");

                        KernelTypes::Int32 OldValue = Count.FetchAdd(static_cast<KernelTypes::Int32>(Number), KernelSync::MemoryOrder::Release);
                        if (OldValue < 0)
                        {
                            // Были ждущие: будим столько, сколько хватит счётчика
                            KernelTypes::Int32 NewValue = OldValue + static_cast<KernelTypes::Int32>(Number);
                            KernelTypes::Int32 ToRelease = (NewValue < 0 ? NewValue : 0) - OldValue;
                            ThreadingBackend::SignalSemaphore(OsSemaphore, static_cast<KernelTypes::UInt32>(ToRelease));
                        }
                    }

                    void Semaphore::Acquire(KernelTypes::UInt32 Number)
                    {
                        KernelAssert::DebugCheck(Number > 0, "Semaphore::Acquire: Number должно быть больше 0");

                        KernelTypes::Int32 OldValue = Count.FetchSub(static_cast<KernelTypes::Int32>(Number), KernelSync::MemoryOrder::Acquire);
                        KernelTypes::Int32 NewValue = OldValue - static_cast<KernelTypes::Int32>(Number);
                        if (NewValue < 0)
                        {
                            // Не хватило: ждём недостающее на семафоре ОС
                            KernelTypes::Int32 ToAcquire = (OldValue < 0 ? OldValue : 0) - NewValue;
                            for (KernelTypes::Int32 Index = 0; Index < ToAcquire; ++Index)
                            {
                                ThreadingBackend::WaitSemaphore(OsSemaphore);
                            }
                        }
                    }
                }
            }
        }
    }
}
