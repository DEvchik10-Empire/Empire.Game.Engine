#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Streams/Streams.StreamIn.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Streams/Streams.StreamOut.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelStreams
                {
                    // Callback-адаптеры бинарных потоков (перенесены из Jolt 5.7.0, Core/StreamWrapper.h; без std::iostream).
                    class StreamInWrapper final : public StreamIn
                    {
                        public:
                            using ReadFunction = KernelTypes::Size (*)(void* Context, void* Destination, KernelTypes::Size ByteCount);

                            StreamInWrapper(void* UserContext, ReadFunction Read) noexcept
                                : Context(UserContext), ReadCallback(Read)
                            {
                            }

                            void ReadBytes(void* Destination, KernelTypes::Size ByteCount) override
                            {
                                if (Failed || ByteCount == 0)
                                    return;

                                if (ReadCallback == nullptr)
                                {
                                    EOFReached = true;
                                    Failed = true;
                                    return;
                                }

                                const KernelTypes::Size ReadCount = ReadCallback(Context, Destination, ByteCount);
                                if (ReadCount != ByteCount)
                                {
                                    EOFReached = true;
                                    Failed = true;
                                }
                            }

                            [[nodiscard]] KernelTypes::Bool IsEOF() const override { return EOFReached; }
                            [[nodiscard]] KernelTypes::Bool IsFailed() const override { return Failed; }

                        private:
                            void* Context;
                            ReadFunction ReadCallback;
                            KernelTypes::Bool EOFReached = false;
                            KernelTypes::Bool Failed = false;
                    };

                    class StreamOutWrapper final : public StreamOut
                    {
                        public:
                            using WriteFunction = KernelTypes::Bool (*)(void* Context, const void* Source, KernelTypes::Size ByteCount);

                            StreamOutWrapper(void* UserContext, WriteFunction Write) noexcept
                                : Context(UserContext), WriteCallback(Write)
                            {
                            }

                            void WriteBytes(const void* Source, KernelTypes::Size ByteCount) override
                            {
                                if (!Failed && ByteCount > 0 && (WriteCallback == nullptr || !WriteCallback(Context, Source, ByteCount)))
                                    Failed = true;
                            }

                            [[nodiscard]] KernelTypes::Bool IsFailed() const override { return Failed; }

                        private:
                            void* Context;
                            WriteFunction WriteCallback;
                            KernelTypes::Bool Failed = false;
                    };
                }
            }
        }
    }
}
