#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FixedSizeFreeList.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystemWithBarrier.hpp"
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
                    // Планировщик на пуле потоков (перенесён из Jolt, Core/JobSystemThreadPool.h).
                    // В Jolt это пример реализации: при желании её можно заменить своей, которая работает поверх другой системы задач.
                    class JobSystemThreadPool final : public JobSystemWithBarrier
                    {
                        public:
                            // Создаёт пул потоков, см. Init
                            JobSystemThreadPool(KernelTypes::UInt32 MaxJobs, KernelTypes::UInt32 MaxBarriers, KernelTypes::Int32 ThreadCount = -1);
                            JobSystemThreadPool() = default;
                            ~JobSystemThreadPool() override;

                            // Функции, вызываемые при запуске и завершении рабочего потока (получают его индекс). Задавать до Init().
                            using InitExitFunction = KernelContainers::Function<void(KernelTypes::Int32)>;

                            void SetThreadInitFunction(const InitExitFunction& InitFunction)
                            {
                                ThreadInitFunction = InitFunction;
                            }

                            void SetThreadExitFunction(const InitExitFunction& ExitFunction)
                            {
                                ThreadExitFunction = ExitFunction;
                            }

                            // Подготовить пул.
                            // MaxJobs: наибольшее число задач одновременно, степень двойки (в очереди реально на одну меньше, потому что в ней всегда лежит служебный узел).
                            // MaxBarriers: наибольшее число барьеров одновременно.
                            // ThreadCount: сколько потоков запустить (одновременно идёт на одну задачу больше, потому что главный поток тоже выполняет задачи,
                            // пока ждёт барьер). -1 означает "по числу процессоров минус один".
                            void Init(KernelTypes::UInt32 MaxJobs, KernelTypes::UInt32 MaxBarriers, KernelTypes::Int32 ThreadCount = -1);

                            KernelTypes::Int32 GetMaxConcurrency() const override
                            {
                                return static_cast<KernelTypes::Int32>(Threads.Size()) + 1;
                            }

                            JobHandle CreateJob(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, KernelTypes::UInt32 Dependencies = 0) override;
                            void CreateJobs(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, JobHandle* OutJobs, KernelTypes::UInt32 JobCount, KernelTypes::UInt32 Dependencies = 0) override;

                            // Изменить число потоков после Init
                            void SetThreadCount(KernelTypes::Int32 ThreadCount)
                            {
                                StopThreads();
                                StartThreads(ThreadCount);
                            }

                        protected:
                            void QueueJob(Job* Target) override;
                            void QueueJobs(Job** Jobs, KernelTypes::UInt32 JobCount) override;
                            void FreeJob(Job* Target) override;

                        private:
                            // Задача, которая одновременно узел очереди. Очередь это многопроизводительная многопотребительская очередь
                            // Майкла и Скотта (Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms).
                            // Указатели хранятся как индекс в пуле задач плюс счётчик изменений (против проблемы ABA), упакованные в UInt64.
                            class Node : public Job
                            {
                                public:
                                    Node(const KernelTypes::Char* JobName, JobColor Color, JobSystem* OwnerSystem, const JobFunction& Function, KernelTypes::UInt32 Dependencies)
                                        : Job(JobName, Color, OwnerSystem, Function, Dependencies), Next(KernelSync::NoInitialize{})
                                    {
                                        // Next можно прочитать из освобождённого и переиспользованного узла (пул фиксированный). Поэтому значение
                                        // не конструируется обычной записью, а записывается атомарной операцией (иначе ThreadSanitizer видит гонку).
                                        Next.Store(0, KernelSync::MemoryOrder::Relaxed);
                                    }

                                    KernelSync::Atomic<KernelTypes::UInt64> Next;   // индекс следующего узла очереди и счётчик
                            };

                            static constexpr KernelTypes::UInt32 NullIndex = 0xffffffff;

                            static KernelTypes::UInt64 MakePointer(KernelTypes::UInt32 Index, KernelTypes::UInt32 Count)
                            {
                                return (static_cast<KernelTypes::UInt64>(Count) << 32) | Index;
                            }

                            static KernelTypes::UInt32 GetIndex(KernelTypes::UInt64 Pointer)
                            {
                                return static_cast<KernelTypes::UInt32>(Pointer);
                            }

                            static KernelTypes::UInt32 GetCount(KernelTypes::UInt64 Pointer)
                            {
                                return static_cast<KernelTypes::UInt32>(Pointer >> 32);
                            }

                            // Данные, которые рабочий поток получает при старте
                            struct WorkerContext
                            {
                                JobSystemThreadPool* Pool = nullptr;
                                KernelTypes::Int32 Index = 0;
                                KernelTypes::Char Name[16] = {};
                            };

                            static void WorkerEntry(void* Context);

                            // Выделить задачу
                            Job* AllocateJob(const KernelTypes::Char* Name, JobColor Color, const JobFunction& Function, KernelTypes::UInt32 Dependencies);

                            // Добавить задачу в очередь
                            void Enqueue(Job* Target);

                            // Взять задачу из очереди, nullptr если пусто. После выполнения вызывающий должен сделать Release().
                            Job* Dequeue();

                            // Запуск и остановка рабочих потоков
                            void StartThreads(KernelTypes::Int32 ThreadCount);
                            void StopThreads();

                            // Точка входа рабочего потока
                            void ThreadMain(KernelTypes::Int32 ThreadIndex);

                            // Выполнить все задачи очереди
                            void ExecuteJobs();

                            // Функции при запуске и завершении потока
                            InitExitFunction ThreadInitFunction = [](KernelTypes::Int32) {};
                            InitExitFunction ThreadExitFunction = [](KernelTypes::Int32) {};

                            // Задачи (фиксированное число)
                            using AvailableJobs = KernelContainers::FixedSizeFreeList<Node>;
                            AvailableJobs JobPool;

                            // Потоки, выполняющие задачи, и их данные (адреса данных не должны меняться, пока потоки живы)
                            KernelContainers::FastArray<KernelThreading::Thread> Threads;
                            KernelContainers::FastArray<WorkerContext> WorkerContexts;

                            // Голова и хвост очереди: индекс и счётчик (см. MakePointer)
                            alignas(KernelCompiler::CacheLineSize) KernelSync::Atomic<KernelTypes::UInt64> Head;
                            alignas(KernelCompiler::CacheLineSize) KernelSync::Atomic<KernelTypes::UInt64> Tail;

                            // Семафор, которым рабочим потокам сообщают, что есть работа
                            KernelThreading::Semaphore WorkSemaphore;

                            // Признак того, что планировщик нужно остановить
                            KernelSync::Atomic<KernelTypes::Bool> Quit{false};
                    };
                }
            }
        }
    }
}
