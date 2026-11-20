#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
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
                    // Байтовый буфер с выравниванием основания по cache line (перенесён из Jolt 5.7.0, Core/ByteBuffer.h).
                    class ByteBuffer : public KernelTypes::NonCopyable
                    {
                        private:
                            KernelTypes::UInt8* Bytes = nullptr;
                            KernelTypes::Size Count = 0;
                            KernelTypes::Size Capacity = 0;

                        public:
                            void Reserve(KernelTypes::Size NewCapacity)
                            {
                                if (NewCapacity <= Capacity)
                                    return;

                                KernelTypes::UInt8* NewBytes = static_cast<KernelTypes::UInt8*>(
                                    KernelMemory::Allocate(NewCapacity, KernelCompiler::CacheLineSize)
                                );
                                if (Count > 0)
                                    KernelMemory::CopyBytes(NewBytes, Bytes, Count);
                                KernelMemory::Deallocate(Bytes);
                                Bytes = NewBytes;
                                Capacity = NewCapacity;
                            }

                            ByteBuffer() = default;
                            ByteBuffer(ByteBuffer&& Other) noexcept
                                : Bytes(Other.Bytes), Count(Other.Count), Capacity(Other.Capacity)
                            {
                                Other.Bytes = nullptr;
                                Other.Count = 0;
                                Other.Capacity = 0;
                            }

                            ~ByteBuffer() { KernelMemory::Deallocate(Bytes); }

                            ByteBuffer& operator=(ByteBuffer&& Other) noexcept
                            {
                                if (this != &Other)
                                {
                                    KernelMemory::Deallocate(Bytes);
                                    Bytes = Other.Bytes;
                                    Count = Other.Count;
                                    Capacity = Other.Capacity;
                                    Other.Bytes = nullptr;
                                    Other.Count = 0;
                                    Other.Capacity = 0;
                                }
                                return *this;
                            }

                            using Iterator = KernelTypes::UInt8*;
                            using ConstIterator = const KernelTypes::UInt8*;

                            [[nodiscard]] KernelTypes::Size GetSize() const noexcept { return Count; }
                            [[nodiscard]] KernelTypes::Size GetCapacity() const noexcept { return Capacity; }
                            [[nodiscard]] KernelTypes::Bool IsEmpty() const noexcept { return Count == 0; }
                            [[nodiscard]] const KernelTypes::UInt8* GetData() const noexcept { return Bytes; }
                            [[nodiscard]] KernelTypes::UInt8* GetData() noexcept { return Bytes; }

                            Iterator begin() noexcept { return Bytes; }
                            Iterator end() noexcept { return Count == 0 ? Bytes : Bytes + Count; }
                            ConstIterator begin() const noexcept { return Bytes; }
                            ConstIterator end() const noexcept { return Count == 0 ? Bytes : Bytes + Count; }

                            KernelTypes::UInt8& operator[](KernelTypes::Size Index)
                            {
                                KernelAssert::DebugCheck(Index < Count, "ByteBuffer::operator[]: индекс вне буфера");
                                return Bytes[Index];
                            }

                            const KernelTypes::UInt8& operator[](KernelTypes::Size Index) const
                            {
                                KernelAssert::DebugCheck(Index < Count, "ByteBuffer::operator[]: индекс вне буфера");
                                return Bytes[Index];
                            }

                            void Clear() noexcept { Count = 0; }
                            [[nodiscard]] KernelTypes::Size Size() const noexcept { return Count; }
                            [[nodiscard]] KernelTypes::Bool Empty() const noexcept { return Count == 0; }
                            [[nodiscard]] KernelTypes::UInt8* Data() noexcept { return Bytes; }
                            [[nodiscard]] const KernelTypes::UInt8* Data() const noexcept { return Bytes; }

                            KernelTypes::UInt8& Front()
                            {
                                KernelAssert::DebugCheck(Count > 0, "ByteBuffer::Front: буфер пуст");
                                return Bytes[0];
                            }

                            const KernelTypes::UInt8& Front() const
                            {
                                KernelAssert::DebugCheck(Count > 0, "ByteBuffer::Front: буфер пуст");
                                return Bytes[0];
                            }

                            KernelTypes::UInt8& Back()
                            {
                                KernelAssert::DebugCheck(Count > 0, "ByteBuffer::Back: буфер пуст");
                                return Bytes[Count - 1];
                            }

                            const KernelTypes::UInt8& Back() const
                            {
                                KernelAssert::DebugCheck(Count > 0, "ByteBuffer::Back: буфер пуст");
                                return Bytes[Count - 1];
                            }

                            void PushBack(KernelTypes::UInt8 Value)
                            {
                                KernelAssert::Check(Count < KernelTypes::MaxValue<KernelTypes::Size>(), "ByteBuffer::PushBack: размер переполнен");
                                const KernelTypes::Size Index = Count;
                                Resize(Count + 1);
                                Bytes[Index] = Value;
                            }

                            void PopBack()
                            {
                                KernelAssert::DebugCheck(Count > 0, "ByteBuffer::PopBack: буфер пуст");
                                --Count;
                            }

                            void Resize(KernelTypes::Size NewSize)
                            {
                                const KernelTypes::Size OldSize = Count;
                                if (NewSize > Capacity)
                                {
                                    KernelTypes::Size NewCapacity = Capacity == 0 ? 64 : Capacity;
                                    while (NewCapacity < NewSize)
                                    {
                                        if (NewCapacity > KernelTypes::MaxValue<KernelTypes::Size>() / 2)
                                        {
                                            NewCapacity = NewSize;
                                            break;
                                        }
                                        NewCapacity *= 2;
                                    }
                                    Reserve(NewCapacity);
                                }
                                Count = NewSize;
                                if (NewSize > OldSize)
                                    KernelMemory::FillBytes(Bytes + OldSize, 0, NewSize - OldSize);
                            }

                            KernelTypes::Size Align(KernelTypes::Size Alignment)
                            {
                                KernelAssert::Check(
                                    Alignment > 0 && (Alignment & (Alignment - 1)) == 0,
                                    "ByteBuffer::Align: выравнивание должно быть степенью двойки"
                                );
                                KernelAssert::Check(
                                    Count <= KernelTypes::MaxValue<KernelTypes::Size>() - (Alignment - 1),
                                    "ByteBuffer::Align: размер переполнен"
                                );
                                const KernelTypes::Size AlignedSize = KernelTypes::AlignUp(Count, Alignment);
                                Resize(AlignedSize);
                                return AlignedSize;
                            }

                            template <typename T>
                            void AppendVector(const KernelContainers::Array<T>& Values)
                            {
                                KernelAssert::Check(
                                    Values.GetCount() <= KernelTypes::MaxValue<KernelTypes::Size>() / sizeof(T),
                                    "ByteBuffer::AppendVector: размер переполнен"
                                );
                                AppendBytes(Values.GetData(), Values.GetCount() * sizeof(T));
                            }

                            void AppendBytes(const void* Source, KernelTypes::Size ByteCount)
                            {
                                const KernelTypes::Size OldSize = Count;
                                KernelAssert::Check(ByteCount == 0 || Source != nullptr, "ByteBuffer::AppendBytes: исходный указатель пуст");
                                KernelAssert::Check(ByteCount <= KernelTypes::MaxValue<KernelTypes::Size>() - Count, "ByteBuffer::AppendBytes: размер переполнен");
                                KernelTypes::Size SourceOffset = 0;
                                KernelTypes::Bool IsInternalSource = false;
                                if (Bytes != nullptr && Source != nullptr)
                                {
                                    const KernelTypes::UIntPtr SourceAddress = reinterpret_cast<KernelTypes::UIntPtr>(Source);
                                    const KernelTypes::UIntPtr BufferAddress = reinterpret_cast<KernelTypes::UIntPtr>(Bytes);
                                    if (SourceAddress >= BufferAddress && SourceAddress < BufferAddress + Count)
                                    {
                                        SourceOffset = static_cast<KernelTypes::Size>(SourceAddress - BufferAddress);
                                        IsInternalSource = true;
                                    }
                                }
                                Resize(Count + ByteCount);
                                if (ByteCount > 0)
                                    KernelMemory::CopyBytes(Bytes + OldSize, IsInternalSource ? Bytes + SourceOffset : Source, ByteCount);
                            }

                            template <typename T>
                            T* Allocate(KernelTypes::Size ElementCount = 1)
                            {
                                static_assert(alignof(T) <= KernelCompiler::CacheLineSize, "ByteBuffer supports types aligned up to cache line size");
                                if (ElementCount == 0)
                                    return nullptr;
                                KernelAssert::Check(ElementCount <= KernelTypes::MaxValue<KernelTypes::Size>() / sizeof(T), "ByteBuffer::Allocate: размер переполнен");
                                Align(alignof(T));
                                const KernelTypes::Size Offset = Count;
                                const KernelTypes::Size ByteCount = ElementCount * sizeof(T);
                                KernelAssert::Check(ByteCount <= KernelTypes::MaxValue<KernelTypes::Size>() - Count, "ByteBuffer::Allocate: размер переполнен");
                                Resize(Count + ByteCount);
                                T* Result = reinterpret_cast<T*>(Bytes + Offset);
                                for (KernelTypes::Size Index = 0; Index < ElementCount; ++Index)
                                    KernelTypes::ConstructAt(Result + Index);
                                return Result;
                            }

                            template <typename T>
                            [[nodiscard]] const T* Get(KernelTypes::Size ByteOffset) const
                            {
                                KernelAssert::DebugCheck(ByteOffset < Count, "ByteBuffer::Get: смещение вне буфера");
                                return reinterpret_cast<const T*>(Bytes + ByteOffset);
                            }

                            template <typename T>
                            [[nodiscard]] T* Get(KernelTypes::Size ByteOffset)
                            {
                                KernelAssert::DebugCheck(ByteOffset < Count, "ByteBuffer::Get: смещение вне буфера");
                                return reinterpret_cast<T*>(Bytes + ByteOffset);
                            }
                    };
                }
            }
        }
    }
}
