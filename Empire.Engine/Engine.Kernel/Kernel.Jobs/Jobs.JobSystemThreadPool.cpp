#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystemThreadPool.hpp"

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
                    void JobSystemThreadPool::Init(KernelTypes::UInt32 MaxJobs, KernelTypes::UInt32 MaxBarriers, KernelTypes::Int32 ThreadCount)
                    {
                        JobSystemWithBarrier::Init(MaxBarriers);

                        // Пул задач
                        JobPool.Init(MaxJobs, MaxJobs);

                        // Рабочие потоки
                        StartThreads(ThreadCount);
                    }

                    JobSystemThreadPool::JobSystemThreadPool(KernelTypes::UInt32 MaxJobs, KernelTypes::UInt32 MaxBarriers, KernelTypes::Int32 ThreadCount)
                    {
                        Init(MaxJobs, MaxBarriers, ThreadCount);
                    }

                    void JobSystemThreadPool::StartThreads(KernelTypes::Int32 ThreadCount)
                    {
                        // Автоопределение числа потоков
                        if (ThreadCount < 0)
                        {
                            ThreadCount = static_cast<KernelTypes::Int32>(KernelThreading::GetHardwareThreadCount()) - 1;
                        }

                        // Потоки не нужны
                        if (ThreadCount == 0)
                        {
                            return;
                        }

                        // Потоки не должны завершаться
                        Quit.Store(false, KernelSync::MemoryOrder::Relaxed);

                        // Сбрасываем очередь: создаём служебный узел, единственный в очереди (очередь держит на него ссылку)
                        Node* Dummy = &JobPool.Get(JobPool.ConstructObject("Dummy", JobColor(0), this, JobFunction(), 0));
                        Dummy->AddRef();
                        Dummy->Next.Store(MakePointer(NullIndex, 0), KernelSync::MemoryOrder::Relaxed);
                        KernelTypes::UInt64 DummyPointer = MakePointer(JobPool.GetObjectIndex(Dummy), 0);
                        Head.Store(DummyPointer, KernelSync::MemoryOrder::Relaxed);
                        Tail.Store(DummyPointer, KernelSync::MemoryOrder::Relaxed);

                        // Запускаем потоки
                        KernelAssert::DebugCheck(Threads.Empty(), "JobSystemThreadPool::StartThreads: потоки уже запущены");
                        Threads.Reserve(static_cast<KernelTypes::UInt32>(ThreadCount));
                        WorkerContexts.Resize(static_cast<KernelTypes::UInt32>(ThreadCount)); // после этого адреса не меняются

                        for (KernelTypes::Int32 Index = 0; Index < ThreadCount; ++Index)
                        {
                            WorkerContext& Context = WorkerContexts[static_cast<KernelTypes::UInt32>(Index)];
                            Context.Pool = this;
                            Context.Index = Index;

                            // Имя потока: "Worker N"
                            const KernelTypes::Char Prefix[] = "Worker ";
                            KernelTypes::UInt32 Length = 0;
                            for (; Prefix[Length] != '\0'; ++Length)
                            {
                                Context.Name[Length] = Prefix[Length];
                            }

                            KernelTypes::Char Digits[12];
                            KernelTypes::UInt32 DigitCount = 0;
                            for (KernelTypes::UInt32 Number = static_cast<KernelTypes::UInt32>(Index) + 1; Number > 0 && DigitCount < 12; Number /= 10)
                            {
                                Digits[DigitCount++] = static_cast<KernelTypes::Char>('0' + Number % 10);
                            }

                            while (DigitCount > 0 && Length < sizeof(Context.Name) - 1)
                            {
                                Context.Name[Length++] = Digits[--DigitCount];
                            }

                            Context.Name[Length] = '\0';

                            Threads.EmplaceBack();
                            KernelAssert::Check(Threads.Back().Start(&WorkerEntry, &Context, Context.Name), "JobSystemThreadPool: не удалось создать поток");
                        }
                    }

                    JobSystemThreadPool::~JobSystemThreadPool()
                    {
                        // Останавливаем все рабочие потоки
                        StopThreads();
                    }

                    void JobSystemThreadPool::StopThreads()
                    {
                        if (Threads.Empty())
                        {
                            return;
                        }

                        // Сообщаем потокам, что пора завершаться, и будим их
                        Quit.Store(true, KernelSync::MemoryOrder::Relaxed);
                        WorkSemaphore.Release(Threads.Size());

                        // Ждём завершения
                        for (KernelThreading::Thread& Worker : Threads)
                        {
                            Worker.Join();
                        }

                        Threads.Clear();
                        WorkerContexts.Clear();

                        // В очереди не должно остаться задач
                        ExecuteJobs();

                        // Уничтожаем служебный узел
                        KernelTypes::UInt64 Dummy = Tail.Load(KernelSync::MemoryOrder::Relaxed);
                        JobPool.DestructObject(GetIndex(Dummy));
                    }

                    JobSystem::Job* JobSystemThreadPool::AllocateJob(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, KernelTypes::UInt32 Dependencies)
                    {
                        // Крутимся, пока из пула не получится взять задачу
                        KernelTypes::UInt32 Index;
                        for (;;)
                        {
                            Index = JobPool.ConstructObject(Name, Color, this, Function, Dependencies);
                            if (Index != AvailableJobs::InvalidObjectIndex)
                            {
                                break;
                            }

                            KernelAssert::DebugCheck(false, "JobSystemThreadPool: нет свободных задач");
                            KernelSystem::SleepThread(100);
                        }

                        return &JobPool.Get(Index);
                    }

                    JobHandle JobSystemThreadPool::CreateJob(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, KernelTypes::UInt32 Dependencies)
                    {
                        Job* Created = AllocateJob(Name, Color, Function, Dependencies);

                        // Хендл держит ссылку: задача ставится в очередь ниже и может сразу завершиться
                        JobHandle Handle(Created);

                        // Без зависимостей ставим в очередь сразу
                        if (Dependencies == 0)
                        {
                            QueueJob(Created);
                        }

                        return Handle;
                    }

                    void JobSystemThreadPool::CreateJobs(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, JobHandle* OutJobs, KernelTypes::UInt32 JobCount, KernelTypes::UInt32 Dependencies)
                    {
                        for (KernelTypes::UInt32 Index = 0; Index < JobCount; ++Index)
                        {
                            Job* Created = AllocateJob(Name, Color, Function, Dependencies);
                            OutJobs[Index] = JobHandle(Created);
                        }

                        // Без зависимостей ставим в очередь сразу
                        if (Dependencies == 0)
                        {
                            static_assert(sizeof(JobHandle) == sizeof(Job*), "JobHandle должен весить как указатель");
                            QueueJobs(reinterpret_cast<Job**>(OutJobs), JobCount);
                        }
                    }

                    void JobSystemThreadPool::FreeJob(Job* Target)
                    {
                        JobPool.DestructObject(static_cast<Node*>(Target));
                    }

                    void JobSystemThreadPool::Enqueue(Job* Target)
                    {
                        Node* NodeToAdd = static_cast<Node*>(Target);
                        KernelTypes::UInt32 NodeIndex = JobPool.GetObjectIndex(NodeToAdd);

                        // Две ссылки: одну держит очередь (пока узел не станет служебным), вторую поток, который будет его выполнять
                        NodeToAdd->AddRef(2);

                        // Next = null; счётчик должен отличаться от любого прежнего использования узла
                        NodeToAdd->Next.Store(MakePointer(NullIndex, GetCount(Tail.Load(KernelSync::MemoryOrder::Relaxed))), KernelSync::MemoryOrder::Relaxed);

                        for (;;)
                        {
                            KernelTypes::UInt64 TailValue = Tail.Load(KernelSync::MemoryOrder::Acquire);
                            Node& TailNode = JobPool.Get(GetIndex(TailValue));
                            // Узел мог быть освобождён, но пул фиксированный, так что память остаётся доступной
                            KernelTypes::UInt64 NextValue = TailNode.Next.Load(KernelSync::MemoryOrder::Acquire);

                            if (TailValue == Tail.Load(KernelSync::MemoryOrder::Relaxed)) // согласованы ли хвост и next
                            {
                                if (GetIndex(NextValue) == NullIndex) // хвост указывал на последний узел?
                                {
                                    // Пробуем прицепить узел в конец списка
                                    if (TailNode.Next.CompareExchange(NextValue, MakePointer(NodeIndex, GetCount(NextValue) + 1), KernelSync::MemoryOrder::Release, KernelSync::MemoryOrder::Relaxed))
                                    {
                                        // Вставка готова, пробуем сдвинуть хвост на добавленный узел
                                        Tail.CompareExchange(TailValue, MakePointer(NodeIndex, GetCount(TailValue) + 1), KernelSync::MemoryOrder::Release, KernelSync::MemoryOrder::Relaxed);
                                        return;
                                    }
                                }
                                else
                                {
                                    // Хвост указывал не на последний узел, сдвигаем его на следующий
                                    Tail.CompareExchange(TailValue, MakePointer(GetIndex(NextValue), GetCount(TailValue) + 1), KernelSync::MemoryOrder::Release, KernelSync::MemoryOrder::Relaxed);
                                }
                            }
                        }
                    }

                    JobSystem::Job* JobSystemThreadPool::Dequeue()
                    {
                        for (;;)
                        {
                            KernelTypes::UInt64 HeadValue = Head.Load(KernelSync::MemoryOrder::Acquire);
                            KernelTypes::UInt64 TailValue = Tail.Load(KernelSync::MemoryOrder::Acquire);
                            // Узел мог быть освобождён, но пул фиксированный, так что память остаётся доступной
                            KernelTypes::UInt64 NextValue = JobPool.Get(GetIndex(HeadValue)).Next.Load(KernelSync::MemoryOrder::Acquire);

                            if (HeadValue == Head.Load(KernelSync::MemoryOrder::Relaxed)) // согласованы ли голова, хвост и next
                            {
                                if (GetIndex(HeadValue) == GetIndex(TailValue)) // очередь пуста или хвост отстаёт?
                                {
                                    if (GetIndex(NextValue) == NullIndex)
                                    {
                                        return nullptr; // очередь пуста
                                    }

                                    // Хвост отстаёт, пробуем его продвинуть
                                    Tail.CompareExchange(TailValue, MakePointer(GetIndex(NextValue), GetCount(TailValue) + 1), KernelSync::MemoryOrder::Release, KernelSync::MemoryOrder::Relaxed);
                                }
                                else
                                {
                                    // Сдвигаем голову на следующий узел, он становится новым служебным
                                    if (Head.CompareExchange(HeadValue, MakePointer(GetIndex(NextValue), GetCount(HeadValue) + 1), KernelSync::MemoryOrder::Release, KernelSync::MemoryOrder::Relaxed))
                                    {
                                        // Старого узла в очереди больше нет, отпускаем ссылку очереди на него
                                        JobPool.Get(GetIndex(HeadValue)).Release();

                                        // Вторую ссылку, взятую в Enqueue, держит вызывающий
                                        return &JobPool.Get(GetIndex(NextValue));
                                    }
                                }
                            }
                        }
                    }

                    void JobSystemThreadPool::QueueJob(Job* Target)
                    {
                        // Если рабочих потоков нет, задачу в очередь не поставить. В этом случае предполагается, что она будет добавлена
                        // в барьер и барьер сам выполнит её в Wait().
                        if (Threads.Empty())
                        {
                            return;
                        }

                        Enqueue(Target);

                        // Будим поток
                        WorkSemaphore.Release();
                    }

                    void JobSystemThreadPool::QueueJobs(Job** Jobs, KernelTypes::UInt32 JobCount)
                    {
                        if (JobCount == 0)
                        {
                            return;
                        }

                        // См. комментарий в QueueJob
                        if (Threads.Empty())
                        {
                            return;
                        }

                        for (Job** Current = Jobs, **End = Jobs + JobCount; Current < End; ++Current)
                        {
                            Enqueue(*Current);
                        }

                        // Будим потоки (не больше, чем есть потоков)
                        WorkSemaphore.Release(KernelTypes::Min<KernelTypes::UInt32>(JobCount, Threads.Size()));
                    }

                    void JobSystemThreadPool::ExecuteJobs()
                    {
                        while (Job* Next = Dequeue())
                        {
                            // Выполняем задачу
                            Next->Execute();
                            Next->Release();
                        }
                    }

                    void JobSystemThreadPool::WorkerEntry(void* Context)
                    {
                        WorkerContext* Worker = static_cast<WorkerContext*>(Context);
                        Worker->Pool->ThreadMain(Worker->Index);
                    }

                    void JobSystemThreadPool::ThreadMain(KernelTypes::Int32 ThreadIndex)
                    {
                        // Включение исключений с плавающей точкой (FPExceptionsEnable в Jolt) будет добавлено вместе с FP-управлением в Kernel.Compiler.

                        // Функция инициализации потока
                        ThreadInitFunction(ThreadIndex);

                        while (!Quit.Load(KernelSync::MemoryOrder::Relaxed))
                        {
                            // Ждём задачи
                            WorkSemaphore.Acquire();

                            ExecuteJobs();
                        }

                        // Функция завершения потока
                        ThreadExitFunction(ThreadIndex);
                    }
                }
            }
        }
    }
}
