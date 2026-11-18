#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FixedSizeFreeList.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystem.hpp"

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
                    // Планировщик без потоков: задача выполняется сразу, как только её можно запускать (перенесён из Jolt, Core/JobSystemSingleThreaded.h)
                    class JobSystemSingleThreaded final : public JobSystem
                    {
                        public:
                            JobSystemSingleThreaded() = default;

                            explicit JobSystemSingleThreaded(KernelTypes::UInt32 MaxJobs)
                            {
                                Init(MaxJobs);
                            }

                            // MaxJobs это наибольшее число задач, существующих одновременно, степень двойки
                            void Init(KernelTypes::UInt32 MaxJobs);

                            KernelTypes::Int32 GetMaxConcurrency() const override
                            {
                                return 1;
                            }

                            JobHandle CreateJob(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, KernelTypes::UInt32 Dependencies = 0) override;
                            void CreateJobs(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, JobHandle* OutJobs, KernelTypes::UInt32 JobCount, KernelTypes::UInt32 Dependencies = 0) override;
                            Barrier* CreateBarrier() override;
                            void DestroyBarrier(Barrier* Target) override;
                            void WaitForJobs(Barrier* Target) override;

                        protected:
                            // Заглушка барьера: все задачи уже выполнены
                            class BarrierImpl : public Barrier
                            {
                                public:
                                    void AddJob(const JobHandle&) override
                                    {
                                    }

                                    void AddJobs(const JobHandle*, KernelTypes::UInt32) override
                                    {
                                    }

                                protected:
                                    void OnJobFinished(Job*) override
                                    {
                                    }
                            };

                            void QueueJob(Job* Target) override;
                            void QueueJobs(Job** Jobs, KernelTypes::UInt32 JobCount) override;
                            void FreeJob(Job* Target) override;

                            // Один общий барьер, потому что он ничего не делает
                            BarrierImpl DummyBarrier;

                            // Задачи (фиксированное число)
                            using AvailableJobs = KernelContainers::FixedSizeFreeList<Job>;
                            AvailableJobs JobPool;
                    };
                }
            }
        }
    }
}
