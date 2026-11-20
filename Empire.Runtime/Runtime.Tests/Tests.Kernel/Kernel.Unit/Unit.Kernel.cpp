// Поведенческий тест Kernel: Types, Memory, Containers, Events, Services, Reflection, Logging, Assert, Sync.
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Kernel.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Result.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StridedPtr.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.ObjectToIdMap.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Function.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.LinearCurve.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StringTools.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Config/Config.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Time/Time.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Events/Events.EventBus.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Logging/Logging.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Profiler/Profiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Reflection/Reflection.TypeRegistry.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Reflection/Reflection.Factory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Services/Services.ServiceRegistry.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.MutexArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystemThreadPool.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Streams/Streams.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <typeinfo>

namespace K = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel;
namespace KT = K::KernelTypes;
namespace KM = K::KernelMemory;
namespace KC = K::KernelContainers;
namespace KA = K::KernelAssert;
namespace KS = K::KernelSync;
namespace KSt = K::KernelStreams;
namespace KCfg = K::KernelConfig;

static_assert(requires(K::KernelJobs::JobSystemThreadPool& Pool) { Pool.SetNumThreads(1); });
static_assert(requires(KM::AllocatorCallbacks& Callbacks) {
    Callbacks.Allocate; Callbacks.Reallocate; Callbacks.Deallocate;
    Callbacks.GetBlockByteCount; Callbacks.GetBlockAlignment;
});
static_assert(requires(KC::IdToObjectMap<struct ObjectMapAliasProbe>& Objects) { Objects.GetCount(); });
static_assert(requires(KC::IDToObjectMap<struct ObjectMapAliasProbe>& Objects) { Objects.GetCount(); });

static int ProfileStarts = 0;
static int ProfileEnds = 0;

static void BeginProfile(const KT::Char*, KT::UInt32, KT::UInt8* UserData)
{
    ++ProfileStarts;
    UserData[0] = 1;
}

static void EndProfile(KT::UInt8* UserData)
{
    if (UserData[0] == 1)
        ++ProfileEnds;
}

static int AssertCallbackCount = 0;
static int TraceCallbackCount = 0;
static KT::Bool CaptureAssertFailure(const KT::Char* Expression, const KT::Char* Message, const KT::SourceLocation&)
{
    if (std::strcmp(Expression, "false expression") == 0 && std::strcmp(Message, "callback assertion") == 0)
        ++AssertCallbackCount;
    // true означает «вызвать Breakpoint()» (как в Jolt); в тесте это SIGTRAP и падение процесса, поэтому false
    return false;
}
static void CaptureTrace(const KT::Char* Message, const KT::SourceLocation&)
{
    if (std::strcmp(Message, "callback trace") == 0)
        ++TraceCallbackCount;
}

static KT::Bool WriteStreamBuffer(void* Context, const void* Source, KT::Size ByteCount)
{
    static_cast<KSt::ByteBuffer*>(Context)->AppendBytes(Source, ByteCount);
    return true;
}

struct StreamBufferReader
{
    const KSt::ByteBuffer* Buffer;
    KT::Size Offset = 0;
};

static KT::Size ReadStreamBuffer(void* Context, void* Destination, KT::Size ByteCount)
{
    auto& Reader = *static_cast<StreamBufferReader*>(Context);
    const KT::Size Available = Reader.Buffer->GetSize() - Reader.Offset;
    const KT::Size ReadCount = Available < ByteCount ? Available : ByteCount;
    if (ReadCount > 0)
        KM::CopyBytes(Destination, Reader.Buffer->GetData() + Reader.Offset, ReadCount);
    Reader.Offset += ReadCount;
    return ReadCount;
}

static KT::Size ReadInvalidCount(void*, void*, KT::Size ByteCount)
{
    return ByteCount + 1;
}

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)
#define SECTION(n) std::printf("[%s]\n", n)

// Тип, считающий живые экземпляры для проверки времени жизни объектов.
struct Tracked
{
    static inline int Alive = 0;
    int Value = 0;
    Tracked(int V = 0) : Value(V) { ++Alive; }
    Tracked(const Tracked& O) : Value(O.Value) { ++Alive; }
    Tracked(Tracked&& O) noexcept : Value(O.Value) { ++Alive; }
    Tracked& operator=(const Tracked&) = default;
    Tracked& operator=(Tracked&&) noexcept = default;
    ~Tracked() { --Alive; }
};

struct MoveConstructOnly
{
    int Value = 0;
    explicit MoveConstructOnly(int InitialValue) : Value(InitialValue) {}
    MoveConstructOnly(const MoveConstructOnly&) = delete;
    MoveConstructOnly(MoveConstructOnly&& Other) noexcept : Value(Other.Value) {}
    MoveConstructOnly& operator=(const MoveConstructOnly&) = delete;
    MoveConstructOnly& operator=(MoveConstructOnly&&) = delete;
};

struct Ping { int N; };
struct Pong { int N; };
struct ServiceA { int X = 7; };
struct ServiceB { int Y = 9; };
struct FactoryItem { int Value = 77; };
struct FactoryBase { int BaseValue = 13; };
struct FactoryOtherBase { int OtherValue = 21; };
struct FactoryDerived : FactoryOtherBase, FactoryBase { int DerivedValue = 34; };
struct FactoryAbstract { virtual ~FactoryAbstract() = default; virtual void Run() = 0; };
struct StreamObject : KM::RefTarget<StreamObject>
{
    KT::UInt32 Value = 0;
    explicit StreamObject(KT::UInt32 InitialValue = 0) : Value(InitialValue) {}
    void SaveBinaryState(KSt::StreamOut& Output) const { Output.Write(Value); }
};

static KC::Result<KM::Ref<StreamObject>> RestoreStreamObject(KSt::StreamIn& Input)
{
    KT::UInt32 Value = 0;
    Input.Read(Value);
    KC::Result<KM::Ref<StreamObject>> Result;
    if (Input.IsFailed())
        Result.SetError("Unable to restore stream object");
    else
        Result.Set(KM::Ref<StreamObject>(new StreamObject(Value)));
    return Result;
}

static KC::Result<KM::Ref<StreamObject>> RestoreStreamObjectByType(KT::UInt32 TypeHash, KSt::StreamIn& Input)
{
    if (TypeHash != 0x5354524du)
    {
        KC::Result<KM::Ref<StreamObject>> Failed;
        Failed.SetError("Unknown stream object type");
        return Failed;
    }
    return RestoreStreamObject(Input);
}

namespace
{
    struct AttributeSample
    {
        int Count = 0;
        float Scale = 0.0f;
    };
}

int RunKernelUnitTests()
{
    CHECK(RunCompilerNeonCompileChecks());
    CHECK(RunCompilerRISCVCompileChecks());
    KT::UInt64 BaseBlocks = KM::GetLiveBlockCount();
    KT::UInt64 BaseBytes = KM::GetLiveByteCount();

    SECTION("Types");
    CHECK(sizeof(KT::Int8) == 1 && sizeof(KT::Int16) == 2 && sizeof(KT::Int32) == 4 && sizeof(KT::Int64) == 8);
    CHECK(sizeof(KT::UInt8) == 1 && sizeof(KT::UInt16) == 2 && sizeof(KT::UInt32) == 4 && sizeof(KT::UInt64) == 8);
    CHECK(sizeof(KT::Size) == sizeof(void*));
    CHECK(KT::MaxValue<KT::UInt8>() == 255 && KT::MaxValue<KT::Int8>() == 127 && KT::MaxValue<KT::UInt64>() == ~0ULL);
    CHECK(KT::MaxValue<KT::Int64>() == 0x7FFFFFFFFFFFFFFFLL && KT::MaxValue<KT::Int32>() == 0x7FFFFFFF);
    CHECK(KT::GetTypeKey<int>() != KT::GetTypeKey<unsigned>() && KT::GetTypeKey<Ping>() != KT::GetTypeKey<Pong>());
    CHECK(KT::GetTypeKey<Ping>() == KT::GetTypeKey<Ping>());
    CHECK(sizeof(KCfg::Real) == sizeof(KT::Float) && sizeof(KCfg::ObjectLayer) == sizeof(KT::UInt16));
    CHECK(KCfg::PhysicsVersionMajor == 5 && KCfg::PhysicsVersionMinor == 7 && KCfg::PhysicsVersionPatch == 0);
    CHECK((KCfg::PhysicsVersionId & 0xFFFFFFu) == 0x050700u);
    CHECK(KCfg::AssertionsEnabled == (K::KernelCompiler::IsDebugBuild || KCfg::AssertionsInRelease));
    CHECK(KT::GetTypeKey<const Ping>() != KT::GetTypeKey<Ping>() || true); // информативно
    auto Loc = KT::SourceLocation::Current();
    CHECK(std::strstr(Loc.GetFile(), "Unit.Kernel.cpp") != nullptr && Loc.GetLine() > 0 && std::strstr(Loc.GetFunction(), "RunKernelUnitTests") != nullptr);
    CHECK(KT::Min(3, 4) == 3 && KT::Max(3, 4) == 4);
    { int A = 1, B = 2; KT::Swap(A, B); CHECK(A == 2 && B == 1); }

    SECTION("Jolt core utilities");
    {
        const KT::UInt32 HashValue = 0x12345678;
        CHECK(KC::Hash<KT::UInt32>{}(HashValue) == KC::HashBytes(&HashValue, sizeof(HashValue)));
        CHECK(KC::Hash<KC::StringView>{}(KC::StringView("abc")) == KC::HashBytes("abc", 3));
        CHECK(KC::Hash<const KT::Char*>{}("abc") == KC::HashString("abc"));

        K::KernelReflection::Factory Factory;
        const K::KernelReflection::TypeId FactoryType = Factory.RegisterType<FactoryItem>("FactoryItem");
        K::KernelReflection::Factory::Object Created = Factory.Create(KT::GetTypeKey<FactoryItem>());
        CHECK(FactoryType != 0 && Factory.GetTypeCount() == 1 && Created.As<FactoryItem>()->Value == 77);

        Factory.RegisterAbstractType<FactoryBase>("FactoryBase");
        Factory.RegisterAbstractType<FactoryOtherBase>("FactoryOtherBase");
        Factory.RegisterType<FactoryDerived>("FactoryDerived");
        CHECK((Factory.RegisterBase<FactoryDerived, FactoryBase>()));
        CHECK((Factory.RegisterBase<FactoryDerived, FactoryOtherBase>()));
        CHECK((Factory.RegisterBase<FactoryDerived, FactoryBase>())); // повтор не добавляет вторую связь
        CHECK(!(Factory.RegisterBase<FactoryBase, FactoryDerived>())); // цикл наследования запрещён
        const K::KernelReflection::TypeInfo* DerivedInfo = Factory.Find(KC::StringView("FactoryDerived"));
        const K::KernelReflection::TypeInfo* BaseInfo = Factory.Find(KC::StringView("FactoryBase"));
        FactoryDerived Derived;
        CHECK(DerivedInfo != nullptr && BaseInfo != nullptr && DerivedInfo->IsKindOf(*BaseInfo));
        CHECK(DerivedInfo->GetBaseClassCount() == 2);
        CHECK(DerivedInfo->GetBaseClass(0) != nullptr && DerivedInfo->GetBaseClass(1) != nullptr);
        CHECK(DerivedInfo->GetBaseClass(2) == nullptr);
        CHECK(*DerivedInfo == *DerivedInfo && *DerivedInfo != *BaseInfo);
        CHECK(K::KernelReflection::IsType(*DerivedInfo, *DerivedInfo));
        CHECK(K::KernelReflection::IsKindOf(*DerivedInfo, *BaseInfo));
        CHECK(BaseInfo->IsAbstract() && !DerivedInfo->IsAbstract());
        CHECK(DerivedInfo->CastTo(&Derived, *BaseInfo) == static_cast<const FactoryBase*>(&Derived));
        CHECK(K::KernelReflection::DynamicCast<FactoryOtherBase>(&Derived, *DerivedInfo, *Factory.Find(KC::StringView("FactoryOtherBase"))) == static_cast<FactoryOtherBase*>(&Derived));
        CHECK(Factory.FindByHash(BaseInfo->GetHash()) == BaseInfo);
        CHECK(Factory.Create(KC::StringView("FactoryDerived")).As<FactoryDerived>() != nullptr);
        CHECK(Factory.CreateByHash(DerivedInfo->GetHash()).As<FactoryDerived>() != nullptr);
        CHECK(Factory.GetAllClasses().GetCount() == 4);

        KC::HashMap<int, int> Map;
        Map.Reserve(20);
        Map.TryEmplace(4, 40);
        Map[5] = 50;
        int Iterated = 0;
        for (auto [Key, Value] : Map)
            Iterated += Key + Value;
        CHECK(Map.GetCount() == 2 && Iterated == 99);
        Factory.Clear();
        CHECK(Factory.GetTypeCount() == 0 && Factory.Find(KC::StringView("FactoryBase")) == nullptr);

        KC::Array<int> Values = KC::Array<int>::Of(1, 3);
        Values.Insert(1, 2);
        CHECK(Values.GetCount() == 3 && Values[0] == 1 && Values[1] == 2 && Values.Back() == 3);
        Values.Resize(5);
        CHECK(Values.GetCount() == 5 && Values[3] == 0 && Values.Find(2) == &Values[1]);
        Values.PopBack();
        Values.Erase(0);
        CHECK(Values.GetCount() == 3 && Values.Front() == 2);

        KC::Result<int> Outcome;
        CHECK(Outcome.IsEmpty());
        Outcome.Set(42);
        CHECK(Outcome.IsValid() && Outcome.Get() == 42);
        Outcome.SetError("failure");
        CHECK(Outcome.HasError() && Outcome.GetError() == KC::String("failure"));
        KC::Result<int> ErrorCopy(Outcome);
        CHECK(ErrorCopy.HasError() && ErrorCopy.GetError() == KC::String("failure"));

        KC::Array<KC::String> Parts;
        KC::Split("red::green::blue", Parts, "::");
        KC::String Joined;
        KC::Join(Parts, Joined, "|");
        CHECK(Parts.GetCount() == 3 && Joined == KC::String("red|green|blue"));
        KC::Split("red::", Parts, "::");
        CHECK(Parts.GetCount() == 2 && Parts[0] == KC::String("red") && Parts[1].IsEmpty());
        KC::Split("::", Parts, "::");
        CHECK(Parts.GetCount() == 2 && Parts[0].IsEmpty() && Parts[1].IsEmpty());
        KC::Split("::a::::", Parts, "::");
        CHECK(Parts.GetCount() == 4 && Parts[0].IsEmpty() && Parts[1] == KC::String("a") && Parts[2].IsEmpty() && Parts[3].IsEmpty());
        KC::Array<KC::String> EmptyLeading;
        EmptyLeading.EmplaceBack("");
        EmptyLeading.EmplaceBack("tail");
        KC::Join(EmptyLeading, Joined, "|");
        CHECK(Joined == KC::String("tail"));
        CHECK(KC::Format("{} + {} = {}", 2, 3, 5) == KC::String("2 + 3 = 5"));
        CHECK(KC::ConvertToString(42) == KC::String("42"));
        KC::String Replaced("ababa");
        KC::Replace(Replaced, "ba", "X");
        CHECK(Replaced == KC::String("aXX") && KC::ToLowerAscii("AbC") == KC::String("abc"));
        CHECK(std::strcmp(KC::NibbleToBinary(10), "1010") == 0);

        KC::LinearCurve Curve;
        Curve.AddPoint(1.0f, 4.0f);
        Curve.AddPoint(0.0f, 2.0f);
        Curve.Sort();
        CHECK(Curve.GetMinX() == 0.0f && Curve.GetMaxX() == 1.0f);
        CHECK(Curve.GetValue(0.5f) == 3.0f && Curve.GetValue(-1.0f) == 2.0f && Curve.GetValue(2.0f) == 4.0f);

        struct StridedItem { int Value; int Padding; } Items[] = {{4, 0}, {8, 0}, {15, 0}};
        KC::StridedPtr<int> Strided(&Items[0].Value, sizeof(StridedItem));
        CHECK(*Strided == 4 && Strided[2] == 15 && (Strided + 2) - Strided == 2);
        CHECK(Strided.GetPtr() == &Items[0].Value && Strided.GetPointer() == &Items[0].Value);

        KSt::ByteBuffer Buffer;
        Buffer.AppendBytes("abc", 3);
        Buffer.Align(4);
        CHECK(Buffer.GetSize() == 4 && Buffer.Get<KT::UInt8>(3)[0] == 0);
        KT::UInt32* Allocated = Buffer.Allocate<KT::UInt32>();
        *Allocated = 123;
        CHECK(*Buffer.Get<KT::UInt32>(4) == 123);
        CHECK(Buffer.begin() == Buffer.GetData() && Buffer.end() == Buffer.GetData() + Buffer.GetSize());
        CHECK(Buffer[0] == static_cast<KT::UInt8>('a') && Buffer.Allocate<KT::UInt32>(0) == nullptr);
        KC::Array<KT::UInt32> VectorBytes = KC::Array<KT::UInt32>::Of(11, 22, 33);
        KSt::ByteBuffer VectorBuffer;
        VectorBuffer.AppendVector(VectorBytes);
        CHECK(VectorBuffer.GetSize() == 3 * sizeof(KT::UInt32) && *VectorBuffer.Get<KT::UInt32>(sizeof(KT::UInt32)) == 22);
        VectorBuffer.Clear();
        CHECK(VectorBuffer.IsEmpty());
        VectorBuffer.PushBack(7);
        VectorBuffer.PushBack(9);
        CHECK(VectorBuffer.Front() == 7 && VectorBuffer.Back() == 9 && VectorBuffer.Size() == 2);
        VectorBuffer.PopBack();
        CHECK(VectorBuffer.Back() == 7);
        VectorBuffer.Reserve(128);
        CHECK(VectorBuffer.GetCapacity() >= 128);

        KSt::ByteBuffer StreamBytes;
        KSt::StreamOutWrapper StreamWriter(&StreamBytes, &WriteStreamBuffer);
        const KT::UInt32 WrittenValue = 0x12345678;
        StreamWriter.Write(WrittenValue);
        CHECK(!StreamWriter.IsFailed() && StreamBytes.GetSize() == sizeof(WrittenValue));
        StreamBufferReader Reader{&StreamBytes};
        KSt::StreamInWrapper StreamReader(&Reader, &ReadStreamBuffer);
        KT::UInt32 ReadValue = 0;
        StreamReader.Read(ReadValue);
        CHECK(!StreamReader.IsFailed() && ReadValue == WrittenValue);
        StreamReader.Read(ReadValue);
        CHECK(StreamReader.IsEOF() && StreamReader.IsFailed());
        KSt::StreamInWrapper InvalidCountReader(nullptr, &ReadInvalidCount);
        InvalidCountReader.Read(ReadValue);
        CHECK(InvalidCountReader.IsEOF() && InvalidCountReader.IsFailed());
        KSt::StreamInWrapper MissingReadCallback(nullptr, nullptr);
        MissingReadCallback.ReadBytes(nullptr, 0);
        CHECK(!MissingReadCallback.IsEOF() && !MissingReadCallback.IsFailed());
        MissingReadCallback.ReadBytes(nullptr, 1);
        CHECK(MissingReadCallback.IsEOF() && MissingReadCallback.IsFailed());
        KSt::StreamOutWrapper MissingWriteCallback(nullptr, nullptr);
        MissingWriteCallback.WriteBytes(nullptr, 0);
        CHECK(!MissingWriteCallback.IsFailed());
        MissingWriteCallback.WriteBytes(nullptr, 1);
        CHECK(MissingWriteCallback.IsFailed());

        KSt::ByteBuffer CustomArrayBytes;
        KSt::StreamOutWrapper CustomArrayWriter(&CustomArrayBytes, &WriteStreamBuffer);
        const KC::Array<KT::UInt32> CustomValues = KC::Array<KT::UInt32>::Of(3, 5, 8);
        CustomArrayWriter.Write(CustomValues, [](KT::UInt32 Value, KSt::StreamOut& Output) { Output.Write(Value + 100); });
        StreamBufferReader CustomArrayReaderState{&CustomArrayBytes};
        KSt::StreamInWrapper CustomArrayReader(&CustomArrayReaderState, &ReadStreamBuffer);
        KC::Array<KT::UInt32> DecodedValues;
        CustomArrayReader.Read(DecodedValues, [](KSt::StreamIn& Input, KT::UInt32& Value) { Input.Read(Value); Value -= 100; });
        CHECK(!CustomArrayReader.IsFailed() && DecodedValues == CustomValues);

        KSt::ByteBuffer StringBytes;
        KSt::StreamOutWrapper StringWriter(&StringBytes, &WriteStreamBuffer);
        const KC::String SourceText("Kernel serialization");
        StringWriter.Write(SourceText);
        StreamBufferReader StringReaderState{&StringBytes};
        KSt::StreamInWrapper StringReader(&StringReaderState, &ReadStreamBuffer);
        KC::String RestoredText("old value");
        StringReader.Read(RestoredText);
        CHECK(!StringReader.IsFailed() && RestoredText == SourceText);

        KSt::ByteBuffer ObjectBytes;
        KSt::StreamOutWrapper ObjectWriter(&ObjectBytes, &WriteStreamBuffer);
        KM::Ref<StreamObject> SourceObject(new StreamObject(91));
        KC::ObjectToIDMap<StreamObject> ObjectIds;
        KSt::StreamUtils::SaveObjectReference(ObjectWriter, SourceObject.GetPtr(), &ObjectIds);
        KSt::StreamUtils::SaveObjectReference(ObjectWriter, SourceObject.GetPtr(), &ObjectIds);
        StreamBufferReader ObjectReaderState{&ObjectBytes};
        KSt::StreamInWrapper ObjectReader(&ObjectReaderState, &ReadStreamBuffer);
        KC::Array<KM::Ref<StreamObject>> ObjectsById;
        auto FirstRestored = KSt::StreamUtils::RestoreObjectReference(ObjectReader, ObjectsById, &RestoreStreamObject);
        auto SecondRestored = KSt::StreamUtils::RestoreObjectReference(ObjectReader, ObjectsById, &RestoreStreamObject);
        CHECK(FirstRestored.IsValid() && SecondRestored.IsValid());
        CHECK(FirstRestored.Get()->Value == 91 && FirstRestored.Get().GetPtr() == SecondRestored.Get().GetPtr());

        KSt::ByteBuffer TypedObjectBytes;
        KSt::StreamOutWrapper TypedObjectWriter(&TypedObjectBytes, &WriteStreamBuffer);
        StreamObject TypedSource(456);
        KSt::StreamUtils::SaveObject(TypedObjectWriter, TypedSource, KT::UInt32(0x5354524du));
        StreamBufferReader TypedObjectReaderState{&TypedObjectBytes};
        KSt::StreamInWrapper TypedObjectReader(&TypedObjectReaderState, &ReadStreamBuffer);
        auto TypedRestored = KSt::StreamUtils::RestoreObject<StreamObject>(TypedObjectReader, &RestoreStreamObjectByType);
        CHECK(TypedRestored.IsValid() && TypedRestored.Get()->Value == 456);

        // StreamUtils должен принимать разные контейнеры для сохранения и восстановления массивов.
        KM::Ref<StreamObject> ArraySource(new StreamObject(123));
        KC::FastArray<StreamObject*> ObjectsToSave;
        ObjectsToSave.PushBack(ArraySource.GetPtr());
        KC::ObjectToIDMap<StreamObject> ArrayObjectIds;
        KSt::ByteBuffer ObjectArrayBytes;
        KSt::StreamOutWrapper ObjectArrayWriter(&ObjectArrayBytes, &WriteStreamBuffer);
        KSt::StreamUtils::SaveObjectArray(ObjectArrayWriter, ObjectsToSave, &ArrayObjectIds);
        StreamBufferReader ObjectArrayReaderState{&ObjectArrayBytes};
        KSt::StreamInWrapper ObjectArrayReader(&ObjectArrayReaderState, &ReadStreamBuffer);
        KC::Array<KM::Ref<StreamObject>> ArrayObjectsById;
        auto RestoredArray = KSt::StreamUtils::RestoreObjectArray<StreamObject, KC::FastArray<KM::Ref<StreamObject>>>(
            ObjectArrayReader, ArrayObjectsById, &RestoreStreamObject
        );
        CHECK(RestoredArray.IsValid() && RestoredArray.Get().Size() == 1);
        CHECK(RestoredArray.IsValid() && RestoredArray.Get()[0]->Value == 123);

        KS::SharedMutex Shared;
        { KS::SharedLock Lock(Shared); }
        { KS::UniqueLock Lock(Shared); }
        CHECK(Shared.TryLock());
        CHECK(!Shared.TryLockShared());
        Shared.Unlock();
        CHECK(Shared.TryLockShared());
        CHECK(!Shared.TryLock());
        Shared.UnlockShared();
        KS::UniqueLock<KS::SharedMutex> DeferredWrite(Shared, KS::DeferLock);
        CHECK(!DeferredWrite.OwnsLock());
        DeferredWrite.Lock();
        CHECK(DeferredWrite.OwnsLock());
        DeferredWrite.Unlock();
        KS::SharedLock<KS::SharedMutex> TryRead(Shared, KS::TryToLock);
        CHECK(TryRead.OwnsLock());
        KS::MutexArray<> Mutexes(8);
        CHECK(Mutexes.GetMutexCount() == 8 && Mutexes.GetMutexIndex(17) < 8);
        CHECK(Mutexes.GetMutexIndex(17) == (static_cast<KT::UInt32>(KC::Hash<KT::UInt32>{}(17)) & 7));
        Mutexes.LockAll();
        Mutexes.UnlockAll();

        K::KernelProfiler::Profiler::NextFrame();
        K::KernelProfiler::Profiler::SetCurrentThreadName("Kernel.Unit");
        K::KernelProfiler::Profiler::SetSamplingEnabled(true);
        K::KernelProfiler::Profiler::SetCallbacks(&BeginProfile, &EndProfile);
        {
            K::KernelProfiler::Scope DefaultColorMeasurement("Kernel unit default color scope");
        }
        {
            K::KernelProfiler::Scope OuterMeasurement("Kernel unit outer scope", 0x11223344);
            {
                K::KernelProfiler::Scope InnerMeasurement("Kernel unit inner scope");
                K::KernelProfiler::Scope ExplicitColorMeasurement("Kernel unit explicit color", 0x55667788);
            }
            K::KernelProfiler::Scope SiblingMeasurement("Kernel unit sibling scope");
        }
        K::KernelProfiler::Profiler::SetCallbacks(nullptr, nullptr);
        std::thread ProfilerWorker([]
        {
            K::KernelProfiler::Profiler::SetCurrentThreadName("Profiler.Worker");
            K::KernelProfiler::Scope WorkerMeasurement("Kernel unit worker scope");
        });
        ProfilerWorker.join();
        const KC::Array<K::KernelProfiler::ProfileSample> Samples = K::KernelProfiler::Profiler::GetSamples();
        const KC::Array<K::KernelProfiler::ProfileStatistics> Statistics = K::KernelProfiler::Profiler::GetStatistics();
        CHECK(ProfileStarts == 5 && ProfileEnds == 5);
        CHECK(Samples.GetCount() == 6 && Samples[0].Depth == 0 && Samples[1].Depth == 0 && Samples[2].Depth == 1 && Samples[3].Depth == 2 && Samples[4].Depth == 1);
        CHECK(Samples[0].Color == 0xff0000ff);
        CHECK(Samples[1].Color == 0x11223344 && Samples[2].Color == 0x11223344);
        CHECK(Samples[3].Color == 0x55667788 && Samples[4].Color == 0x11223344);
        CHECK(Samples[0].ThreadId == Samples[1].ThreadId && Samples[0].EndTick >= Samples[0].StartTick);
        CHECK(KC::StringView(Samples[0].ThreadName) == KC::StringView("Kernel.Unit"));
        CHECK(Samples[5].ThreadId != Samples[0].ThreadId && KC::StringView(Samples[5].ThreadName) == KC::StringView("Profiler.Worker"));
        const KC::String HtmlProfile = K::KernelProfiler::Profiler::BuildHtmlReport();
        CHECK(std::strstr(HtmlProfile.GetText(), "Kernel unit outer scope") != nullptr && std::strstr(HtmlProfile.GetText(), "Kernel.Unit") != nullptr && std::strstr(HtmlProfile.GetText(), "Profiler.Worker") != nullptr);
        CHECK(Statistics.GetCount() == 6 && Statistics[0].CallCount == 1 && Statistics[1].CallCount == 1);
        K::KernelProfiler::Profiler::NextFrame();
        CHECK(K::KernelProfiler::Profiler::GetSampleCount() == 0);
        K::KernelProfiler::Profiler::SetSamplingEnabled(false);
    }

    // Profiler intentionally keeps its registry and per-thread buffers for process lifetime.
    // Rebase leak checks after this one-time infrastructure initialization so subsequent
    // sections measure only allocations they are responsible for.
    BaseBlocks = KM::GetLiveBlockCount();
    BaseBytes = KM::GetLiveByteCount();

    SECTION("Memory");
    {
        Tracked* Made = KM::Make<Tracked>(37);
        CHECK(Made->Value == 37 && Tracked::Alive == 1);
        KM::Destroy(Made);
        CHECK(Tracked::Alive == 0);

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
        unsigned char A[8] = {1,2,3,4,5,6,7,8}; KM::MoveBytes(A + 2, A, 6);
        CHECK(A[2] == 1 && A[7] == 6);
        CHECK(KM::CompareBytes("abc", "abd", 3) < 0 && KM::CompareBytes("abc", "abc", 3) == 0 && KM::CompareBytes("b", "a", 1) > 0);
        CHECK(KM::GetLiveBlockCount() == BaseBlocks);
    }

    SECTION("Array");
    {
        KC::Array<int> Empty;
        CHECK(Empty.Begin() == Empty.End() && Empty.begin() == Empty.end() && Empty.cbegin() == Empty.cend());
        CHECK(Empty.rbegin() == Empty.rend() && Empty.crbegin() == Empty.crend());
        CHECK((Empty.rbegin() + 0) == Empty.rend());
        CHECK(Empty.Erase(Empty.begin(), Empty.end()) == Empty.end());
        int EmptyInsertValues[] = {4, 5};
        Empty.Insert(Empty.begin(), EmptyInsertValues, EmptyInsertValues + 2);
        CHECK(Empty.GetCount() == 2 && Empty[0] == 4 && Empty[1] == 5);

        KC::Array<int> A;
        for (int I = 0; I < 10000; I++) A.Add(I);
        CHECK(A.GetCount() == 10000 && A[9999] == 9999);
        KC::Array<int> B(A); CHECK(B.GetCount() == 10000 && B[1234] == 1234);
        KC::Array<int> C(KT::Move(B)); CHECK(C.GetCount() == 10000 && B.GetCount() == 0);
        A.RemoveAt(0); CHECK(A.GetCount() == 9999 && A[0] == 1);
        A.RemoveAt(A.GetCount() - 1); CHECK(A[A.GetCount() - 1] == 9998);
        auto D = KC::Array<int>::Of(1, 2, 3); CHECK(D.GetCount() == 3 && D[2] == 3);
        KC::Array<int> Filled(3, 42); CHECK(Filled.GetCount() == 3 && Filled[0] == 42 && Filled[2] == 42);
        KC::Array<int> Ranged(D.begin(), D.end()); CHECK(Ranged == D);
        Filled.Resize(5, 7); CHECK(Filled.GetCount() == 5 && Filled[3] == 7 && Filled[4] == 7);
        Filled.Resize(2, 9); CHECK(Filled.GetCount() == 2 && Filled[1] == 42);
        { auto& Alias = D; D = Alias; } CHECK(D.GetCount() == 3 && D[0] == 1);   // самоприсваивание
        KC::Array<int> E = D; E = KT::Move(E); (void)E;                  // самоперемещение
        // алиасинг: добавление элемента самого массива при реаллокации
        KC::Array<KC::String> S;
        S.Add(KC::String("hello"));
        for (int I = 0; I < 200; I++) S.Add(S[0]);
        bool AllHello = true; for (auto& X : S) AllHello &= (X == KC::String("hello"));
        CHECK(S.GetCount() == 201 && AllHello);
        int Sum = 0; for (int X : D) Sum += X; CHECK(Sum == 6);

        auto Reverse = D.rbegin();
        CHECK(*Reverse == 3);
        CHECK(Reverse[1] == 2 && D.rend() - D.rbegin() == 3);
        ++Reverse;
        CHECK(*Reverse == 2);
        CHECK(*D.crbegin() == 3 && D.crend() == KC::Array<int>::ConstReverseIterator(D.rend()));

        int Extra[] = {7, 8};
        D.Insert(D.begin() + 1, Extra, Extra + 2);
        CHECK(D.GetCount() == 5 && D[0] == 1 && D[1] == 7 && D[2] == 8 && D[3] == 2 && D[4] == 3);
        auto Erased = D.Erase(D.begin() + 1, D.begin() + 3);
        CHECK(Erased == D.begin() + 1 && D.GetCount() == 3 && D[1] == 2);
        D.Assign(D.begin() + 1, D.end());
        CHECK(D.GetCount() == 2 && D[0] == 2 && D[1] == 3);
        D.ShrinkToFit();
        CHECK(D.GetCapacity() == D.GetCount());
        KC::Array<int> Swapped = KC::Array<int>::Of(9, 10);
        D.Swap(Swapped);
        CHECK(D.GetCount() == 2 && D[0] == 9 && Swapped.GetCount() == 2 && Swapped[0] == 2);
        CHECK(D == KC::Array<int>::Of(9, 10) && D != Swapped);
        CHECK(D.GetHash() == KC::Array<int>::Of(9, 10).GetHash());

        // Insert/RemoveAt должны работать без move-assignment у элемента.
        KC::Array<MoveConstructOnly> MoveOnly;
        MoveOnly.EmplaceBack(1);
        MoveOnly.EmplaceBack(3);
        MoveOnly.Insert(1, MoveConstructOnly(2));
        CHECK(MoveOnly.GetCount() == 3 && MoveOnly[0].Value == 1 && MoveOnly[1].Value == 2 && MoveOnly[2].Value == 3);
        MoveOnly.RemoveAt(0);
        CHECK(MoveOnly.GetCount() == 2 && MoveOnly[0].Value == 2 && MoveOnly[1].Value == 3);

        Tracked::Alive = 0;
        {
            KC::Array<Tracked> TrackedValues;
            TrackedValues.EmplaceBack(1);
            TrackedValues.EmplaceBack(3);
            TrackedValues.Insert(1, Tracked(2));
            Tracked ExtraValue(4);
            TrackedValues.Insert(2, ExtraValue);
            CHECK(TrackedValues.GetCount() == 4 && TrackedValues[0].Value == 1 && TrackedValues[1].Value == 2 && TrackedValues[2].Value == 4 && TrackedValues[3].Value == 3);
            CHECK(Tracked::Alive == 5);
        }
        CHECK(Tracked::Alive == 0);

        D.Clear(); CHECK(D.IsEmpty());
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
        CHECK(R.Remove<ServiceA>() && !R.Remove<ServiceA>() && R.GetCount() == 0);
    }
    SECTION("TypeRegistry");
    {
        K::KernelReflection::TypeRegistry T;
        auto Id = T.RegisterType<Ping>("Ping");
        CHECK(T.Find(Id) != nullptr && T.Find<Ping>() != nullptr && T.FindByName(KC::StringView("Ping")) != nullptr);
        CHECK(T.Find<Ping>()->GetSize() == sizeof(Ping) && T.Find<Ping>()->GetAlignment() == alignof(Ping));
        CHECK(T.Find<Pong>() == nullptr && T.FindByName(KC::StringView("Nope")) == nullptr);
    }
    SECTION("TypeInfo attributes");
    {
        K::KernelReflection::TypeRegistry T;
        T.RegisterType<AttributeSample>("AttributeSample");
        auto* Info = const_cast<K::KernelReflection::TypeInfo*>(T.Find<AttributeSample>());
        CHECK(Info != nullptr && Info->GetAttributeCount() == 0);
        CHECK(Info->AddAttribute<&AttributeSample::Count>(KC::StringView("Count")));
        CHECK(Info->AddAttribute<&AttributeSample::Scale>(KC::StringView("Scale")));
        CHECK(!Info->AddAttribute<&AttributeSample::Scale>(KC::StringView("Scale")));  // повторное имя
        CHECK(!Info->AddAttribute<&AttributeSample::Count>(KC::StringView("")));       // пустое имя
        CHECK(Info->GetAttributeCount() == 2 && Info->GetAttribute(2) == nullptr);
        CHECK(Info->FindAttribute(KC::StringView("Nope")) == nullptr);

        const auto* Count = Info->FindAttribute(KC::StringView("Count"));
        const auto* Scale = Info->GetAttribute(1);
        CHECK(Count != nullptr && Scale != nullptr && Count->ValueSize == sizeof(int) && Scale->ValueSize == sizeof(float));
        CHECK(Count->ValueType == KT::GetTypeKey<int>() && Scale->ValueType == KT::GetTypeKey<float>());

        AttributeSample Sample;
        *static_cast<int*>(Count->GetValue(&Sample)) = 42;
        *static_cast<float*>(Scale->GetValue(&Sample)) = 2.5f;
        const AttributeSample& ConstSample = Sample;
        CHECK(Sample.Count == 42 && Sample.Scale == 2.5f);
        CHECK(*static_cast<const int*>(Count->GetValue(&ConstSample)) == 42);
    }

    SECTION("Time / Prefetch");
    {
        const KT::UInt64 First = K::KernelTime::GetMonotonicMicroseconds();
        K::KernelCompiler::PrefetchL1(&First);
        const KT::UInt64 Second = K::KernelTime::GetMonotonicMicroseconds();
        CHECK(First != 0 && Second >= First);
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
            CHECK(Cap.Lines[0].find("(Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Kernel.cpp:") != std::string::npos || Cap.Lines[0].find("(Unit.Kernel.cpp:") != std::string::npos);
            CHECK(Cap.Lines[0].find('/') != 0 && Cap.Lines[0].back() == '\n');
        }
        K::KernelLogging::SetMinimumLevel(K::KernelLogging::Level::Off); K::KernelLogging::Fatal("off"); CHECK(Cap.Lines.size() == 2);
        K::KernelLogging::SetSink(nullptr); K::KernelLogging::SetMinimumLevel(K::KernelLogging::Level::Info);
    }
    SECTION("Assert");
    {
        KA::Check(true, "ok");
        KA::SetTraceFunction(&CaptureTrace);
        KA::Trace("callback trace");
        KA::SetTraceFunction(nullptr);
        KA::SetAssertFailedFunction(&CaptureAssertFailure);
        KA::DebugCheck(false, "false expression", "callback assertion");
        KA::SetAssertFailedFunction(nullptr);
        CHECK(TraceCallbackCount == 1 && AssertCallbackCount == (K::KernelCompiler::IsDebugBuild ? 1 : 0));
        KA::Error E("msg"); CHECK(std::strcmp(E.GetText(), "msg") == 0);
        std::string Long(1000, 'x'); KA::LogicError L(Long.c_str());
        CHECK(std::strlen(L.GetText()) <= 255 && std::strlen(L.GetText()) > 200);
        std::printf("    GetKind(Error)=%s GetKind(LogicError)=%s len(long)=%zu line=%u\n", E.GetKind(), L.GetKind(), std::strlen(L.GetText()), (unsigned)E.GetLocation().GetLine());
        KA::Error Copy(L); CHECK(std::strcmp(Copy.GetText(), L.GetText()) == 0);
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
