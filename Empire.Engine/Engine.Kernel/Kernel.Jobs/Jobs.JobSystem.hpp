#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Function.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.Reference.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Profiler/Profiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

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
                    // Цвет задачи в профайлере, 0xAARRGGBB.
                    using JobColor = KernelTypes::UInt32;

                    // Интерфейс планировщика задач (перенесён из Jolt, Core/JobSystem.h).
                    //
                    // Задача (Job) это функция, которая запускается, когда счётчик её зависимостей станет нулём. Задачи хранятся в планировщике,
                    // а снаружи на них ссылаются через JobHandle. Барьер (Barrier) собирает группу задач и позволяет дождаться их всех.
                    //
                    // Чтобы написать свой планировщик, реализуй:
                    //  * GetMaxConcurrency, сколько задач может идти одновременно;
                    //  * CreateJob, CreateJobs: создание задач (при нуле зависимостей задача сразу ставится в очередь через QueueJob);
                    //  * QueueJob, QueueJobs: поставить задачу в очередь на выполнение (зависимости ведёт сам Job, это вызывается, когда задачу можно запускать);
                    //  * FreeJob: освободить задачу, когда на неё не осталось ссылок;
                    //  * CreateBarrier, DestroyBarrier, WaitForJobs: барьеры. Если не хочется писать барьер, наследуйся от JobSystemWithBarrier
                    //    и реализуй только первые четыре пункта.
                    // Готовые реализации: JobSystemThreadPool, JobSystemSingleThreaded.
                    class JobSystem : public KernelTypes::NonCopyable
                    {
                        protected:
                            class Job;

                        public:
                            // Ссылка на задачу. Задача удаляется, когда на неё не осталось JobHandle и она не стоит в очереди и не выполняется.
                            class JobHandle : private KernelMemory::Ref<Job>
                            {
                                public:
                                    JobHandle() = default;
                                    JobHandle(const JobHandle&) = default;

                                    JobHandle(JobHandle&& Other) noexcept
                                        : KernelMemory::Ref<Job>(static_cast<KernelMemory::Ref<Job>&&>(Other))
                                    {
                                    }

                                    // Только для планировщика
                                    explicit JobHandle(Job* Target)
                                        : KernelMemory::Ref<Job>(Target)
                                    {
                                    }

                                    JobHandle& operator=(const JobHandle&) = default;
                                    JobHandle& operator=(JobHandle&&) noexcept = default;

                                    // Есть ли в хендле задача
                                    KernelTypes::Bool IsValid() const
                                    {
                                        return GetPtr() != nullptr;
                                    }

                                    // Завершилась ли задача
                                    KernelTypes::Bool IsDone() const
                                    {
                                        return GetPtr() != nullptr && GetPtr()->IsDone();
                                    }

                                    // Увеличить счётчик зависимостей
                                    void AddDependency(KernelTypes::Int32 Count = 1) const
                                    {
                                        GetPtr()->AddDependency(Count);
                                    }

                                    // Уменьшить счётчик зависимостей. Когда он дойдёт до нуля, задача запустится, и после этого
                                    // вызывать AddDependency и RemoveDependency для неё нельзя.
                                    void RemoveDependency(KernelTypes::Int32 Count = 1) const
                                    {
                                        GetPtr()->RemoveDependencyAndQueue(Count);
                                    }

                                    // Уменьшить зависимости сразу у пачки задач. Это быстрее, чем по одной, потому что очередь блокируется реже.
                                    static void RemoveDependencies(const JobHandle* Handles, KernelTypes::UInt32 HandleCount, KernelTypes::Int32 Count = 1);

                                    template <KernelTypes::UInt32 N>
                                    static void RemoveDependencies(KernelContainers::StaticArray<JobHandle, N>& Handles, KernelTypes::Int32 Count = 1)
                                    {
                                        RemoveDependencies(Handles.Data(), Handles.Size(), Count);
                                    }

                                    // Только для планировщика
                                    using KernelMemory::Ref<Job>::GetPtr;
                            };

                            // Барьер следит за набором задач и позволяет дождаться их завершения.
                            class Barrier : public KernelTypes::NonCopyable
                            {
                                public:
                                    // Добавить задачу. Задачи можно добавлять и пока кто-то уже ждёт на барьере.
                                    virtual void AddJob(const JobHandle& Handle) = 0;

                                    // Добавить несколько задач
                                    virtual void AddJobs(const JobHandle* Handles, KernelTypes::UInt32 HandleCount) = 0;

                                protected:
                                    // Job должен уметь вызывать OnJobFinished
                                    friend class Job;

                                    // Барьер удаляют через JobSystem::DestroyBarrier, а не напрямую
                                    virtual ~Barrier() = default;

                                    // Вызывается задачей, когда она закончилась
                                    virtual void OnJobFinished(Job* Finished) = 0;
                            };

                            // Тело задачи
                            using JobFunction = KernelContainers::Function<void()>;

                            virtual ~JobSystem() = default;

                            // Наибольшее число одновременно выполняемых задач
                            virtual KernelTypes::Int32 GetMaxConcurrency() const = 0;

                            // Создать задачу. При Dependencies == 0 она запускается сразу, иначе когда RemoveDependency доведёт счётчик до нуля.
                            virtual JobHandle CreateJob(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, KernelTypes::UInt32 Dependencies = 0) = 0;

                            // Создать JobCount одинаковых задач, остальное как в CreateJob
                            virtual void CreateJobs(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, JobHandle* OutJobs, KernelTypes::UInt32 JobCount, KernelTypes::UInt32 Dependencies = 0) = 0;

                            // Создать барьер
                            virtual Barrier* CreateBarrier() = 0;

                            // Уничтожить барьер, когда он больше не нужен (он уже должен быть пустым)
                            virtual void DestroyBarrier(Barrier* Target) = 0;

                            // Дождаться завершения задач барьера. На одном барьере одновременно может ждать только один поток.
                            virtual void WaitForJobs(Barrier* Target) = 0;

                        protected:
                            // Единица работы
                            class Job
                            {
                                public:
                                    Job(const KernelTypes::Char* JobName, JobColor ColorValue, JobSystem* OwnerSystem, const JobFunction& Function, KernelTypes::UInt32 Dependencies)
                                        : Body(Function), Name(JobName), Color(ColorValue), System(OwnerSystem), DependencyCount(Dependencies)
                                    {
                                    }

                                    // Планировщик, которому принадлежит задача
                                    JobSystem* GetJobSystem()
                                    {
                                        return System;
                                    }

                                    // Добавить ссылку (достаточно ослабленного порядка памяти)
                                    void AddRef(KernelTypes::UInt32 Count = 1)
                                    {
                                        ReferenceCount.FetchAdd(Count, KernelSync::MemoryOrder::Relaxed);
                                    }

                                    // Отпустить ссылку, на последней задача освобождается планировщиком
                                    void Release()
                                    {
                                        if constexpr (!KernelCompiler::IsThreadSanitizer)
                                        {
                                            // Release при уменьшении, а Acquire-барьер перед освобождением, чтобы видеть всё, что сделали другие потоки до своего Release
                                            if (ReferenceCount.FetchSub(1, KernelSync::MemoryOrder::Release) == 1)
                                            {
                                                KernelCompiler::AtomicThreadFence(KernelSync::MemoryOrder::Acquire);
                                                System->FreeJob(this);
                                            }
                                        }
                                        else
                                        {
                                            // ThreadSanitizer не понимает отдельные барьеры, поэтому Acquire-Release на каждую операцию
                                            if (ReferenceCount.FetchSub(1, KernelSync::MemoryOrder::AcquireRelease) == 1)
                                            {
                                                System->FreeJob(this);
                                            }
                                        }
                                    }

                                    // Увеличить счётчик зависимостей
                                    void AddDependency(KernelTypes::Int32 Count);

                                    // Уменьшить счётчик зависимостей, возвращает true, когда он дошёл до нуля (после этого счётчик трогать нельзя)
                                    KernelTypes::Bool RemoveDependency(KernelTypes::Int32 Count);

                                    // То же, но при нуле задача ставится в очередь
                                    void RemoveDependencyAndQueue(KernelTypes::Int32 Count);

                                    // Привязать задачу к барьеру. Возвращает false, если задача уже завершилась.
                                    KernelTypes::Bool SetBarrier(Barrier* Target)
                                    {
                                        // Acquire при неудаче: если задача уже закончилась, вызывающий видит все её побочные эффекты
                                        KernelTypes::IntPtr Expected = 0;
                                        if (BarrierPointer.CompareExchange(Expected, reinterpret_cast<KernelTypes::IntPtr>(Target), KernelSync::MemoryOrder::Release, KernelSync::MemoryOrder::Acquire))
                                        {
                                            return true;
                                        }

                                        KernelAssert::DebugCheck(Expected == BarrierDoneState, "Job: задача может принадлежать только одному барьеру");
                                        return false;
                                    }

                                    // Выполнить задачу. Возвращает сколько у неё ещё зависимостей, либо ExecutingState или DoneState.
                                    KernelTypes::UInt32 Execute()
                                    {
                                        // В выполнение можно перейти только при нуле зависимостей
                                        KernelTypes::UInt32 State = 0;
                                        if (!DependencyCount.CompareExchange(State, ExecutingState, KernelSync::MemoryOrder::Acquire))
                                        {
                                            return State; // CompareExchange записал сюда текущее значение
                                        }

                                        {
                                            KernelProfiler::Scope Measurement(Name, Color);
                                            Body();
                                        }

                                        // Тело задачи может держать захваченное состояние, поэтому уничтожаем его здесь, до уведомления барьера:
                                        // после уведомления это состояние уже могло бы указывать на удалённые данные.
                                        Body = JobFunction();

                                        // Меняем указатель на барьер на состояние "готово": после этого барьер уже не привяжется
                                        KernelTypes::IntPtr BarrierValue = BarrierPointer.Load(KernelSync::MemoryOrder::Relaxed);
                                        // Acquire в паре с Release в SetBarrier, Release чтобы неудавшийся SetBarrier видел побочные эффекты тела
                                        while (!BarrierPointer.CompareExchange(BarrierValue, BarrierDoneState, KernelSync::MemoryOrder::AcquireRelease, KernelSync::MemoryOrder::Relaxed))
                                        {
                                        }

                                        KernelAssert::DebugCheck(BarrierValue != BarrierDoneState, "Job: задача уже была завершена");

                                        // Помечаем задачу выполненной. Release: поток, увидевший IsDone, видит и побочные эффекты тела.
                                        State = ExecutingState;
                                        [[maybe_unused]] KernelTypes::Bool Marked = DependencyCount.CompareExchange(State, DoneState, KernelSync::MemoryOrder::Release);
                                        KernelAssert::DebugCheck(Marked, "Job: неожиданное состояние при завершении");

                                        // Барьер уведомляем после смены состояния, чтобы получивший уведомление поток видел, что задача завершена
                                        if (BarrierValue != 0)
                                        {
                                            reinterpret_cast<Barrier*>(BarrierValue)->OnJobFinished(this);
                                        }

                                        return DoneState;
                                    }

                                    // Можно ли запускать задачу
                                    KernelTypes::Bool CanBeExecuted() const
                                    {
                                        return DependencyCount.Load(KernelSync::MemoryOrder::Relaxed) == 0;
                                    }

                                    // Завершилась ли задача. Если true, побочные эффекты тела видны вызывающему потоку.
                                    KernelTypes::Bool IsDone() const
                                    {
                                        return DependencyCount.Load(KernelSync::MemoryOrder::Acquire) == DoneState;
                                    }

                                    static constexpr KernelTypes::UInt32 ExecutingState = 0xe0e0e0e0;   // значение счётчика зависимостей, пока задача выполняется
                                    static constexpr KernelTypes::UInt32 DoneState = 0xd0d0d0d0;        // значение счётчика после завершения
                                    static constexpr KernelTypes::IntPtr BarrierDoneState = ~KernelTypes::IntPtr(0); // значение указателя на барьер, когда барьер уже сработал

                                private:
                                    JobFunction Body;                                                   // тело задачи (первое поле: занимает целую строку кэша)
                                    const KernelTypes::Char* Name;                                      // имя для профайлера
                                    JobColor Color;                                                      // цвет для профайлера
                                    JobSystem* System;                                                  // планировщик, которому принадлежит задача
                                    KernelSync::Atomic<KernelTypes::IntPtr> BarrierPointer{0};          // барьер этой задачи (указатель на Barrier)
                                    KernelSync::Atomic<KernelTypes::UInt32> ReferenceCount{0};          // число JobHandle на эту задачу
                                    KernelSync::Atomic<KernelTypes::UInt32> DependencyCount;            // сколько задач должны завершиться до запуска этой
                            };

                            // Поставить задачу в очередь
                            virtual void QueueJob(Job* Target) = 0;

                            // Поставить в очередь сразу несколько задач
                            virtual void QueueJobs(Job** Jobs, KernelTypes::UInt32 JobCount) = 0;

                            // Освободить задачу
                            virtual void FreeJob(Job* Target) = 0;
                    };

                    using JobHandle = JobSystem::JobHandle;

                    inline void JobSystem::Job::AddDependency(KernelTypes::Int32 Count)
                    {
                        [[maybe_unused]] KernelTypes::UInt32 OldValue = DependencyCount.FetchAdd(static_cast<KernelTypes::UInt32>(Count), KernelSync::MemoryOrder::Relaxed);
                        KernelAssert::DebugCheck(OldValue > 0 && OldValue != ExecutingState && OldValue != DoneState, "Job: задача в очереди, выполняется или завершена, зависимость добавлять нельзя");
                    }

                    inline KernelTypes::Bool JobSystem::Job::RemoveDependency(KernelTypes::Int32 Count)
                    {
                        KernelTypes::UInt32 OldValue = DependencyCount.FetchSub(static_cast<KernelTypes::UInt32>(Count), KernelSync::MemoryOrder::Release);
                        KernelAssert::DebugCheck(OldValue != ExecutingState && OldValue != DoneState, "Job: задача выполняется или завершена, зависимость убирать нельзя");
                        KernelTypes::UInt32 NewValue = OldValue - static_cast<KernelTypes::UInt32>(Count);
                        KernelAssert::DebugCheck(OldValue > NewValue, "Job: счётчик зависимостей ушёл ниже нуля");
                        return NewValue == 0;
                    }

                    inline void JobSystem::Job::RemoveDependencyAndQueue(KernelTypes::Int32 Count)
                    {
                        if (RemoveDependency(Count))
                        {
                            System->QueueJob(this);
                        }
                    }

                    inline void JobSystem::JobHandle::RemoveDependencies(const JobHandle* Handles, KernelTypes::UInt32 HandleCount, KernelTypes::Int32 Count)
                    {
                        KernelAssert::DebugCheck(HandleCount > 0, "JobHandle::RemoveDependencies: пустой список");

                        // Все задачи принадлежат одному планировщику
                        JobSystem* System = Handles->GetPtr()->GetJobSystem();

                        // Готовые к запуску копим в небольшом буфере и ставим в очередь пачками (без alloca, чтобы не выходить за стек)
                        constexpr KernelTypes::UInt32 BatchSize = 64;
                        Job* ToQueue[BatchSize];
                        KernelTypes::UInt32 QueuedCount = 0;

                        for (const JobHandle* Handle = Handles, *End = Handles + HandleCount; Handle < End; ++Handle)
                        {
                            Job* Target = Handle->GetPtr();
                            KernelAssert::DebugCheck(Target->GetJobSystem() == System, "JobHandle::RemoveDependencies: задачи из разных планировщиков");

                            if (Target->RemoveDependency(Count))
                            {
                                ToQueue[QueuedCount++] = Target;

                                if (QueuedCount == BatchSize)
                                {
                                    System->QueueJobs(ToQueue, QueuedCount);
                                    QueuedCount = 0;
                                }
                            }
                        }

                        if (QueuedCount != 0)
                        {
                            System->QueueJobs(ToQueue, QueuedCount);
                        }
                    }
                }
            }
        }
    }
}
