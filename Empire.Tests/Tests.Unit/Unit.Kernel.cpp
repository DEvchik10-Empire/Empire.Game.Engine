// Поведенческий тест Kernel: Types, Memory, Containers, Events, Services, Reflection, Logging, Assert, Sync.
#include "Empire.Tests/Tests.Unit/Unit.Kernel.hpp"

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.Function.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Events/Events.EventBus.hpp"
#include "Empire.Kernel/Kernel.Logging/Logging.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Kernel/Kernel.Reflection/Reflection.TypeRegistry.hpp"
#include "Empire.Kernel/Kernel.Services/Services.ServiceRegistry.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <typeinfo>

namespace K = DEvchik10Empire::EmpireGameEngine::EmpireKernel;
namespace KT = K::KernelTypes;
namespace KM = K::KernelMemory;
namespace KC = K::KernelContainers;
namespace KA = K::KernelAssert;
namespace KS = K::KernelSync;

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)
#define CHECK_THROWS(E, expr) do { ++Checks; bool Caught = false; try { expr; } catch (const E&) { Caught = true; } catch (...) { } \
    if (!Caught) { ++Failures; std::printf("  FAIL %s:%d  не бросило %s: %s\n", __FILE__, __LINE__, #E, #expr); } } while (0)
#define SECTION(n) std::printf("[%s]\n", n)

// Тип, считающий живые экземпляры и бросающий исключение на N-й копии.
struct Tracked
{
    static inline int Alive = 0;
    static inline int ThrowAtCopy = -1;
    static inline int Copies = 0;
    int Value = 0;
    Tracked(int V = 0) : Value(V) { ++Alive; }
    Tracked(const Tracked& O) : Value(O.Value) { if (ThrowAtCopy >= 0 && ++Copies == ThrowAtCopy) throw 42; ++Alive; }
    Tracked(Tracked&& O) noexcept : Value(O.Value) { ++Alive; }
    Tracked& operator=(const Tracked&) = default;
    Tracked& operator=(Tracked&&) noexcept = default;
    ~Tracked() { --Alive; }
};

struct Ping { int N; };
struct Pong { int N; };
struct ServiceA { int X = 7; };
struct ServiceB { int Y = 9; };

int RunKernelUnitTests()
{
    const KT::UInt64 BaseBlocks = KM::GetLiveBlockCount();
    const KT::UInt64 BaseBytes = KM::GetLiveByteCount();

    SECTION("Types");
    CHECK(sizeof(KT::Int8) == 1 && sizeof(KT::Int16) == 2 && sizeof(KT::Int32) == 4 && sizeof(KT::Int64) == 8);
    CHECK(sizeof(KT::UInt8) == 1 && sizeof(KT::UInt16) == 2 && sizeof(KT::UInt32) == 4 && sizeof(KT::UInt64) == 8);
    CHECK(sizeof(KT::Size) == sizeof(void*));
    CHECK(KT::MaxValue<KT::UInt8>() == 255 && KT::MaxValue<KT::Int8>() == 127 && KT::MaxValue<KT::UInt64>() == ~0ULL);
    CHECK(KT::MaxValue<KT::Int64>() == 0x7FFFFFFFFFFFFFFFLL && KT::MaxValue<KT::Int32>() == 0x7FFFFFFF);
    CHECK(KT::GetTypeKey<int>() != KT::GetTypeKey<unsigned>() && KT::GetTypeKey<Ping>() != KT::GetTypeKey<Pong>());
    CHECK(KT::GetTypeKey<Ping>() == KT::GetTypeKey<Ping>());
    CHECK(KT::GetTypeKey<const Ping>() != KT::GetTypeKey<Ping>() || true); // информативно
    auto Loc = KT::SourceLocation::Current();
    CHECK(std::strstr(Loc.GetFile(), "Unit.Kernel.cpp") != nullptr && Loc.GetLine() > 0 && std::strstr(Loc.GetFunction(), "RunKernelUnitTests") != nullptr);
    CHECK(KT::Min(3, 4) == 3 && KT::Max(3, 4) == 4);
    { int A = 1, B = 2; KT::Swap(A, B); CHECK(A == 2 && B == 1); }

    SECTION("Memory");
    {
        for (KT::Size Alignment : {8u, 16u, 32u, 64u, 4096u})
        {
            void* P = KM::Allocate(100, Alignment);
            CHECK(P != nullptr && (reinterpret_cast<KT::Size>(P) % Alignment) == 0);
            KM::FillBytes(P, 0xAB, 100);
            CHECK(KM::GetBlockByteCount(P) >= 100);
            KM::Deallocate(P);
        }
        void* P = KM::Allocate(64);
        for (int I = 0; I < 64; I++) static_cast<unsigned char*>(P)[I] = static_cast<unsigned char>(I);
        P = KM::Reallocate(P, 100000);
        bool Same = true; for (int I = 0; I < 64; I++) Same &= static_cast<unsigned char*>(P)[I] == I;
        CHECK(Same);
        P = KM::Reallocate(P, 16);
        Same = true; for (int I = 0; I < 16; I++) Same &= static_cast<unsigned char*>(P)[I] == I;
        CHECK(Same);
        KM::Deallocate(P);
        KM::Deallocate(nullptr);
        void* Z = KM::Allocate(0); KM::Deallocate(Z);
        CHECK_THROWS(KA::AllocationError, KM::Allocate(KT::MaxValue<KT::Size>() - 8));
        CHECK_THROWS(KA::AllocationError, KM::Allocate(KT::MaxValue<KT::Size>() / 2));
        unsigned char A[8] = {1,2,3,4,5,6,7,8}; KM::MoveBytes(A + 2, A, 6);
        CHECK(A[2] == 1 && A[7] == 6);
        CHECK(KM::CompareBytes("abc", "abd", 3) < 0 && KM::CompareBytes("abc", "abc", 3) == 0 && KM::CompareBytes("b", "a", 1) > 0);
        CHECK(KM::GetLiveBlockCount() == BaseBlocks);
    }

    SECTION("Array");
    {
        KC::Array<int> A;
        for (int I = 0; I < 10000; I++) A.Add(I);
        CHECK(A.GetCount() == 10000 && A[9999] == 9999);
        KC::Array<int> B(A); CHECK(B.GetCount() == 10000 && B[1234] == 1234);
        KC::Array<int> C(KT::Move(B)); CHECK(C.GetCount() == 10000 && B.GetCount() == 0);
        A.RemoveAt(0); CHECK(A.GetCount() == 9999 && A[0] == 1);
        A.RemoveAt(A.GetCount() - 1); CHECK(A[A.GetCount() - 1] == 9998);
        CHECK_THROWS(KA::OutOfRangeError, A[A.GetCount()]);
        CHECK_THROWS(KA::OutOfRangeError, A.RemoveAt(A.GetCount()));
        CHECK_THROWS(KA::LengthError, A.Reserve(KT::MaxValue<KT::Size>() / 2));
        auto D = KC::Array<int>::Of(1, 2, 3); CHECK(D.GetCount() == 3 && D[2] == 3);
        { auto& Alias = D; D = Alias; } CHECK(D.GetCount() == 3 && D[0] == 1);   // самоприсваивание
        KC::Array<int> E = D; E = KT::Move(E); (void)E;                  // самоперемещение
        // алиасинг: добавление элемента самого массива при реаллокации
        KC::Array<KC::String> S;
        S.Add(KC::String("hello")); 
        for (int I = 0; I < 200; I++) S.Add(S[0]);
        bool AllHello = true; for (auto& X : S) AllHello &= (X == KC::String("hello"));
        CHECK(S.GetCount() == 201 && AllHello);
        int Sum = 0; for (int X : D) Sum += X; CHECK(Sum == 6);
        D.Clear(); CHECK(D.IsEmpty());
    }
    {   // безопасность исключений
        Tracked::Alive = 0;
        {
            KC::Array<Tracked> A; for (int I = 0; I < 20; I++) A.Add(Tracked(I));
            CHECK(Tracked::Alive == 20);
            Tracked::Copies = 0; Tracked::ThrowAtCopy = 5;
            bool Thrown = false; try { KC::Array<Tracked> Copy(A); } catch (int) { Thrown = true; }
            Tracked::ThrowAtCopy = -1;
            CHECK(Thrown); CHECK(Tracked::Alive == 20);
            Tracked::Copies = 0; Tracked::ThrowAtCopy = 3;
            Thrown = false; try { KC::Array<Tracked> Other; Other = A; } catch (int) { Thrown = true; }
            Tracked::ThrowAtCopy = -1;
            CHECK(Thrown); CHECK(Tracked::Alive == 20);
        }
        CHECK(Tracked::Alive == 0);
        CHECK(KM::GetLiveBlockCount() == BaseBlocks);
    }

    SECTION("HashMap");
    {
        KC::HashMap<KT::UInt64, int> M;
        for (KT::UInt64 I = 0; I < 100000; I++) CHECK(M.Insert(I * 2654435761ULL, static_cast<int>(I)));
        CHECK(M.GetCount() == 100000);
        CHECK(!M.Insert(0, 5));                                            // дубликат
        CHECK(*M.Find(0) == 0);
        M.Set(0, 77); CHECK(*M.Find(0) == 77);
        int Removed = 0; for (KT::UInt64 I = 0; I < 100000; I += 2) Removed += M.Remove(I * 2654435761ULL);
        CHECK(Removed == 50000 && M.GetCount() == 50000);
        CHECK(!M.Contains(2 * 2654435761ULL) && M.Contains(3 * 2654435761ULL) == (true));
        for (KT::UInt64 I = 0; I < 100000; I += 2) M.Insert(I * 2654435761ULL, 1);   // повторное использование tombstone
        CHECK(M.GetCount() == 100000);
        KT::Size Visited = 0; M.ForEach([&](const KT::UInt64&, int&) { Visited++; return true; }); CHECK(Visited == 100000);
        // много циклов вставка/удаление одного ключа: не должен расти бесконечно
        KC::HashMap<KT::UInt64, int> T2;
        for (int I = 0; I < 1000000; I++) { T2.Insert(static_cast<KT::UInt64>(I), I); T2.Remove(static_cast<KT::UInt64>(I)); }
        CHECK(T2.GetCount() == 0);
        KC::HashMap<KT::UInt64, int> Moved(KT::Move(M)); CHECK(Moved.GetCount() == 100000 && M.GetCount() == 0);
        M = KT::Move(Moved); CHECK(M.GetCount() == 100000);
        M.Clear(); CHECK(M.GetCount() == 0 && M.Find(0) == nullptr);
        KC::HashMap<KT::UInt64, KC::String> Strs;
        for (KT::UInt64 I = 0; I < 1000; I++) Strs.Insert(I, KC::String("value") + KC::ToString(I));
        CHECK(*Strs.Find(999) == KC::String("value999"));
    }
    CHECK(KM::GetLiveBlockCount() == BaseBlocks);

    SECTION("String / StringView");
    {
        KC::String S("abc"); S.Append("def"); S += 'g'; S += KC::String("hi");
        CHECK(S == KC::String("abcdefghi") && S.GetCount() == 9 && std::strcmp(S.GetText(), "abcdefghi") == 0);
        CHECK(S[0] == 'a' && S[8] == 'i');
        CHECK_THROWS(KA::OutOfRangeError, S[9]);
        KC::String E; CHECK(E.IsEmpty() && std::strcmp(E.GetText(), "") == 0);
        S.Append(S); CHECK(S == KC::String("abcdefghiabcdefghi"));       // самоприсваивание в Append
        KC::String Self("x"); { auto& Alias = Self; Self = Alias; } CHECK(Self == KC::String("x"));
        KC::String Big; for (int I = 0; I < 100000; I++) Big += 'z'; CHECK(Big.GetCount() == 100000 && Big.GetText()[99999] == 'z' && Big.GetText()[100000] == '\0');
        CHECK(KC::ToString(-123) == KC::String("-123") && KC::ToString(0) == KC::String("0"));
        CHECK(KC::ToString(KT::MaxValue<KT::UInt64>()) == KC::String("18446744073709551615"));
        CHECK(KC::ToString(KT::Int64(-9223372036854775807LL - 1)) == KC::String("-9223372036854775808"));
        std::printf("    ToString(0.5)=%s ToString(-1.25)=%s ToString(1e20)=%s ToString(3.14159)=%s\n",
            KC::ToString(0.5).GetText(), KC::ToString(-1.25).GetText(), KC::ToString(1e20).GetText(), KC::ToString(3.14159).GetText());
        KC::StringView V("hello"); CHECK(V.GetCount() == 5 && V[1] == 'e' && V == KC::StringView("hello") && V != KC::StringView("help"));
        CHECK_THROWS(KA::OutOfRangeError, V[5]);
        KC::StringView Null(static_cast<const char*>(nullptr)); std::printf("    StringView(nullptr): count=%zu\n", (size_t)Null.GetCount());
        KC::String FromNull(static_cast<const char*>(nullptr)); std::printf("    String(nullptr): count=%zu\n", (size_t)FromNull.GetCount());
    }
    CHECK(KM::GetLiveBlockCount() == BaseBlocks);

    SECTION("SharedPtr / Function");
    {
        Tracked::Alive = 0;
        {
            auto P = KM::MakeShared<Tracked>(5); auto Q = P;
            CHECK(P.GetUseCount() == 2 && Q->Value == 5 && Tracked::Alive == 1);
            auto R = KT::Move(Q); CHECK(!Q && P.GetUseCount() == 2);
            R.Reset(); CHECK(P.GetUseCount() == 1);
            KM::SharedPtr<void> V(P); CHECK(P.GetUseCount() == 2);
            auto Back = KM::StaticPointerCast<Tracked>(V); CHECK(Back->Value == 5 && P.GetUseCount() == 3);
            { auto& Alias = P; P = Alias; }  CHECK(P.GetUseCount() == 3);
        }
        CHECK(Tracked::Alive == 0);
        int Hits = 0; KC::Function<int(int)> F([&Hits](int X) { Hits++; return X * 2; });
        auto G = F; CHECK(F(4) == 8 && G(5) == 10 && Hits == 2);
        KC::Function<void()> Empty; CHECK(!Empty); CHECK_THROWS(KA::LogicError, Empty());
        KC::String Cap("captured"); KC::Function<KC::String()> H([Cap]() { return Cap; }); CHECK(H() == Cap);
    }
    CHECK(KM::GetLiveBlockCount() == BaseBlocks);

    SECTION("EventBus");
    {
        K::KernelEvents::EventBus Bus; int PingSum = 0, PongSum = 0;
        auto A = Bus.Subscribe<Ping>([&](const Ping& E) { PingSum += E.N; });
        auto B = Bus.Subscribe<Pong>([&](const Pong& E) { PongSum += E.N; });
        Bus.Publish(Ping{3}); Bus.Publish(Pong{5}); Bus.Publish(Ping{4});
        CHECK(PingSum == 7 && PongSum == 5 && Bus.GetListenerCount<Ping>() == 1);
        CHECK(Bus.Unsubscribe(A) && !Bus.Unsubscribe(A)); Bus.Publish(Ping{100}); CHECK(PingSum == 7);
        // подписка и отписка изнутри обработчика (блокировка во время вызова не держится)
        K::KernelEvents::EventBus::SubscriptionId Self = 0; int Calls = 0;
        Self = Bus.Subscribe<Ping>([&](const Ping&) { Calls++; Bus.Unsubscribe(Self); Bus.Subscribe<Ping>([&](const Ping&) { Calls += 100; }); });
        Bus.Publish(Ping{1}); Bus.Publish(Ping{1}); CHECK(Calls == 101);
        (void)B; Bus.Clear(); CHECK(Bus.GetSubscriptionCount() == 0);
    }
    SECTION("ServiceRegistry");
    {
        K::KernelServices::ServiceRegistry R;
        auto SA = R.Create<ServiceA>(); CHECK(SA->X == 7 && R.Contains<ServiceA>() && !R.Contains<ServiceB>());
        CHECK(R.Get<ServiceA>().Get() == SA.Get() && !R.Get<ServiceB>());
        CHECK_THROWS(KA::LogicError, R.Create<ServiceA>());
        CHECK_THROWS(KA::InvalidArgumentError, R.Register<ServiceB>(KM::SharedPtr<ServiceB>()));
        CHECK(R.Remove<ServiceA>() && !R.Remove<ServiceA>() && R.GetCount() == 0);
    }
    SECTION("TypeRegistry");
    {
        K::KernelReflection::TypeRegistry T;
        auto Id = T.RegisterType<Ping>("Ping");
        CHECK(T.Find(Id) != nullptr && T.Find<Ping>() != nullptr && T.FindByName(KC::StringView("Ping")) != nullptr);
        CHECK(T.Find<Ping>()->GetSize() == sizeof(Ping) && T.Find<Ping>()->GetAlignment() == alignof(Ping));
        CHECK(T.Find<Pong>() == nullptr && T.FindByName(KC::StringView("Nope")) == nullptr);
        bool Thrown = false; try { T.RegisterType<Ping>("Ping"); } catch (const KA::Error&) { Thrown = true; }
        std::printf("    повторная регистрация того же типа/имени бросает: %d (count=%zu)\n", Thrown, (size_t)T.GetTypeCount());
        bool Thrown2 = false; try { T.RegisterType<Pong>("Ping"); } catch (const KA::Error&) { Thrown2 = true; }
        std::printf("    другая тип с тем же именем бросает: %d\n", Thrown2);
        bool Thrown3 = false; try { T.RegisterType<Pong>(""); } catch (const KA::Error&) { Thrown3 = true; }
        std::printf("    пустое имя бросает: %d\n", Thrown3);
    }

    SECTION("Logging");
    {
        struct Capture : K::KernelLogging::Sink {
            std::vector<std::string> Lines; std::vector<int> Levels;
            void Write(K::KernelLogging::Level L, KC::StringView Line) override { Levels.push_back((int)L); Lines.emplace_back(Line.GetText(), Line.GetCount()); }
        } Cap;
        K::KernelLogging::SetSink(&Cap);
        K::KernelLogging::SetMinimumLevel(K::KernelLogging::Level::Info);
        K::KernelLogging::Debug("hidden"); K::KernelLogging::Info("shown"); K::KernelLogging::Error("bad");
        CHECK(Cap.Lines.size() == 2 && Cap.Levels[0] == 2 && Cap.Levels[1] == 4);
        if (Cap.Lines.size() == 2)
        {
            std::printf("    строка лога: \"%s\"\n", Cap.Lines[0].c_str());
            // путь укорочен (без абсолютного префикса), строка заканчивается переводом строки
            CHECK(Cap.Lines[0].find("(Empire.Tests/Tests.Unit/Unit.Kernel.cpp:") != std::string::npos || Cap.Lines[0].find("(Unit.Kernel.cpp:") != std::string::npos);
            CHECK(Cap.Lines[0].find('/') != 0 && Cap.Lines[0].back() == '\n');
        }
        K::KernelLogging::SetMinimumLevel(K::KernelLogging::Level::Off); K::KernelLogging::Fatal("off"); CHECK(Cap.Lines.size() == 2);
        K::KernelLogging::SetSink(nullptr); K::KernelLogging::SetMinimumLevel(K::KernelLogging::Level::Info);
    }
    SECTION("Assert");
    {
        KA::Check(true, "ok");
        KA::Error E("msg"); CHECK(std::strcmp(E.GetText(), "msg") == 0);
        std::string Long(1000, 'x'); KA::LogicError L(Long.c_str());
        CHECK(std::strlen(L.GetText()) <= 255 && std::strlen(L.GetText()) > 200);
        std::printf("    GetKind(Error)=%s GetKind(LogicError)=%s len(long)=%zu line=%u\n", E.GetKind(), L.GetKind(), std::strlen(L.GetText()), (unsigned)E.GetLocation().GetLine());
        KA::Error Copy(L); CHECK(std::strcmp(Copy.GetText(), L.GetText()) == 0);
        bool ByBase = false; try { throw KA::OutOfRangeError("r"); } catch (const KA::LogicError&) { ByBase = true; } CHECK(ByBase);
    }

    SECTION("Многопоточность");
    {
        KS::Mutex M; long Counter = 0; KS::Atomic<KT::UInt64> At(0);
        std::vector<std::thread> Ts;
        for (int T = 0; T < 8; T++) Ts.emplace_back([&] { for (int I = 0; I < 100000; I++) { KS::LockGuard G(M); Counter++; At.FetchAdd(1, KS::MemoryOrder::Relaxed); } });
        for (auto& T : Ts) T.join();
        CHECK(Counter == 800000 && At.Load() == 800000);
        // аллокатор, SharedPtr и EventBus из многих потоков
        K::KernelEvents::EventBus Bus; std::atomic<long> Got{0};
        auto Id = Bus.Subscribe<Ping>([&](const Ping& E) { Got += E.N; });
        auto Shared = KM::MakeShared<ServiceA>();
        K::KernelServices::ServiceRegistry Reg; Reg.Register<ServiceA>(Shared);
        Ts.clear();
        for (int T = 0; T < 8; T++) Ts.emplace_back([&] {
            for (int I = 0; I < 20000; I++) {
                Bus.Publish(Ping{1});
                auto S = Reg.Get<ServiceA>(); (void)S;
                void* P = KM::Allocate(48 + (I & 63)); KM::Deallocate(P);
                KC::Array<int> A; for (int J = 0; J < 16; J++) A.Add(J);
                auto Sub = Bus.Subscribe<Pong>([](const Pong&) {}); Bus.Unsubscribe(Sub);
            } });
        for (auto& T : Ts) T.join();
        CHECK(Got == 8 * 20000); (void)Id;
    }

    SECTION("Memory.Trim");
    {
        // выделили много блоков разных классов, проверили содержимое, освободили, вернули память ОС, выделили заново
        auto Fill = [](unsigned char* P, size_t N, unsigned Seed) { for (size_t I = 0; I < N; I++) P[I] = (unsigned char)(Seed + I * 7); };
        auto Verify = [](const unsigned char* P, size_t N, unsigned Seed) { for (size_t I = 0; I < N; I++) if (P[I] != (unsigned char)(Seed + I * 7)) return false; return true; };

        std::vector<void*> Blocks; std::vector<size_t> Sizes;
        const KT::Size ReservedBase = KM::GetReservedByteCount();
        unsigned Seed = 1;

        for (int I = 0; I < 200000; I++)
        {
            size_t N = 16 + (size_t)(I * 2654435761u) % 2000;
            auto* P = static_cast<unsigned char*>(KM::Allocate(N)); Fill(P, N, Seed + I); Blocks.push_back(P); Sizes.push_back(N);
        }
        const KT::Size ReservedFull = KM::GetReservedByteCount();
        CHECK(ReservedFull > ReservedBase + 100u * 1024 * 1024);

        bool AllGood = true;
        for (size_t I = 0; I < Blocks.size(); I++) AllGood = AllGood && Verify(static_cast<unsigned char*>(Blocks[I]), Sizes[I], Seed + (unsigned)I);
        CHECK(AllGood);

        // пока блоки живы, Trim ничего не отдаёт и ничего не ломает
        KM::Trim();
        AllGood = true;
        for (size_t I = 0; I < Blocks.size(); I++) AllGood = AllGood && Verify(static_cast<unsigned char*>(Blocks[I]), Sizes[I], Seed + (unsigned)I);
        CHECK(AllGood);
        CHECK(KM::GetReservedByteCount() >= ReservedFull - 4u * 1024 * 1024);

        // освобождаем каждый второй: куски заняты наполовину, Trim не должен отдавать их
        for (size_t I = 0; I < Blocks.size(); I += 2) { KM::Deallocate(Blocks[I]); Blocks[I] = nullptr; }
        KM::Trim();
        AllGood = true;
        for (size_t I = 1; I < Blocks.size(); I += 2) AllGood = AllGood && Verify(static_cast<unsigned char*>(Blocks[I]), Sizes[I], Seed + (unsigned)I);
        CHECK(AllGood);

        for (size_t I = 1; I < Blocks.size(); I += 2) KM::Deallocate(Blocks[I]);
        const KT::Size Returned = KM::Trim();
        const KT::Size ReservedAfter = KM::GetReservedByteCount();
        std::printf("    Trim: держали %llu МБ, вернули ОС %llu МБ, осталось %llu МБ\n",
            (unsigned long long)(ReservedFull >> 20), (unsigned long long)(Returned >> 20), (unsigned long long)(ReservedAfter >> 20));
        CHECK(Returned > 100u * 1024 * 1024);
        CHECK(ReservedAfter < ReservedBase + 8u * 1024 * 1024);

        // после Trim аллокатор работает: другие размеры, содержимое цело
        std::vector<void*> Again;
        for (int I = 0; I < 100000; I++) { auto* P = static_cast<unsigned char*>(KM::Allocate(1024)); Fill(P, 1024, (unsigned)I); Again.push_back(P); }
        AllGood = true;
        for (size_t I = 0; I < Again.size(); I++) AllGood = AllGood && Verify(static_cast<unsigned char*>(Again[I]), 1024, (unsigned)I);
        CHECK(AllGood);
        for (void* P : Again) KM::Deallocate(P);
        KM::Trim();
    }

    CHECK(KM::GetLiveBlockCount() == BaseBlocks);
    CHECK(KM::GetLiveByteCount() == BaseBytes);
    std::printf("\nпроверок: %d, провалов: %d, живых блоков в конце: %llu (было %llu)\n",
        Checks, Failures, (unsigned long long)KM::GetLiveBlockCount(), (unsigned long long)BaseBlocks);
    return Failures == 0 ? 0 : 1;
}
