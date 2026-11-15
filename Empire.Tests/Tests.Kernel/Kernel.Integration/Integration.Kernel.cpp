// Многопоточные проверки аллокатора Kernel: блоки выделяются в одних потоках, освобождаются в других,
// потоки завершаются с непустыми кэшами, выделение идёт из деструкторов thread_local.
#include "Empire.Tests/Tests.Kernel/Kernel.Integration/Integration.Kernel.hpp"

#include "Empire.Kernel/Kernel.Memory/Memory.hpp"

#include <atomic>
#include <cstddef>
#include <cstdio>
#include <utility>
#include <thread>
#include <vector>

namespace KM = DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelMemory;

namespace
{
    int Failures = 0;

    void Check(bool Condition, const char* Text)
    {
        if (!Condition)
        {
            Failures++;
            std::printf("  ПРОВАЛ: %s\n", Text);
        }
    }

    struct AllocatesInDestructor
    {
        ~AllocatesInDestructor()
        {
            void* Block = KM::Allocate(48);
            KM::Deallocate(Block);
        }
    };
}

int RunKernelIntegrationTests()
{
    constexpr int ThreadCount = 8;
    constexpr int PerThread = 5000;

    const unsigned long long BaseBlocks = KM::GetLiveBlockCount();
    const unsigned long long BaseBytes = KM::GetLiveByteCount();

    // 1) выделение в одних потоках, освобождение в других
    std::vector<void*> Blocks(ThreadCount * PerThread);

    {
        std::vector<std::thread> Threads;

        for (int T = 0; T < ThreadCount; T++)
            Threads.emplace_back([&, T] { for (int I = 0; I < PerThread; I++) Blocks[T * PerThread + I] = KM::Allocate(16 + (I * 37) % 2000); });

        for (auto& Thread : Threads)
            Thread.join();
    }

    Check(KM::GetLiveBlockCount() == BaseBlocks + ThreadCount * PerThread, "живых блоков после выделения в потоках");

    {
        std::vector<std::thread> Threads;

        for (int T = 0; T < ThreadCount; T++)
            Threads.emplace_back([&, T] { for (int I = 0; I < PerThread; I++) KM::Deallocate(Blocks[((T + 3) % ThreadCount) * PerThread + I]); });

        for (auto& Thread : Threads)
            Thread.join();
    }

    Check(KM::GetLiveBlockCount() == BaseBlocks, "живых блоков после освобождения в других потоках");
    Check(KM::GetLiveByteCount() == BaseBytes, "живых байт после освобождения в других потоках");

    // 2) много коротких потоков (кэши и счётчики завершившихся потоков переиспользуются)
    for (int Round = 0; Round < 200; Round++)
    {
        std::thread([]
        {
            void* Small[100];

            for (auto& Block : Small)
                Block = KM::Allocate(64);

            for (auto Block : Small)
                KM::Deallocate(Block);
        }).join();
    }

    Check(KM::GetLiveBlockCount() == BaseBlocks, "живых блоков после коротких потоков");

    // 3) выделение из деструктора thread_local, когда кэш потока уже может быть уничтожен
    std::thread([]
    {
        static thread_local AllocatesInDestructor Probe;
        void* Block = KM::Allocate(48);
        KM::Deallocate(Block);
        (void)Probe;
    }).join();

    Check(KM::GetLiveBlockCount() == BaseBlocks, "живых блоков после деструктора thread_local");

    // 4) Trim одновременно с выделением и освобождением в других потоках: содержимое блоков не портится
    {
        std::atomic<bool> Stop{false};
        std::atomic<int> Corrupt{0};
        std::vector<std::thread> Workers;

        for (int T = 0; T < 4; T++)
        {
            Workers.emplace_back([&, T]
            {
                std::vector<std::pair<unsigned char*, std::size_t>> Mine;
                unsigned Counter = 0;

                while (!Stop.load())
                {
                    for (int I = 0; I < 2000; I++)
                    {
                        const std::size_t N = 16 + (Counter * 2654435761u + T) % 1500;
                        auto* P = static_cast<unsigned char*>(KM::Allocate(N));
                        const unsigned char Mark = static_cast<unsigned char>(Counter);
                        for (std::size_t B = 0; B < N; B++) P[B] = static_cast<unsigned char>(Mark + B);
                        Mine.emplace_back(P, N);
                        Counter++;
                    }

                    for (auto& [P, N] : Mine)
                    {
                        const unsigned char Mark = static_cast<unsigned char>(P[0]);
                        for (std::size_t B = 0; B < N; B++)
                            if (P[B] != static_cast<unsigned char>(Mark + B)) { Corrupt++; break; }

                        KM::Deallocate(P);
                    }

                    Mine.clear();
                }
            });
        }

        std::thread Trimmer([&] { for (int I = 0; I < 200; I++) { KM::Trim(); std::this_thread::yield(); } Stop = true; });
        Trimmer.join();

        for (auto& Worker : Workers)
            Worker.join();

        Check(Corrupt.load() == 0, "содержимое блоков при параллельном Trim");
    }

    Check(KM::GetLiveBlockCount() == BaseBlocks, "живых блоков в конце");
    KM::Trim();

    std::printf("Kernel, интеграционные проверки аллокатора: %s\n", Failures == 0 ? "ok" : "ПРОВАЛ");
    return Failures == 0 ? 0 : 1;
}
