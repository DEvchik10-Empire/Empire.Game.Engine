#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystemSingleThreaded.hpp"

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
                    void JobSystemSingleThreaded::Init(KernelTypes::UInt32 MaxJobs)
                    {
                        JobPool.Init(MaxJobs, MaxJobs);
                    }

                    JobHandle JobSystemSingleThreaded::CreateJob(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, KernelTypes::UInt32 Dependencies)
                    {
                        KernelTypes::UInt32 Index = JobPool.ConstructObject(Name, Color, this, Function, Dependencies);
                        KernelAssert::Check(Index != AvailableJobs::InvalidObjectIndex, "JobSystemSingleThreaded: закончились задачи");
                        Job* Created = &JobPool.Get(Index);

                        // Хендл держит ссылку: задача ставится в очередь ниже и сразу завершается
                        JobHandle Handle(Created);

                        if (Dependencies == 0)
                        {
                            QueueJob(Created);
                        }

                        return Handle;
                    }

                    void JobSystemSingleThreaded::CreateJobs(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, JobHandle* OutJobs, KernelTypes::UInt32 JobCount, KernelTypes::UInt32 Dependencies)
                    {
                        for (KernelTypes::UInt32 Index = 0; Index < JobCount; ++Index)
                        {
                            OutJobs[Index] = CreateJob(Name, Color, Function, Dependencies);
                        }
                    }

                    void JobSystemSingleThreaded::FreeJob(Job* Target)
                    {
                        JobPool.DestructObject(Target);
                    }

                    void JobSystemSingleThreaded::QueueJob(Job* Target)
                    {
                        Target->Execute();
                    }

                    void JobSystemSingleThreaded::QueueJobs(Job** Jobs, KernelTypes::UInt32 JobCount)
                    {
                        for (KernelTypes::UInt32 Index = 0; Index < JobCount; ++Index)
                        {
                            QueueJob(Jobs[Index]);
                        }
                    }

                    JobSystem::Barrier* JobSystemSingleThreaded::CreateBarrier()
                    {
                        return &DummyBarrier;
                    }

                    void JobSystemSingleThreaded::DestroyBarrier(Barrier*)
                    {
                    }

                    void JobSystemSingleThreaded::WaitForJobs(Barrier*)
                    {
                    }
                }
            }
        }
    }
}
