#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelContainers
                {
                    // Указатель на элементы с шагом в байтах (перенесён из Jolt 5.7.0, Core/StridedPtr.h).
                    template <typename T>
                    class StridedPtr
                    {
                        private:
                            KernelTypes::UInt8* Pointer = nullptr;
                            KernelTypes::SignedSize Stride = 0;

                        public:
                            using ValueType = T;

                            constexpr StridedPtr() noexcept = default;
                            constexpr StridedPtr(const StridedPtr&) noexcept = default;
                            constexpr StridedPtr& operator=(const StridedPtr&) noexcept = default;

                            StridedPtr(T* Value, KernelTypes::SignedSize ByteStride = static_cast<KernelTypes::SignedSize>(sizeof(T))) noexcept
                                : Pointer(const_cast<KernelTypes::UInt8*>(reinterpret_cast<const KernelTypes::UInt8*>(Value))), Stride(ByteStride)
                            {
                            }

                            StridedPtr& operator++() noexcept { Pointer += Stride; return *this; }
                            StridedPtr& operator--() noexcept { Pointer -= Stride; return *this; }
                            StridedPtr operator++(int) noexcept { StridedPtr Previous(*this); ++(*this); return Previous; }
                            StridedPtr operator--(int) noexcept { StridedPtr Previous(*this); --(*this); return Previous; }
                            StridedPtr operator+(KernelTypes::SignedSize Offset) const noexcept { StridedPtr Copy(*this); Copy += Offset; return Copy; }
                            StridedPtr operator-(KernelTypes::SignedSize Offset) const noexcept { StridedPtr Copy(*this); Copy -= Offset; return Copy; }
                            StridedPtr& operator+=(KernelTypes::SignedSize Offset) noexcept { Pointer += Offset * Stride; return *this; }
                            StridedPtr& operator-=(KernelTypes::SignedSize Offset) noexcept { Pointer -= Offset * Stride; return *this; }

                            KernelTypes::SignedSize operator-(const StridedPtr& Other) const noexcept
                            {
                                KernelAssert::DebugCheck(Other.Stride == Stride, "StridedPtr: нельзя вычитать указатели с разным шагом");
                                return (Pointer - Other.Pointer) / Stride;
                            }

                            [[nodiscard]] KernelTypes::Bool operator==(const StridedPtr& Other) const noexcept { return Pointer == Other.Pointer; }
                            [[nodiscard]] KernelTypes::Bool operator!=(const StridedPtr& Other) const noexcept { return Pointer != Other.Pointer; }
                            [[nodiscard]] KernelTypes::Bool operator<(const StridedPtr& Other) const noexcept { return Pointer < Other.Pointer; }
                            [[nodiscard]] KernelTypes::Bool operator<=(const StridedPtr& Other) const noexcept { return Pointer <= Other.Pointer; }
                            [[nodiscard]] KernelTypes::Bool operator>(const StridedPtr& Other) const noexcept { return Pointer > Other.Pointer; }
                            [[nodiscard]] KernelTypes::Bool operator>=(const StridedPtr& Other) const noexcept { return Pointer >= Other.Pointer; }

                            T& operator*() const noexcept { return *reinterpret_cast<T*>(Pointer); }
                            T* operator->() const noexcept { return reinterpret_cast<T*>(Pointer); }
                            T& operator[](KernelTypes::SignedSize Offset) const noexcept { return *reinterpret_cast<T*>(Pointer + Offset * Stride); }
                            [[nodiscard]] T* GetPointer() const noexcept { return reinterpret_cast<T*>(Pointer); }
                            [[nodiscard]] T* GetPtr() const noexcept { return GetPointer(); }
                            [[nodiscard]] KernelTypes::SignedSize GetStride() const noexcept { return Stride; }
                    };
                }
            }
        }
    }
}
