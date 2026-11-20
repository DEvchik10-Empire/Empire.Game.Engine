#pragma once

// Resizable matrix storage ported from Jolt Physics 5.7.0 (Math/DynMatrix.h).
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathMatrix
                {
                    using namespace EngineKernel;

                    class [[nodiscard]] DynMatrix
                    {
                    public:
                        DynMatrix(const DynMatrix&) = default;
                        DynMatrix& operator=(const DynMatrix&) = default;

                        DynMatrix(KernelTypes::UInt32 Rows, KernelTypes::UInt32 Columns)
                            : mRows(Rows)
                            , mColumns(Columns)
                        {
                            const KernelTypes::UInt64 ElementCount =
                                static_cast<KernelTypes::UInt64>(Rows) * static_cast<KernelTypes::UInt64>(Columns);
                            KernelAssert::Check(
                                ElementCount <= KernelTypes::MaxValue<KernelTypes::Size>(),
                                "Dynamic matrix element count fits in Size"
                            );
                            mElements.Resize(static_cast<KernelTypes::Size>(ElementCount));
                        }

                        [[nodiscard]] KernelTypes::Float operator()(
                            KernelTypes::UInt32 Row,
                            KernelTypes::UInt32 Column
                        ) const
                        {
                            KernelAssert::DebugCheck(Row < mRows && Column < mColumns, "Dynamic matrix indices are in range");
                            return mElements[Row * mColumns + Column];
                        }

                        KernelTypes::Float& operator()(
                            KernelTypes::UInt32 Row,
                            KernelTypes::UInt32 Column
                        )
                        {
                            KernelAssert::DebugCheck(Row < mRows && Column < mColumns, "Dynamic matrix indices are in range");
                            return mElements[Row * mColumns + Column];
                        }

                        [[nodiscard]] KernelTypes::UInt32 GetCols() const noexcept { return mColumns; }
                        [[nodiscard]] KernelTypes::UInt32 GetRows() const noexcept { return mRows; }

                    private:
                        KernelTypes::UInt32 mRows;
                        KernelTypes::UInt32 mColumns;
                        KernelContainers::Array<KernelTypes::Float> mElements;
                    };
                }
            }
        }
    }
}
