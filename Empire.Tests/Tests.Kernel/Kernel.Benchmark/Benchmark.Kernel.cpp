// Сравнение аллокатора Kernel с malloc: скорость (1, 2, 4, 8 потоков) и задержка одного вызова.
// Время самого медленного потока, лучшее из нескольких запусков; задержка: медиана перцентилей по запускам.
#include "Empire.Tests/Tests.Kernel/Kernel.Benchmark/Benchmark.Kernel.hpp"

#include "Empire.Kernel/Kernel.Memory/Memory.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

#if defined(__x86_64__) || defined(__i386__)
    #include <x86intrin.h>
#endif

namespace KM = DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelMemory;

namespace
{
    struct KernelAllocator
    {
        static void* Allocate(std::size_t ByteCount) { return KM::Allocate(ByteCount); }
        static void Deallocate(void* Block) { KM::Deallocate(Block); }
    };

    struct SystemAllocator
    {
        static void* Allocate(std::size_t ByteCount) { return std::malloc(ByteCount); }
        static void Deallocate(void* Block) { std::free(Block); }
    };

    struct Random
    {
        std::uint64_t State;

        unsigned Next()
        {
            State ^= State << 13;
            State ^= State >> 7;
            State ^= State << 17;
            return static_cast<unsigned>(State >> 32);
        }
    };

    inline void Touch(void* Block)
    {
        *static_cast<volatile char*>(Block) = 1;
    }

    inline std::uint64_t Ticks()
    {
#if defined(__x86_64__) || defined(__i386__)
        return __rdtsc();
#else
        return static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
#endif
    }

    double Milliseconds(std::chrono::steady_clock::time_point Start)
    {
        return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - Start).count();
    }

    // Тест 1: кольцо из 16 блоков, размеры 16..256 (alloc и free одного и того же размера вперемешку).
    struct PingPongTest
    {
        template <typename Allocator>
        static double Run(unsigned Seed)
        {
        (void)Seed;
        void* Ring[16] = {};
        const auto Start = std::chrono::steady_clock::now();

        for (int I = 0; I < 400000; I++)
        {
            void*& Slot = Ring[I & 15];

            if (Slot != nullptr)
                Allocator::Deallocate(Slot);

            Slot = Allocator::Allocate(16 + (I % 7) * 40);
            Touch(Slot);
        }

        const double Result = Milliseconds(Start);

        for (void* Block : Ring)
            if (Block != nullptr)
                Allocator::Deallocate(Block);

        return Result;
        }
    };

    // Тест 2: 4096 живых блоков, на каждом шаге случайный блок заменяется новым, размеры 16..2000.
    struct MixedTest
    {
        template <typename Allocator>
        static double Run(unsigned Seed)
        {
        Random Generator{Seed * 2654435761ULL + 1};
        std::vector<void*> Live(4096, nullptr);
        const auto Start = std::chrono::steady_clock::now();

        for (int I = 0; I < 1000000; I++)
        {
            void*& Slot = Live[Generator.Next() % Live.size()];

            if (Slot != nullptr)
                Allocator::Deallocate(Slot);

            const unsigned X = Generator.Next();
            const std::size_t Size = (X & 7) != 0 ? 16 + (X >> 3) % 240 : 16 + (X >> 3) % 2000;

            Slot = Allocator::Allocate(Size);
            Touch(Slot);
        }

        const double Result = Milliseconds(Start);

        for (void* Block : Live)
            if (Block != nullptr)
                Allocator::Deallocate(Block);

        return Result;
        }
    };

    template <typename Test, typename Allocator>
    double BestOf(int ThreadCount, int Runs)
    {
        double Best = 0;

        for (int Run = 0; Run < Runs; Run++)
        {
            std::vector<double> Times(ThreadCount);
            std::vector<std::thread> Threads;

            for (int T = 0; T < ThreadCount; T++)
                Threads.emplace_back([&, T] { Times[T] = Test::template Run<Allocator>(static_cast<unsigned>(T + 1)); });

            for (auto& Thread : Threads)
                Thread.join();

            const double Slowest = *std::max_element(Times.begin(), Times.end());

            if (Run == 0 || Slowest < Best)
                Best = Slowest;
        }

        return Best;
    }

    const char* Verdict(double Kernel, double Reference)
    {
        if (Kernel <= Reference)
            return "Kernel быстрее или равен";

        return "Kernel МЕДЛЕННЕЕ";
    }

    template <typename Test>
    void SpeedTable(const char* Title)
    {
        std::printf("\n%s\n", Title);
        std::printf("  Потоков | Kernel, мс | malloc, мс | Kernel/malloc | итог\n");

        for (int Threads : {1, 2, 4, 8})
        {
            const double Kernel = BestOf<Test, KernelAllocator>(Threads, 5);
            const double System = BestOf<Test, SystemAllocator>(Threads, 5);

            std::printf("  %7d | %10.1f | %10.1f | %13.2f | %s\n", Threads, Kernel, System, Kernel / System, Verdict(Kernel, System));
        }
    }

    // Задержка отдельных вызовов (в тактах), 1 поток, 4096 живых блоков размером 16..515.
    struct LatencyResult
    {
        std::uint64_t P50, P99, P999, P9999;
    };

    template <typename Allocator>
    LatencyResult MeasureLatency()
    {
        constexpr int Operations = 2000000;
        std::vector<std::uint32_t> Samples;
        Samples.reserve(Operations * 2);

        Random Generator{12345};
        std::vector<void*> Live(4096, nullptr);

        for (int I = 0; I < Operations; I++)
        {
            void*& Slot = Live[Generator.Next() % Live.size()];

            const std::uint64_t A = Ticks();

            if (Slot != nullptr)
                Allocator::Deallocate(Slot);

            const std::uint64_t B = Ticks();
            Slot = Allocator::Allocate(16 + Generator.Next() % 500);
            const std::uint64_t C = Ticks();

            Samples.push_back(static_cast<std::uint32_t>(C - B));
            Samples.push_back(static_cast<std::uint32_t>(B - A));
        }

        for (void* Block : Live)
            if (Block != nullptr)
                Allocator::Deallocate(Block);

        std::sort(Samples.begin(), Samples.end());

        auto Percentile = [&](double Q) { return static_cast<std::uint64_t>(Samples[static_cast<std::size_t>(Q * static_cast<double>(Samples.size() - 1))]); };
        return {Percentile(0.5), Percentile(0.99), Percentile(0.999), Percentile(0.9999)};
    }

    std::uint64_t Median(std::vector<std::uint64_t> Values)
    {
        std::sort(Values.begin(), Values.end());
        return Values[Values.size() / 2];
    }

    template <typename Allocator>
    LatencyResult MedianLatency(int Runs)
    {
        std::vector<std::uint64_t> P50, P99, P999, P9999;

        for (int Run = 0; Run < Runs; Run++)
        {
            const LatencyResult R = MeasureLatency<Allocator>();
            P50.push_back(R.P50);
            P99.push_back(R.P99);
            P999.push_back(R.P999);
            P9999.push_back(R.P9999);
        }

        return {Median(P50), Median(P99), Median(P999), Median(P9999)};
    }

    void LatencyRow(const char* Name, std::uint64_t Kernel, std::uint64_t System)
    {
        std::printf("  %-14s | %8llu | %8llu | %s\n", Name, static_cast<unsigned long long>(Kernel), static_cast<unsigned long long>(System),
            Kernel <= System ? "Kernel лучше или равен" : "Kernel ХУЖЕ");
    }
}

int RunKernelBenchmarks()
{
    std::printf("Kernel против malloc (glibc или системный). Меньше = лучше.\n");

    SpeedTable<PingPongTest>("Тест 1: alloc+free по кругу (16 блоков, размеры 16..256), 400k операций на поток");
    SpeedTable<MixedTest>("Тест 2: смешанные размеры 16..2000, 4096 живых блоков, 1M операций на поток");

    const LatencyResult Kernel = MedianLatency<KernelAllocator>(5);
    const LatencyResult System = MedianLatency<SystemAllocator>(5);

    std::printf("\nТест 3: задержка одного вызова, такты rdtsc (медиана по 5 запускам), 1 поток\n");
    std::printf("  Перцентиль     |   Kernel |   malloc | итог\n");
    LatencyRow("p50 (обычный)", Kernel.P50, System.P50);
    LatencyRow("p99", Kernel.P99, System.P99);
    LatencyRow("p99.9", Kernel.P999, System.P999);
    LatencyRow("p99.99", Kernel.P9999, System.P9999);

    return 0;
}
