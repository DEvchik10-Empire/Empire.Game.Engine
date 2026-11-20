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
                    // Базовый бинарный поток чтения (перенесён из Jolt 5.7.0, Core/StreamIn.h).
                    class StreamIn : public KernelTypes::NonCopyable
                    {
                        public:
                            virtual ~StreamIn() = default;
                            virtual void ReadBytes(void* Destination, KernelTypes::Size ByteCount) = 0;
                            [[nodiscard]] virtual KernelTypes::Bool IsEOF() const = 0;
                            [[nodiscard]] virtual KernelTypes::Bool IsFailed() const = 0;

                            template <typename T>
                                requires KernelTypes::IsTriviallyCopyable<T>
                            void Read(T& Value)
                            {
                                ReadBytes(&Value, sizeof(T));
                            }

                            template <typename T>
                                requires KernelTypes::IsTriviallyCopyable<T>
                            void Read(KernelContainers::Array<T>& Values)
                            {
                                KernelTypes::UInt32 Count = static_cast<KernelTypes::UInt32>(Values.GetCount());
                                Read(Count);
                                if (IsEOF() || IsFailed())
                                {
                                    Values.Clear();
                                    return;
                                }

                                Values.Resize(Count);
                                if (Count > 0)
                                    ReadBytes(Values.GetData(), static_cast<KernelTypes::Size>(Count) * sizeof(T));
                                if (IsEOF() || IsFailed())
                                    Values.Clear();
                            }

                            // Чтение массива с пользовательским декодированием каждого элемента (Jolt Core/StreamIn.h).
                            template <typename T, typename Reader>
                            void Read(KernelContainers::Array<T>& Values, Reader&& ReadElement)
                            {
                                KernelTypes::UInt32 Count = static_cast<KernelTypes::UInt32>(Values.GetCount());
                                Read(Count);
                                if (IsEOF() || IsFailed())
                                {
                                    Values.Clear();
                                    return;
                                }

                                Values.Resize(Count);
                                for (KernelTypes::UInt32 Index = 0; Index < Count; ++Index)
                                {
                                    ReadElement(*this, Values[Index]);
                                    if (IsEOF() || IsFailed())
                                    {
                                        Values.Clear();
                                        return;
                                    }
                                }
                            }

                            // Строка: UInt32 длина, затем ровно Length байт без завершающего нуля (Jolt Core/StreamIn.h).
                            void Read(KernelContainers::String& Value)
                            {
                                KernelTypes::UInt32 Count = static_cast<KernelTypes::UInt32>(Value.GetCount());
                                Read(Count);
                                if (IsEOF() || IsFailed())
                                {
                                    Value.Clear();
                                    return;
                                }

                                KernelContainers::Array<KernelTypes::Char> Characters(Count);
                                if (Count > 0)
                                    ReadBytes(Characters.GetData(), Count);
                                if (IsEOF() || IsFailed())
                                {
                                    Value.Clear();
                                    return;
                                }

                                Value.Clear();
                                Value.Append(KernelContainers::StringView(Characters.GetData(), Count));
                            }
                    };
                }
            }
        }
    }
}
