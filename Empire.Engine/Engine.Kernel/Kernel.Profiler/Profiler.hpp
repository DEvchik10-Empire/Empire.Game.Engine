#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

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
                    // ThreadName points into the owning per-thread buffer and remains valid until NextFrame().
                    struct ProfileSample
                    {
                        const KernelTypes::Char* Name = nullptr;
                        KernelTypes::UInt32 Color = 0;
                        KernelTypes::UInt8 Depth = 0;
                        KernelTypes::UInt8 Reserved[3]{};
                        KernelTypes::UInt64 StartTick = 0;
                        KernelTypes::UInt64 EndTick = 0;
                        KernelTypes::UInt32 ThreadId = 0;
                        const KernelTypes::Char* ThreadName = nullptr;
                    };

                    struct ProfileStatistics
                    {
                        const KernelTypes::Char* Name = nullptr;
                        KernelTypes::UInt32 Color = 0;
                        KernelTypes::UInt64 CallCount = 0;
                        KernelTypes::UInt64 TotalTicks = 0;
                        KernelTypes::UInt64 MinimumTicks = 0;
                        KernelTypes::UInt64 MaximumTicks = 0;
                    };

                    // Scope samples can be consumed internally or forwarded to an external tracing backend.
                    class Profiler
                    {
                        public:
                            using StartMeasurement = void (*)(const KernelTypes::Char* Name, KernelTypes::UInt32 Color, KernelTypes::UInt8* UserData);
                            using EndMeasurement = void (*)(KernelTypes::UInt8* UserData);

                            static void SetCallbacks(StartMeasurement Start, EndMeasurement End) noexcept;
                            static void SetSamplingEnabled(KernelTypes::Bool Enabled) noexcept;
                            // Sets the name attached to samples produced by the calling thread.
                            static void SetCurrentThreadName(KernelContainers::StringView Name) noexcept;
                            static void NextFrame() noexcept;
                            [[nodiscard]] static KernelTypes::UInt64 GetProcessorTicksPerSecond() noexcept;
                            [[nodiscard]] static KernelContainers::Array<ProfileSample> GetSamples();
                            [[nodiscard]] static KernelContainers::Array<ProfileStatistics> GetStatistics();
                            // Builds a self-contained HTML timeline and statistics report for the current sample frame.
                            [[nodiscard]] static KernelContainers::String BuildHtmlReport();
                            [[nodiscard]] static KernelTypes::UInt32 GetSampleCount() noexcept;
                            [[nodiscard]] static KernelTypes::Bool WasSampleBufferOverflowed() noexcept;

                        private:
                            friend class Scope;
                            static void Begin(
                                const KernelTypes::Char* Name,
                                KernelTypes::UInt32 Color,
                                ProfileSample*& Sample,
                                KernelTypes::Bool& DepthTracked,
                                KernelTypes::UInt32& PreviousColor,
                                EndMeasurement& End,
                                KernelTypes::UInt8* UserData
                            ) noexcept;
                            static void End(ProfileSample* Sample, KernelTypes::Bool DepthTracked, KernelTypes::UInt32 PreviousColor) noexcept;
                    };

                    // Замер времени жизни scope; передаёт начало/конец подключённому profiler backend-у.
                    class Scope : public KernelTypes::NonCopyable
                    {
                        private:
                            ProfileSample* Sample = nullptr;
                            KernelTypes::Bool DepthTracked = false;
                            KernelTypes::UInt32 PreviousColor = 0;
                            Profiler::EndMeasurement End = nullptr;
                            alignas(16) KernelTypes::UInt8 UserData[64]{};

                        public:
                            explicit Scope(const KernelTypes::Char* Name, KernelTypes::UInt32 Color = 0) noexcept
                            {
                                Profiler::Begin(Name, Color, Sample, DepthTracked, PreviousColor, End, UserData);
                            }

                            ~Scope()
                            {
                                Profiler::End(Sample, DepthTracked, PreviousColor);
                                if (End != nullptr)
                                    End(UserData);
                            }
                    };
                }
            }
        }
    }
}
