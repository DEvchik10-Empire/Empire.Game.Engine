#pragma once

#include "Empire.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Memory
            {
                class ControlBlock
                {
                    private:
                        Sync::Atomic<Types::Size> StrongCount{1};

                    public:
                        ControlBlock() = default;
                        ControlBlock(const ControlBlock&) = delete;
                        ControlBlock& operator=(const ControlBlock&) = delete;
                        virtual ~ControlBlock() = default;

                        virtual void DestroyObject() noexcept = 0;
                        virtual void Free() noexcept = 0;

                        void AddReference() noexcept
                        {
                            StrongCount.FetchAdd(1, Sync::MemoryOrder::Relaxed);
                        }

                        void Release() noexcept
                        {
                            if (StrongCount.FetchSub(1, Sync::MemoryOrder::AcquireRelease) == 1)
                            {
                                DestroyObject();
                                Free();
                            }
                        }

                        Types::Size GetCount() const noexcept
                        {
                            return StrongCount.Load(Sync::MemoryOrder::Relaxed);
                        }
                };

                template <typename T>
                class InlineControlBlock final : public ControlBlock
                {
                    private:
                        alignas(T) Types::UInt8 Storage[sizeof(T)];
                        T* Object = nullptr;

                    public:
                        template <typename... Arguments>
                        explicit InlineControlBlock(Arguments&&... Args)
                        {
                            Object = Types::ConstructAt(reinterpret_cast<T*>(Storage), Types::Forward<Arguments>(Args)...);
                        }

                        T* GetObject() noexcept
                        {
                            return Object;
                        }

                        void DestroyObject() noexcept override
                        {
                            Types::DestroyAt(Object);
                        }

                        void Free() noexcept override
                        {
                            this->~InlineControlBlock();
                            Memory::Deallocate(this);
                        }
                };

                template <typename T>
                class SharedPtr;

                template <typename T, typename... Arguments>
                SharedPtr<T> MakeShared(Arguments&&... Args);

                template <typename T, typename U>
                SharedPtr<T> StaticPointerCast(const SharedPtr<U>& Source) noexcept;

                template <typename T>
                class SharedPtr
                {
                    private:
                        T* Pointer = nullptr;
                        ControlBlock* Block = nullptr;

                        template <typename U>
                        friend class SharedPtr;

                        template <typename U, typename... Arguments>
                        friend SharedPtr<U> MakeShared(Arguments&&... Args);

                        template <typename U, typename V>
                        friend SharedPtr<U> StaticPointerCast(const SharedPtr<V>& Source) noexcept;

                        void Release() noexcept
                        {
                            if (Block != nullptr)
                                Block->Release();

                            Pointer = nullptr;
                            Block = nullptr;
                        }

                    public:
                        SharedPtr() noexcept = default;
                        SharedPtr(Types::NullPointer) noexcept {}

                        SharedPtr(const SharedPtr& Other) noexcept
                            : Pointer(Other.Pointer), Block(Other.Block)
                        {
                            if (Block != nullptr)
                                Block->AddReference();
                        }

                        SharedPtr(SharedPtr&& Other) noexcept
                            : Pointer(Other.Pointer), Block(Other.Block)
                        {
                            Other.Pointer = nullptr;
                            Other.Block = nullptr;
                        }

                        template <typename U>
                            requires requires(U* Source, T*& Target) { Target = Source; }
                        SharedPtr(const SharedPtr<U>& Other) noexcept
                            : Pointer(Other.Pointer), Block(Other.Block)
                        {
                            if (Block != nullptr)
                                Block->AddReference();
                        }

                        template <typename U>
                            requires requires(U* Source, T*& Target) { Target = Source; }
                        SharedPtr(SharedPtr<U>&& Other) noexcept
                            : Pointer(Other.Pointer), Block(Other.Block)
                        {
                            Other.Pointer = nullptr;
                            Other.Block = nullptr;
                        }

                        ~SharedPtr()
                        {
                            Release();
                        }

                        SharedPtr& operator=(const SharedPtr& Other) noexcept
                        {
                            if (this == &Other)
                                return *this;

                            SharedPtr Copy(Other);
                            Types::Swap(Pointer, Copy.Pointer);
                            Types::Swap(Block, Copy.Block);
                            return *this;
                        }

                        SharedPtr& operator=(SharedPtr&& Other) noexcept
                        {
                            if (this == &Other)
                                return *this;

                            Release();
                            Pointer = Other.Pointer;
                            Block = Other.Block;
                            Other.Pointer = nullptr;
                            Other.Block = nullptr;
                            return *this;
                        }

                        void Reset() noexcept
                        {
                            Release();
                        }

                        T* Get() const noexcept
                        {
                            return Pointer;
                        }

                        T* operator->() const noexcept
                        {
                            return Pointer;
                        }

                        auto& operator*() const noexcept
                            requires (!Types::IsVoid<T>)
                        {
                            return *Pointer;
                        }

                        explicit operator Types::Bool() const noexcept
                        {
                            return Pointer != nullptr;
                        }

                        Types::Size GetUseCount() const noexcept
                        {
                            return Block == nullptr ? 0 : Block->GetCount();
                        }
                };

                template <typename T>
                Types::Bool operator==(const SharedPtr<T>& Left, Types::NullPointer) noexcept
                {
                    return Left.Get() == nullptr;
                }

                template <typename T>
                Types::Bool operator!=(const SharedPtr<T>& Left, Types::NullPointer) noexcept
                {
                    return Left.Get() != nullptr;
                }

                template <typename T, typename U>
                Types::Bool operator==(const SharedPtr<T>& Left, const SharedPtr<U>& Right) noexcept
                {
                    return Left.Get() == Right.Get();
                }

                template <typename T, typename U>
                Types::Bool operator!=(const SharedPtr<T>& Left, const SharedPtr<U>& Right) noexcept
                {
                    return Left.Get() != Right.Get();
                }

                template <typename T, typename... Arguments>
                SharedPtr<T> MakeShared(Arguments&&... Args)
                {
                    using Block = InlineControlBlock<T>;

                    void* Storage = Memory::Allocate(sizeof(Block), alignof(Block));
                    Block* NewBlock = nullptr;

                    try
                    {
                        NewBlock = Types::ConstructAt(static_cast<Block*>(Storage), Types::Forward<Arguments>(Args)...);
                    }
                    catch (...)
                    {
                        Memory::Deallocate(Storage);
                        throw;
                    }

                    SharedPtr<T> Result;
                    Result.Pointer = NewBlock->GetObject();
                    Result.Block = NewBlock;
                    return Result;
                }

                template <typename T, typename U>
                SharedPtr<T> StaticPointerCast(const SharedPtr<U>& Source) noexcept
                {
                    SharedPtr<T> Result;

                    if (Source.Block == nullptr)
                        return Result;

                    Result.Pointer = static_cast<T*>(Source.Pointer);
                    Result.Block = Source.Block;
                    Result.Block->AddReference();
                    return Result;
                }
            }
        }
    }
}
