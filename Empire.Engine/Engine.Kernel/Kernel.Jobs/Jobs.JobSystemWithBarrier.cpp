#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystemWithBarrier.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.System/System.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelJobs
                {
                    JobSystemWithBarrier::BarrierImpl::BarrierImpl()
                    {
                        for (KernelSync::Atomic<Job*>& Slot : Jobs)
                        {
                            Slot.Store(nullptr, KernelSync::MemoryOrder::Relaxed);
                        }
                    }

                    JobSystemWithBarrier::BarrierImpl::~BarrierImpl()
                    {
                        KernelAssert::DebugCheck(IsEmpty(), "BarrierImpl: уничтожение барьера с задачами");
                    }

                    void JobSystemWithBarrier::BarrierImpl::AddJob(const JobHandle& Handle)
                    {
                        // Сначала увеличиваем NumToAcquire, чтобы барьер знал, что надо ждать. Если сделать это после SetBarrier,
                        // задача может успеть завершиться и вызвать OnJobFinished до увеличения, и WaitForJobs вернётся слишком рано.
                        NumToAcquire.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);

                        // Если барьер привязался, задача ещё не готова. Иначе она уже завершена и в список её добавлять не нужно.
                        Job* Target = Handle.GetPtr();
                        if (Target->SetBarrier(this))
                        {
                            KernelTypes::Bool ReleaseSemaphore = false;

                            // Если задачу можно запускать, освобождаем семафор лишний раз, чтобы ждущий поток мог начать её выполнять
                            if (Target->CanBeExecuted())
                            {
                                ReleaseSemaphore = true;
                                NumToAcquire.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                            }

                            // Добавляем задачу в список
                            Target->AddRef();
                            KernelTypes::UInt32 WriteIndex = JobWriteIndex.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                            while (WriteIndex - JobReadIndex.Load(KernelSync::MemoryOrder::Acquire) >= MaxJobs)
                            {
                                KernelAssert::DebugCheck(false, "Барьер переполнен, ожидание");
                                KernelSystem::SleepThread(100);
                            }

                            Jobs[WriteIndex & (MaxJobs - 1)].Store(Target, KernelSync::MemoryOrder::Release);

                            // Будим ждущий поток: появилась задача, которую можно выполнять
                            if (ReleaseSemaphore)
                            {
                                Semaphore.Release();
                            }
                        }
                        else
                        {
                            // Задача уже готова, в барьер не добавлена, поэтому NumToAcquire возвращаем назад.
                            // Wait() теоретически мог бы повиснуть, если задача закончилась, а ждущий поток уже пошёл на новый круг ожидания
                            // семафора. Но AddJob вызывают либо до Wait, либо из задачи этого же барьера, а такая задача при завершении гарантированно разбудит семафор.
                            NumToAcquire.FetchSub(1, KernelSync::MemoryOrder::Relaxed);
                        }
                    }

                    void JobSystemWithBarrier::BarrierImpl::AddJobs(const JobHandle* Handles, KernelTypes::UInt32 HandleCount)
                    {
                        KernelTypes::Bool ReleaseSemaphore = false;

                        for (const JobHandle* Handle = Handles, *HandlesEnd = Handles + HandleCount; Handle < HandlesEnd; ++Handle)
                        {
                            // См. комментарии в AddJob
                            NumToAcquire.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);

                            Job* Target = Handle->GetPtr();
                            if (Target->SetBarrier(this))
                            {
                                if (!ReleaseSemaphore && Target->CanBeExecuted())
                                {
                                    ReleaseSemaphore = true;
                                    NumToAcquire.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                                }

                                Target->AddRef();
                                KernelTypes::UInt32 WriteIndex = JobWriteIndex.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                                while (WriteIndex - JobReadIndex.Load(KernelSync::MemoryOrder::Acquire) >= MaxJobs)
                                {
                                    KernelAssert::DebugCheck(false, "Барьер переполнен, ожидание");
                                    KernelSystem::SleepThread(100);
                                }

                                Jobs[WriteIndex & (MaxJobs - 1)].Store(Target, KernelSync::MemoryOrder::Release);
                            }
                            else
                            {
                                NumToAcquire.FetchSub(1, KernelSync::MemoryOrder::Relaxed);
                            }
                        }

                        if (ReleaseSemaphore)
                        {
                            Semaphore.Release();
                        }
                    }

                    void JobSystemWithBarrier::BarrierImpl::OnJobFinished(Job*)
                    {
                        Semaphore.Release();
                    }

                    void JobSystemWithBarrier::BarrierImpl::Wait()
                    {
                        while (NumToAcquire.Load(KernelSync::MemoryOrder::Relaxed) > 0)
                        {
                            {
                                // Выполняем задачи
                                KernelTypes::Bool HasExecuted;
                                do
                                {
                                    HasExecuted = false;

                                    // Убираем из начала списка завершённые задачи. Индексы сравниваем через !=, потому что они могут переходить через край.
                                    while (JobReadIndex.Load(KernelSync::MemoryOrder::Relaxed) != JobWriteIndex.Load(KernelSync::MemoryOrder::Relaxed))
                                    {
                                        KernelSync::Atomic<Job*>& Slot = Jobs[JobReadIndex.Load(KernelSync::MemoryOrder::Relaxed) & (MaxJobs - 1)];
                                        Job* JobPointer = Slot.Load(KernelSync::MemoryOrder::Acquire); // Acquire: видим содержимое задачи
                                        if (JobPointer == nullptr || !JobPointer->IsDone())
                                        {
                                            break;
                                        }

                                        // Задача завершена, отпускаем
                                        JobPointer->Release();
                                        Slot.Store(nullptr, KernelSync::MemoryOrder::Relaxed);
                                        JobReadIndex.FetchAdd(1, KernelSync::MemoryOrder::Release); // Release: запись nullptr выше упорядочена до того, как слот переиспользует писатель
                                    }

                                    // Выполняем первую запускаемую задачу
                                    for (KernelTypes::UInt32 Index = JobReadIndex.Load(KernelSync::MemoryOrder::Relaxed), EndIndex = JobWriteIndex.Load(KernelSync::MemoryOrder::Relaxed); Index != EndIndex; ++Index)
                                    {
                                        const KernelSync::Atomic<Job*>& Slot = Jobs[Index & (MaxJobs - 1)];
                                        Job* JobPointer = Slot.Load(KernelSync::MemoryOrder::Acquire);
                                        if (JobPointer != nullptr && JobPointer->CanBeExecuted())
                                        {
                                            // Выполнится, только если ещё не выполнялась
                                            JobPointer->Execute();
                                            HasExecuted = true;
                                            break;
                                        }
                                    }
                                }
                                while (HasExecuted);
                            }

                            // Ждём, пока другой поток разбудит нас: либо появилась работа, либо все задачи завершены.
                            // Если освобождений было несколько, забираем их разом, чтобы не крутиться впустую.
                            // GetValue может вернуть устаревшее значение, но это безопасно: семафор захватываем только здесь,
                            // а остальные потоки его лишь освобождают, значит прочитанное не больше настоящего.
                            KernelTypes::Int32 CurrentValue = Semaphore.GetValue();
                            KernelTypes::Int32 ToAcquire = CurrentValue > 1 ? CurrentValue : 1;
                            Semaphore.Acquire(static_cast<KernelTypes::UInt32>(ToAcquire));
                            NumToAcquire.FetchSub(ToAcquire, KernelSync::MemoryOrder::Relaxed);
                        }

                        KernelAssert::DebugCheck(Semaphore.GetValue() == 0, "Барьер: семафор не пуст после ожидания");
                        KernelAssert::DebugCheck(NumToAcquire.Load(KernelSync::MemoryOrder::Relaxed) == 0, "Барьер: NumToAcquire не ноль после ожидания");

                        // Все задачи завершены, отпускаем их
                        while (JobReadIndex.Load(KernelSync::MemoryOrder::Relaxed) != JobWriteIndex.Load(KernelSync::MemoryOrder::Relaxed))
                        {
                            KernelSync::Atomic<Job*>& Slot = Jobs[JobReadIndex.Load(KernelSync::MemoryOrder::Relaxed) & (MaxJobs - 1)];
                            Job* JobPointer = Slot.Load(KernelSync::MemoryOrder::Acquire);
                            KernelAssert::DebugCheck(JobPointer != nullptr && JobPointer->IsDone(), "Барьер: ожидалась завершённая задача");
                            JobPointer->Release();
                            Slot.Store(nullptr, KernelSync::MemoryOrder::Relaxed);
                            JobReadIndex.FetchAdd(1, KernelSync::MemoryOrder::Release);
                        }
                    }

                    void JobSystemWithBarrier::Init(KernelTypes::UInt32 MaxBarriers)
                    {
                        KernelAssert::DebugCheck(Barriers == nullptr, "JobSystemWithBarrier::Init: уже подготовлено");

                        MaxBarrierCount = MaxBarriers;
                        Barriers = new BarrierImpl[MaxBarriers];
                    }

                    JobSystemWithBarrier::JobSystemWithBarrier(KernelTypes::UInt32 MaxBarriers)
                    {
                        Init(MaxBarriers);
                    }

                    JobSystemWithBarrier::~JobSystemWithBarrier()
                    {
                        // Ни один барьер не должен быть выдан
                        if constexpr (KernelCompiler::IsDebugBuild)
                        {
                            for (const BarrierImpl* Current = Barriers, *End = Barriers + MaxBarrierCount; Current < End; ++Current)
                            {
                                KernelAssert::DebugCheck(!Current->InUse.Load(KernelSync::MemoryOrder::Relaxed), "JobSystemWithBarrier: уничтожение с выданными барьерами");
                            }
                        }

                        delete[] Barriers;
                    }

                    JobSystem::Barrier* JobSystemWithBarrier::CreateBarrier()
                    {
                        // Ищем первый свободный барьер
                        for (KernelTypes::UInt32 Index = 0; Index < MaxBarrierCount; ++Index)
                        {
                            KernelTypes::Bool Expected = false;
                            if (Barriers[Index].InUse.CompareExchange(Expected, true, KernelSync::MemoryOrder::Acquire, KernelSync::MemoryOrder::Relaxed))
                            {
                                return &Barriers[Index];
                            }
                        }

                        return nullptr;
                    }

                    void JobSystemWithBarrier::DestroyBarrier(Barrier* Target)
                    {
                        KernelAssert::DebugCheck(static_cast<BarrierImpl*>(Target)->IsEmpty(), "DestroyBarrier: в барьере остались задачи");

                        // Помечаем барьер свободным
                        KernelTypes::Bool Expected = true;
                        [[maybe_unused]] KernelTypes::Bool Released = static_cast<BarrierImpl*>(Target)->InUse.CompareExchange(Expected, false, KernelSync::MemoryOrder::Release, KernelSync::MemoryOrder::Relaxed);
                        KernelAssert::DebugCheck(Released, "DestroyBarrier: барьер не был выдан");
                    }

                    void JobSystemWithBarrier::WaitForJobs(Barrier* Target)
                    {
                        static_cast<BarrierImpl*>(Target)->Wait();
                    }
                }
            }
        }
    }
}
