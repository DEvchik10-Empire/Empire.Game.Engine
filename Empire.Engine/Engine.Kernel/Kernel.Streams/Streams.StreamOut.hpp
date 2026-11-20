#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

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
                    // Базовый бинарный поток записи (перенесён из Jolt 5.7.0, Core/StreamOut.h).
                    class StreamOut : public KernelTypes::NonCopyable
                    {
                        public:
                            virtual ~StreamOut() = default;
                            virtual void WriteBytes(const void* Source, KernelTypes::Size ByteCount) = 0;
                            [[nodiscard]] virtual KernelTypes::Bool IsFailed() const = 0;

                            template <typename T>
                                requires KernelTypes::IsTriviallyCopyable<T>
                            void Write(const T& Value)
                            {
                                WriteBytes(&Value, sizeof(T));
                            }

                            template <typename T>
                                requires KernelTypes::IsTriviallyCopyable<T>
                            void Write(const KernelContainers::Array<T>& Values)
                            {
                                const KernelTypes::UInt32 Count = static_cast<KernelTypes::UInt32>(Values.GetCount());
                                Write(Count);
                                if (!IsFailed() && Count > 0)
                                    WriteBytes(Values.GetData(), static_cast<KernelTypes::Size>(Count) * sizeof(T));
                            }

                            // Запись массива с пользовательским кодированием каждого элемента (Jolt Core/StreamOut.h).
                            template <typename T, typename Writer>
                            void Write(const KernelContainers::Array<T>& Values, Writer&& WriteElement)
                            {
                                const KernelTypes::UInt32 Count = static_cast<KernelTypes::UInt32>(Values.GetCount());
                                Write(Count);
                                if (IsFailed())
                                    return;

                                for (KernelTypes::UInt32 Index = 0; Index < Count; ++Index)
                                {
                                    WriteElement(Values[Index], *this);
                                    if (IsFailed())
                                        return;
                                }
                            }

                            // Строка: UInt32 длина, затем ровно Length байт без завершающего нуля (Jolt Core/StreamOut.h).
                            void Write(const KernelContainers::String& Value)
                            {
                                const KernelTypes::UInt32 Count = static_cast<KernelTypes::UInt32>(Value.GetCount());
                                Write(Count);
                                if (!IsFailed() && Count > 0)
                                    WriteBytes(Value.GetText(), Count);
                            }
                    };
                }
            }
        }
    }
}
