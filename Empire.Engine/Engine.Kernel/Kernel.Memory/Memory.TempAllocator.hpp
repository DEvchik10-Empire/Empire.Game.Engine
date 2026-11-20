#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Config/Config.hpp"
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
                namespace KernelMemory
                {
                    // Аллокатор для временных выделений (перенесён из Jolt, Core/TempAllocator.h).
                    // Работает как стек: блоки нужно освобождать строго в порядке, обратном выделению.
                    // Выделять и освобождать можно из разных потоков, но порядок обеспечивают зависимости между задачами,
                    // поэтому блокировки не нужны.
                    class TempAllocator : public KernelTypes::NonCopyable
                    {
                        public:
                            virtual ~TempAllocator() = default;

                            // Выделить Size байт. Адрес выровнен по TempAllocatorAlignment.
                            virtual void* Allocate(KernelTypes::UInt32 Size) = 0;

                            // Освободить Size байт по адресу Address
                            virtual void Free(void* Address, KernelTypes::UInt32 Size) = 0;

                            // Выравнивание всех блоков
                            static constexpr KernelTypes::Size TempAllocatorAlignment = KernelConfig::WorldUsesDoublePrecision
                                ? KernelCompiler::DoubleVectorAlignment
                                : KernelCompiler::VectorAlignment;

                            // Совместимый по смыслу признак Jolt needs_aligned_allocate: здесь общий allocator
                            // умеет принимать явное выравнивание, поэтому TempAllocatorImpl всегда передаёт его явно.
                            static constexpr KernelTypes::Bool NeedsAlignedAllocate = TempAllocatorAlignment > KernelMemory::DefaultAlignment;

                            static KernelTypes::Size GetAlignedSize(KernelTypes::UInt32 Size)
                            {
                                KernelAssert::Check(
                                    static_cast<KernelTypes::Size>(Size) <= KernelTypes::MaxValue<KernelTypes::Size>() - (TempAllocatorAlignment - 1),
                                    "TempAllocator: размер выделения переполняет Size"
                                );
                                return KernelTypes::AlignUp(Size, TempAllocatorAlignment);
                            }
                    };

                    // Реализация по умолчанию: один большой блок, выделенный заранее
                    class TempAllocatorImpl final : public TempAllocator
                    {
                        public:
                            // Наибольший суммарный размер выделений равен Size
                            explicit TempAllocatorImpl(KernelTypes::Size Size)
                                : BlockSize(Size)
                            {
                                Base = static_cast<KernelTypes::UInt8*>(KernelMemory::Allocate(Size, TempAllocatorAlignment));
                            }

                            ~TempAllocatorImpl() override
                            {
                                KernelAssert::DebugCheck(Top == 0, "TempAllocatorImpl: уничтожение с живыми выделениями");
                                KernelMemory::Deallocate(Base);
                            }

                            void* Allocate(KernelTypes::UInt32 Size) override
                            {
                                if (Size == 0)
                                {
                                    return nullptr;
                                }

                                const KernelTypes::Size AlignedSize = GetAlignedSize(Size);
                                KernelAssert::Check(
                                    Top <= BlockSize && AlignedSize <= BlockSize - Top,
                                    "TempAllocator: не хватает памяти"
                                );

                                void* Address = Base + Top;
                                Top += AlignedSize;
                                return Address;
                            }

                            void Free(void* Address, KernelTypes::UInt32 Size) override
                            {
                                if (Address == nullptr)
                                {
                                    KernelAssert::DebugCheck(Size == 0, "TempAllocator: нулевой адрес с ненулевым размером");
                                    return;
                                }

                                // Порядок и размер проверяются до изменения состояния; адреса сравниваются как целые,
                                // чтобы некорректный размер не приводил к pointer arithmetic за пределами блока.
                                const KernelTypes::Size AlignedSize = GetAlignedSize(Size);
                                KernelAssert::Check(AlignedSize <= Top, "TempAllocator: размер освобождения больше занятой памяти");
                                const KernelTypes::Size NewTop = Top - AlignedSize;
                                const KernelTypes::UIntPtr ExpectedAddress = reinterpret_cast<KernelTypes::UIntPtr>(Base) + NewTop;
                                KernelAssert::Check(ExpectedAddress == reinterpret_cast<KernelTypes::UIntPtr>(Address),
                                    "TempAllocator: освобождение в неправильном порядке");
                                Top = NewTop;
                            }

                            // Не было ли выделений
                            KernelTypes::Bool IsEmpty() const
                            {
                                return Top == 0;
                            }

                            // Размер буфера
                            KernelTypes::Size GetSize() const
                            {
                                return BlockSize;
                            }

                            // Сколько байт занято
                            KernelTypes::Size GetUsage() const
                            {
                                return Top;
                            }

                            // Поместится ли выделение Size байт
                            KernelTypes::Bool CanAllocate(KernelTypes::UInt32 Size) const
                            {
                                if (static_cast<KernelTypes::Size>(Size) > KernelTypes::MaxValue<KernelTypes::Size>() - (TempAllocatorAlignment - 1))
                                    return false;
                                const KernelTypes::Size AlignedSize = GetAlignedSize(Size);
                                return Top <= BlockSize && AlignedSize <= BlockSize - Top;
                            }

                            // Принадлежит ли адрес этому буферу
                            KernelTypes::Bool OwnsMemory(const void* Address) const
                            {
                                const KernelTypes::UIntPtr AddressValue = reinterpret_cast<KernelTypes::UIntPtr>(Address);
                                const KernelTypes::UIntPtr BaseValue = reinterpret_cast<KernelTypes::UIntPtr>(Base);
                                return AddressValue >= BaseValue && AddressValue - BaseValue < BlockSize;
                            }

                        private:
                            KernelTypes::UInt8* Base;       // начало блока
                            KernelTypes::Size BlockSize;    // размер блока
                            KernelTypes::Size Top = 0;      // конец занятой части
                    };

                    // Реализация, которая просто обращается к общему аллокатору
                    class TempAllocatorMalloc final : public TempAllocator
                    {
                        public:
                            void* Allocate(KernelTypes::UInt32 Size) override
                            {
                                return Size > 0 ? KernelMemory::Allocate(Size, TempAllocatorAlignment) : nullptr;
                            }

                            void Free(void* Address, KernelTypes::UInt32) override
                            {
                                if (Address != nullptr)
                                {
                                    KernelMemory::Deallocate(Address);
                                }
                            }
                    };

                    // Берёт память из большого заранее выделенного блока, а когда он кончается, переходит на общий аллокатор
                    class TempAllocatorImplWithMallocFallback final : public TempAllocator
                    {
                        public:
                            explicit TempAllocatorImplWithMallocFallback(KernelTypes::UInt32 Size)
                                : Primary(Size)
                            {
                            }

                            void* Allocate(KernelTypes::UInt32 Size) override
                            {
                                return Primary.CanAllocate(Size) ? Primary.Allocate(Size) : Fallback.Allocate(Size);
                            }

                            void Free(void* Address, KernelTypes::UInt32 Size) override
                            {
                                if (Address == nullptr)
                                {
                                    KernelAssert::DebugCheck(Size == 0, "TempAllocator: нулевой адрес с ненулевым размером");
                                    return;
                                }

                                if (Primary.OwnsMemory(Address))
                                {
                                    Primary.Free(Address, Size);
                                }
                                else
                                {
                                    Fallback.Free(Address, Size);
                                }
                            }

                        private:
                            TempAllocatorImpl Primary;
                            TempAllocatorMalloc Fallback;
                    };
                }
            }
        }
    }
}
