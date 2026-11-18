#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystem.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Threading/Threading.hpp"

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
                    // Готовая реализация барьера для планировщика (перенесена из Jolt, Core/JobSystemWithBarrier.h).
                    // Облегчает написание своего планировщика: остаются только GetMaxConcurrency, CreateJob, CreateJobs, FreeJob и QueueJob/QueueJobs.
                    class JobSystemWithBarrier : public JobSystem
                    {
                        public:
                            // Создаёт барьеры, см. Init
                            explicit JobSystemWithBarrier(KernelTypes::UInt32 MaxBarriers);
                            JobSystemWithBarrier() = default;
                            ~JobSystemWithBarrier() override;

                            // Подготовить барьеры. MaxBarriers это наибольшее число барьеров, существующих одновременно.
                            void Init(KernelTypes::UInt32 MaxBarriers);

                            Barrier* CreateBarrier() override;
                            void DestroyBarrier(Barrier* Target) override;
                            void WaitForJobs(Barrier* Target) override;

                        private:
                            class BarrierImpl : public Barrier
                            {
                                public:
                                    BarrierImpl();
                                    ~BarrierImpl() override;

                                    void AddJob(const JobHandle& Handle) override;
                                    void AddJobs(const JobHandle* Handles, KernelTypes::UInt32 HandleCount) override;

                                    // Нет ли в барьере задач
                                    KernelTypes::Bool IsEmpty() const
                                    {
                                        return JobReadIndex.Load(KernelSync::MemoryOrder::Relaxed) == JobWriteIndex.Load(KernelSync::MemoryOrder::Relaxed);
                                    }

                                    // Ждать все задачи барьера, а пока ждём, выполнять его задачи в текущем потоке
                                    void Wait();

                                    // Выдан ли барьер
                                    KernelSync::Atomic<KernelTypes::Bool> InUse{false};

                                protected:
                                    // Вызывается задачей, когда она закончилась
                                    void OnJobFinished(Job* Finished) override;

                                    // Очередь задач барьера
                                    static constexpr KernelTypes::UInt32 MaxJobs = 2048;
                                    static_assert(KernelTypes::IsPowerOfTwo(MaxJobs), "MaxJobs должно быть степенью двойки (используются битовые операции)");

                                    KernelSync::Atomic<Job*> Jobs[MaxJobs];                             // задачи барьера, nullptr в пустых слотах

                                    // Первая возможно действительная задача (по модулю MaxJobs). Может быть nullptr, если другой поток ещё добавляет задачу.
                                    alignas(KernelCompiler::CacheLineSize) KernelSync::Atomic<KernelTypes::UInt32> JobReadIndex{0};

                                    // Первый слот, куда можно писать (по модулю MaxJobs)
                                    alignas(KernelCompiler::CacheLineSize) KernelSync::Atomic<KernelTypes::UInt32> JobWriteIndex{0};

                                    // Сколько раз семафор уже освобождён, столько раз барьер должен его захватить (пишется вместе с JobWriteIndex, поэтому в той же строке кэша)
                                    KernelSync::Atomic<KernelTypes::Int32> NumToAcquire{0};

                                    // Семафор, которым заканчивающиеся задачи сообщают барьеру, что они готовы
                                    KernelThreading::Semaphore Semaphore;
                            };

                            // Барьеры создаются заранее и живут всё время (создание семафора недешёвое)
                            KernelTypes::UInt32 MaxBarrierCount = 0;
                            BarrierImpl* Barriers = nullptr;
                    };
                }
            }
        }
    }
}
