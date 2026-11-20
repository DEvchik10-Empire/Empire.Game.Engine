#include "Empire.Engine/Engine.Kernel/Kernel.Profiler/Profiler.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Time/Time.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelProfiler
                {
                    namespace
                    {
                        inline constexpr KernelTypes::UInt32 MaximumSamplesPerThread = 65536;
                        inline constexpr KernelTypes::UInt32 DefaultProfileColor = 0xff0000ff;

                        struct ThreadBuffer
                        {
                            ProfileSample Samples[MaximumSamplesPerThread]{};
                            KernelSync::Atomic<KernelTypes::UInt32> SampleCount{0};
                            KernelSync::Atomic<KernelTypes::Bool> Overflowed{false};
                            KernelTypes::UInt32 ThreadId = 0;
                            KernelTypes::Char ThreadName[48] = "Thread";
                            KernelTypes::Bool Active = true; // Accessed only under Registry::Guard.
                        };

                        struct Registry
                        {
                            KernelSync::Mutex Guard;
                            KernelContainers::Array<ThreadBuffer*> Buffers;
                        };

                        Registry& GetRegistry()
                        {
                            // Intentionally process-lifetime storage: TLS destructors may run during shutdown.
                            static Registry* Value = []()
                            {
                                void* Storage = KernelMemory::Allocate(sizeof(Registry), alignof(Registry));
                                return KernelTypes::ConstructAt(static_cast<Registry*>(Storage));
                            }();
                            return *Value;
                        }

                        KernelSync::Atomic<KernelTypes::UInt32> ThreadIdCounter{0};
                        KernelSync::Atomic<KernelTypes::Bool> SamplingEnabled{false};
                        KernelSync::Atomic<KernelTypes::Bool> CallbacksEnabled{false};
                        KernelSync::Atomic<KernelTypes::Bool> AnyBufferOverflowed{false};
                        KernelSync::Mutex CallbackMutex;
                        KernelTypes::UInt64 ReferenceTick = 0;
                        KernelTypes::UInt64 ReferenceMicroseconds = 0;
                        KernelTypes::Bool HasReferenceTime = false;
                        Profiler::StartMeasurement StartCallback = nullptr;
                        Profiler::EndMeasurement EndCallback = nullptr;

                        thread_local KernelTypes::Char CurrentThreadName[48] = "Thread";
                        thread_local KernelTypes::Bool HasExplicitThreadName = false;
                        thread_local KernelTypes::UInt32 CurrentDepth = 0;
                        thread_local KernelTypes::UInt32 CurrentColor = DefaultProfileColor;

                        void UpdateReferenceTime() noexcept
                        {
                            ReferenceTick = KernelTime::GetProcessorTickCount();
                            ReferenceMicroseconds = KernelTime::GetMonotonicMicroseconds();
                            HasReferenceTime = true;
                        }

                        void CopyText(KernelTypes::Char* Destination, KernelTypes::Size Capacity, KernelContainers::StringView Source) noexcept
                        {
                            if (Capacity == 0)
                                return;
                            KernelTypes::Size Index = 0;
                            for (; Index + 1 < Capacity && Index < Source.GetCount(); ++Index)
                                Destination[Index] = Source[Index];
                            Destination[Index] = '\0';
                        }

                        void DeactivateThreadBuffer(ThreadBuffer* Buffer) noexcept
                        {
                            if (Buffer == nullptr)
                                return;
                            Registry& Data = GetRegistry();
                            KernelSync::LockGuard Lock(Data.Guard);
                            Buffer->Active = false;
                        }

                        struct ThreadBufferHolder
                        {
                            ThreadBuffer* Buffer = nullptr;
                            ~ThreadBufferHolder() { DeactivateThreadBuffer(Buffer); }
                        };

                        thread_local ThreadBufferHolder CurrentThreadBuffer;

                        ThreadBuffer* GetCurrentThreadBuffer() noexcept
                        {
                            if (CurrentThreadBuffer.Buffer != nullptr)
                                return CurrentThreadBuffer.Buffer;

                            ThreadBuffer* Buffer = nullptr;

                                void* Storage = KernelMemory::Allocate(sizeof(ThreadBuffer), alignof(ThreadBuffer));
                                Buffer = KernelTypes::ConstructAt(static_cast<ThreadBuffer*>(Storage));
                                Buffer->ThreadId = ThreadIdCounter.FetchAdd(1, KernelSync::MemoryOrder::Relaxed) + 1;
                                if (!HasExplicitThreadName)
                                {
                                    KernelContainers::String GeneratedName("Thread ");
                                    GeneratedName.Append(KernelContainers::FormatUnsigned(Buffer->ThreadId));
                                    CopyText(CurrentThreadName, sizeof(CurrentThreadName), GeneratedName.GetView());
                                }
                                CopyText(Buffer->ThreadName, sizeof(Buffer->ThreadName), KernelContainers::StringView(CurrentThreadName));

                                Registry& Data = GetRegistry();
                                {
                                    KernelSync::LockGuard Lock(Data.Guard);
                                    Data.Buffers.Add(Buffer);
                                }
                                CurrentThreadBuffer.Buffer = Buffer;
                                return Buffer;

                        }

                        KernelTypes::Bool TryReserveSample(ThreadBuffer& Buffer, KernelTypes::UInt32& Index) noexcept
                        {
                            Index = Buffer.SampleCount.Load(KernelSync::MemoryOrder::Relaxed);
                            while (Index < MaximumSamplesPerThread)
                            {
                                KernelTypes::UInt32 Expected = Index;
                                if (Buffer.SampleCount.CompareExchange(
                                    Expected,
                                    Index + 1,
                                    KernelSync::MemoryOrder::AcquireRelease,
                                    KernelSync::MemoryOrder::Relaxed
                                ))
                                    return true;
                                Index = Expected;
                            }

                            Buffer.Overflowed.Store(true, KernelSync::MemoryOrder::Relaxed);
                            AnyBufferOverflowed.Store(true, KernelSync::MemoryOrder::Relaxed);
                            return false;
                        }

                        void AppendHtmlEscaped(KernelContainers::String& Output, KernelContainers::StringView Value)
                        {
                            for (KernelTypes::Size Index = 0; Index < Value.GetCount(); ++Index)
                            {
                                switch (Value[Index])
                                {
                                    case '&': Output.Append("&amp;"); break;
                                    case '<': Output.Append("&lt;"); break;
                                    case '>': Output.Append("&gt;"); break;
                                    case '\"': Output.Append("&quot;"); break;
                                    case '\'': Output.Append("&#39;"); break;
                                    default: Output.Append(Value[Index]); break;
                                }
                            }
                        }

                        void AppendPercent(KernelContainers::String& Output, KernelTypes::Double Value)
                        {
                            Output.Append(KernelContainers::FormatFloating(Value));
                            Output.Append("%");
                        }
                    }

                    void Profiler::SetCallbacks(StartMeasurement Start, EndMeasurement End) noexcept
                    {
                        KernelSync::LockGuard Lock(CallbackMutex);
                        StartCallback = Start;
                        EndCallback = End;
                        CallbacksEnabled.Store(Start != nullptr && End != nullptr, KernelSync::MemoryOrder::Release);
                    }

                    void Profiler::SetSamplingEnabled(KernelTypes::Bool Enabled) noexcept
                    {
                        SamplingEnabled.Store(Enabled, KernelSync::MemoryOrder::Release);
                    }

                    void Profiler::SetCurrentThreadName(KernelContainers::StringView Name) noexcept
                    {
                        CopyText(CurrentThreadName, sizeof(CurrentThreadName), Name);
                        HasExplicitThreadName = true;
                    }

                    void Profiler::NextFrame() noexcept
                    {
                        // As in Jolt, call at a frame boundary when profiled threads are not inside Scope.
                        Registry& Data = GetRegistry();
                        KernelSync::LockGuard Lock(Data.Guard);
                        for (KernelTypes::Size Index = 0; Index < Data.Buffers.GetCount();)
                        {
                            ThreadBuffer* Buffer = Data.Buffers[Index];
                            if (!Buffer->Active)
                            {
                                KernelTypes::DestroyAt(Buffer);
                                KernelMemory::Deallocate(Buffer);
                                Data.Buffers.Erase(Index);
                                continue;
                            }
                            Buffer->SampleCount.Store(0, KernelSync::MemoryOrder::Release);
                            Buffer->Overflowed.Store(false, KernelSync::MemoryOrder::Relaxed);
                            ++Index;
                        }
                        AnyBufferOverflowed.Store(false, KernelSync::MemoryOrder::Relaxed);
                        UpdateReferenceTime();
                    }

                    KernelTypes::UInt64 Profiler::GetProcessorTicksPerSecond() noexcept
                    {
                        if (!HasReferenceTime)
                            UpdateReferenceTime();
                        const KernelTypes::UInt64 TickNow = KernelTime::GetProcessorTickCount();
                        const KernelTypes::UInt64 TimeNow = KernelTime::GetMonotonicMicroseconds();
                        if (TickNow <= ReferenceTick || TimeNow <= ReferenceMicroseconds)
                            return 0;
                        const KernelTypes::Double Frequency = static_cast<KernelTypes::Double>(TickNow - ReferenceTick) * 1000000.0 /
                            static_cast<KernelTypes::Double>(TimeNow - ReferenceMicroseconds);
                        return Frequency >= static_cast<KernelTypes::Double>(KernelTypes::MaxValue<KernelTypes::UInt64>())
                            ? KernelTypes::MaxValue<KernelTypes::UInt64>()
                            : static_cast<KernelTypes::UInt64>(Frequency);
                    }

                    KernelTypes::UInt32 Profiler::GetSampleCount() noexcept
                    {
                        Registry& Data = GetRegistry();
                        KernelSync::LockGuard Lock(Data.Guard);
                        KernelTypes::UInt64 Total = 0;
                        for (ThreadBuffer* Buffer : Data.Buffers)
                            Total += Buffer->SampleCount.Load(KernelSync::MemoryOrder::Acquire);
                        return Total > KernelTypes::MaxValue<KernelTypes::UInt32>()
                            ? KernelTypes::MaxValue<KernelTypes::UInt32>()
                            : static_cast<KernelTypes::UInt32>(Total);
                    }

                    KernelTypes::Bool Profiler::WasSampleBufferOverflowed() noexcept
                    {
                        if (AnyBufferOverflowed.Load(KernelSync::MemoryOrder::Relaxed))
                            return true;
                        Registry& Data = GetRegistry();
                        KernelSync::LockGuard Lock(Data.Guard);
                        for (ThreadBuffer* Buffer : Data.Buffers)
                            if (Buffer->Overflowed.Load(KernelSync::MemoryOrder::Relaxed))
                                return true;
                        return false;
                    }

                    KernelContainers::Array<ProfileSample> Profiler::GetSamples()
                    {
                        Registry& Data = GetRegistry();
                        KernelSync::LockGuard Lock(Data.Guard);
                        KernelContainers::Array<ProfileSample> Result;
                        KernelTypes::Size Total = 0;
                        for (ThreadBuffer* Buffer : Data.Buffers)
                            Total += Buffer->SampleCount.Load(KernelSync::MemoryOrder::Acquire);
                        Result.Reserve(Total);
                        for (ThreadBuffer* Buffer : Data.Buffers)
                        {
                            const KernelTypes::UInt32 Count = Buffer->SampleCount.Load(KernelSync::MemoryOrder::Acquire);
                            for (KernelTypes::UInt32 Index = 0; Index < Count && Index < MaximumSamplesPerThread; ++Index)
                                Result.Add(Buffer->Samples[Index]);
                        }
                        return Result;
                    }

                    KernelContainers::Array<ProfileStatistics> Profiler::GetStatistics()
                    {
                        const KernelContainers::Array<ProfileSample> Samples = GetSamples();
                        KernelContainers::Array<ProfileStatistics> Result;
                        for (const ProfileSample& Sample : Samples)
                        {
                            const KernelContainers::StringView Name(Sample.Name);
                            const KernelTypes::UInt64 Duration = Sample.EndTick >= Sample.StartTick ? Sample.EndTick - Sample.StartTick : 0;
                            KernelTypes::Size StatisticsIndex = 0;
                            for (; StatisticsIndex < Result.GetCount(); ++StatisticsIndex)
                                if (KernelContainers::StringView(Result[StatisticsIndex].Name) == Name)
                                    break;

                            if (StatisticsIndex == Result.GetCount())
                                Result.Add(ProfileStatistics{Sample.Name, Sample.Color, 1, Duration, Duration, Duration});
                            else
                            {
                                ProfileStatistics& Statistics = Result[StatisticsIndex];
                                ++Statistics.CallCount;
                                Statistics.TotalTicks += Duration;
                                if (Duration < Statistics.MinimumTicks) Statistics.MinimumTicks = Duration;
                                if (Duration > Statistics.MaximumTicks) Statistics.MaximumTicks = Duration;
                            }
                        }
                        for (KernelTypes::Size Index = 1; Index < Result.GetCount(); ++Index)
                        {
                            KernelTypes::Size Position = Index;
                            while (Position > 0 && Result[Position].TotalTicks > Result[Position - 1].TotalTicks)
                            {
                                KernelTypes::Swap(Result[Position], Result[Position - 1]);
                                --Position;
                            }
                        }
                        return Result;
                    }

                    KernelContainers::String Profiler::BuildHtmlReport()
                    {
                        KernelContainers::Array<ProfileSample> Samples = GetSamples();
                        const KernelContainers::Array<ProfileStatistics> Statistics = GetStatistics();
                        for (KernelTypes::Size Index = 1; Index < Samples.GetCount(); ++Index)
                        {
                            KernelTypes::Size Position = Index;
                            while (Position > 0 && Samples[Position].StartTick < Samples[Position - 1].StartTick)
                            {
                                KernelTypes::Swap(Samples[Position], Samples[Position - 1]);
                                --Position;
                            }
                        }
                        KernelContainers::String Html;
                        Html.Append("<!doctype html><html><head><meta charset=\"utf-8\"><title>Empire profiler</title><style>");
                        Html.Append("body{font:14px sans-serif;background:#17191d;color:#e8e8e8;margin:24px}h1,h2{font-weight:600}table{border-collapse:collapse;width:100%;margin:12px 0 28px}th,td{padding:6px 9px;border-bottom:1px solid #383c44;text-align:left}.track{position:relative;height:22px;background:#252a32;min-width:320px}.bar{position:absolute;height:18px;top:2px;min-width:2px;border-radius:3px;overflow:hidden;white-space:nowrap;color:#111;font-size:12px;padding-left:3px;box-sizing:border-box}.muted{color:#aab0bb}</style></head><body><h1>Profiler report</h1>");
                        Html.Append("<p class=\"muted\">Samples: ");
                        Html.Append(KernelContainers::FormatUnsigned(Samples.GetCount()));
                        Html.Append("</p><h2>Timeline</h2><table><thead><tr><th>Thread</th><th>Scope</th><th>Timeline</th><th>Ticks</th><th>Microseconds</th></tr></thead><tbody>");

                        KernelTypes::UInt64 FirstTick = KernelTypes::MaxValue<KernelTypes::UInt64>();
                        KernelTypes::UInt64 LastTick = 0;
                        for (const ProfileSample& Sample : Samples)
                        {
                            if (Sample.StartTick < FirstTick) FirstTick = Sample.StartTick;
                            if (Sample.EndTick > LastTick) LastTick = Sample.EndTick;
                        }
                        const KernelTypes::UInt64 TickRange = LastTick > FirstTick ? LastTick - FirstTick : 1;
                        for (const ProfileSample& Sample : Samples)
                        {
                            const KernelTypes::UInt64 StartOffset = Sample.StartTick >= FirstTick ? Sample.StartTick - FirstTick : 0;
                            const KernelTypes::UInt64 Duration = Sample.EndTick >= Sample.StartTick ? Sample.EndTick - Sample.StartTick : 0;
                            const KernelTypes::Double Left = static_cast<KernelTypes::Double>(StartOffset) * 100.0 / static_cast<KernelTypes::Double>(TickRange);
                            const KernelTypes::Double Width = static_cast<KernelTypes::Double>(Duration) * 100.0 / static_cast<KernelTypes::Double>(TickRange);
                            Html.Append("<tr><td>");
                            AppendHtmlEscaped(Html, KernelContainers::StringView(Sample.ThreadName));
                            Html.Append("</td><td>");
                            for (KernelTypes::UInt8 Depth = 0; Depth < Sample.Depth; ++Depth) Html.Append("&nbsp;&nbsp;");
                            AppendHtmlEscaped(Html, KernelContainers::StringView(Sample.Name != nullptr ? Sample.Name : ""));
                            Html.Append("</td><td><div class=\"track\"><div class=\"bar\" style=\"left:");
                            AppendPercent(Html, Left);
                            Html.Append(";width:");
                            AppendPercent(Html, Width < 0.2 ? 0.2 : Width);
                            Html.Append(";background:#");
                            const KernelTypes::UInt32 RGB = (Sample.Color >> 8) & 0x00ffffffu;
                            static constexpr KernelTypes::Char Hex[] = "0123456789abcdef";
                            for (KernelTypes::Int32 Shift = 20; Shift >= 0; Shift -= 4)
                                Html.Append(Hex[(RGB >> Shift) & 0xf]);
                            Html.Append(";\">");
                            AppendHtmlEscaped(Html, KernelContainers::StringView(Sample.Name != nullptr ? Sample.Name : ""));
                            Html.Append("</div></div></td><td>");
                            Html.Append(KernelContainers::FormatUnsigned(Duration));
                            Html.Append("</td><td>");
                            const KernelTypes::UInt64 Frequency = GetProcessorTicksPerSecond();
                            if (Frequency > 0)
                                Html.Append(KernelContainers::FormatFloating(static_cast<KernelTypes::Double>(Duration) * 1000000.0 / static_cast<KernelTypes::Double>(Frequency)));
                            else
                                Html.Append("n/a");
                            Html.Append("</td></tr>");
                        }

                        Html.Append("</tbody></table><h2>Statistics by scope</h2><table><thead><tr><th>Scope</th><th>Calls</th><th>Total ticks</th><th>Min</th><th>Max</th></tr></thead><tbody>");
                        for (const ProfileStatistics& Item : Statistics)
                        {
                            Html.Append("<tr><td>");
                            AppendHtmlEscaped(Html, KernelContainers::StringView(Item.Name != nullptr ? Item.Name : ""));
                            Html.Append("</td><td>");
                            Html.Append(KernelContainers::FormatUnsigned(Item.CallCount));
                            Html.Append("</td><td>");
                            Html.Append(KernelContainers::FormatUnsigned(Item.TotalTicks));
                            Html.Append("</td><td>");
                            Html.Append(KernelContainers::FormatUnsigned(Item.MinimumTicks));
                            Html.Append("</td><td>");
                            Html.Append(KernelContainers::FormatUnsigned(Item.MaximumTicks));
                            Html.Append("</td></tr>");
                        }
                        Html.Append("</tbody></table></body></html>");
                        return Html;
                    }

                    void Profiler::Begin(
                        const KernelTypes::Char* Name,
                        KernelTypes::UInt32 Color,
                        ProfileSample*& Sample,
                        KernelTypes::Bool& DepthTracked,
                        KernelTypes::UInt32& PreviousColor,
                        EndMeasurement& End,
                        KernelTypes::UInt8* UserData
                    ) noexcept
                    {
                        Sample = nullptr;
                        End = nullptr;
                        PreviousColor = CurrentColor;
                        DepthTracked = SamplingEnabled.Load(KernelSync::MemoryOrder::Acquire);
                        const KernelTypes::Bool UseCallbacks = CallbacksEnabled.Load(KernelSync::MemoryOrder::Acquire);
                        if (!DepthTracked && !UseCallbacks)
                            return;

                        if (DepthTracked)
                        {
                            const KernelTypes::UInt32 SampleDepth = CurrentDepth;
                            const KernelTypes::UInt32 EffectiveColor = Color == 0 ? CurrentColor : Color;
                            CurrentColor = EffectiveColor;
                            ++CurrentDepth;

                            if (ThreadBuffer* Buffer = GetCurrentThreadBuffer())
                            {
                                if (Buffer->SampleCount.Load(KernelSync::MemoryOrder::Relaxed) == 0)
                                    CopyText(Buffer->ThreadName, sizeof(Buffer->ThreadName), KernelContainers::StringView(CurrentThreadName));
                                KernelTypes::UInt32 Index = 0;
                                if (TryReserveSample(*Buffer, Index))
                                {
                                    ProfileSample& NewSample = Buffer->Samples[Index];
                                    NewSample.Name = Name == nullptr ? "" : Name;
                                    NewSample.Color = EffectiveColor;
                                    NewSample.Depth = static_cast<KernelTypes::UInt8>(SampleDepth < 255 ? SampleDepth : 255);
                                    NewSample.StartTick = KernelTime::GetProcessorTickCount();
                                    NewSample.EndTick = 0;
                                    NewSample.ThreadId = Buffer->ThreadId;
                                    NewSample.ThreadName = Buffer->ThreadName;
                                    Sample = &NewSample;
                                }
                            }
                        }

                        if (UseCallbacks)
                        {
                            StartMeasurement Start;
                            {
                                KernelSync::LockGuard Lock(CallbackMutex);
                                Start = StartCallback;
                                End = EndCallback;
                            }
                            if (Start != nullptr && End != nullptr)
                                Start(Name, Color, UserData);
                            else
                                End = nullptr;
                        }
                    }

                    void Profiler::End(
                        ProfileSample* Sample,
                        KernelTypes::Bool DepthTracked,
                        KernelTypes::UInt32 PreviousColor
                    ) noexcept
                    {
                        if (Sample != nullptr)
                            Sample->EndTick = KernelTime::GetProcessorTickCount();
                        if (DepthTracked)
                        {
                            if (CurrentDepth > 0)
                                --CurrentDepth;
                            CurrentColor = PreviousColor;
                        }
                    }
                }
            }
        }
    }
}
