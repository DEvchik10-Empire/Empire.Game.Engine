#pragma once

// Gauss-Jordan solver ported from Jolt Physics 5.7.0 (Math/GaussianElimination.h).
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

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
                    using namespace MathScalar;

                    template <typename MatrixA, typename MatrixB>
                    [[nodiscard]] inline KernelTypes::Bool GaussianElimination(
                        MatrixA& Matrix,
                        MatrixB& Result,
                        KernelTypes::Float Tolerance = 1.0e-16f
                    )
                    {
                        const KernelTypes::UInt32 Size = Matrix.GetCols();
                        const KernelTypes::UInt32 ResultColumns = Result.GetCols();
                        KernelAssert::DebugCheck(Matrix.GetRows() == Size, "Gaussian elimination input is square");
                        KernelAssert::DebugCheck(Result.GetRows() == Size, "Gaussian elimination dimensions match");

                        KernelContainers::Array<KernelTypes::UInt32> PivotCounts(Size, 0u);
                        for (KernelTypes::UInt32 Index = 0; Index < Size; ++Index)
                        {
                            KernelTypes::UInt32 PivotRow = Index;
                            KernelTypes::UInt32 PivotColumn = Index;
                            KernelTypes::Float LargestElement = 0.0f;
                            for (KernelTypes::UInt32 Row = 0; Row < Size; ++Row)
                                if (PivotCounts[Row] != 1)
                                    for (KernelTypes::UInt32 Column = 0; Column < Size; ++Column)
                                    {
                                        if (PivotCounts[Column] == 0)
                                        {
                                            const KernelTypes::Float Element = Abs(Matrix(Row, Column));
                                            if (Element >= LargestElement)
                                            {
                                                LargestElement = Element;
                                                PivotRow = Row;
                                                PivotColumn = Column;
                                            }
                                        }
                                        else if (PivotCounts[Column] > 1)
                                        {
                                            return false;
                                        }
                                    }

                            ++PivotCounts[PivotColumn];
                            if (PivotRow != PivotColumn)
                            {
                                for (KernelTypes::UInt32 Column = 0; Column < Size; ++Column)
                                {
                                    const KernelTypes::Float Temporary = Matrix(PivotRow, Column);
                                    Matrix(PivotRow, Column) = Matrix(PivotColumn, Column);
                                    Matrix(PivotColumn, Column) = Temporary;
                                }
                                for (KernelTypes::UInt32 Column = 0; Column < ResultColumns; ++Column)
                                {
                                    const KernelTypes::Float Temporary = Result(PivotRow, Column);
                                    Result(PivotRow, Column) = Result(PivotColumn, Column);
                                    Result(PivotColumn, Column) = Temporary;
                                }
                            }

                            const KernelTypes::Float Diagonal = Matrix(PivotColumn, PivotColumn);
                            if (Abs(Diagonal) < Tolerance)
                                return false;

                            for (KernelTypes::UInt32 Column = 0; Column < Size; ++Column)
                                Matrix(PivotColumn, Column) /= Diagonal;
                            for (KernelTypes::UInt32 Column = 0; Column < ResultColumns; ++Column)
                                Result(PivotColumn, Column) /= Diagonal;
                            Matrix(PivotColumn, PivotColumn) = 1.0f;

                            for (KernelTypes::UInt32 Row = 0; Row < Size; ++Row)
                                if (Row != PivotColumn)
                                {
                                    const KernelTypes::Float Element = Matrix(Row, PivotColumn);
                                    for (KernelTypes::UInt32 Column = 0; Column < Size; ++Column)
                                        Matrix(Row, Column) -= Matrix(PivotColumn, Column) * Element;
                                    for (KernelTypes::UInt32 Column = 0; Column < ResultColumns; ++Column)
                                        Result(Row, Column) -= Result(PivotColumn, Column) * Element;
                                    Matrix(Row, PivotColumn) = 0.0f;
                                }
                        }
                        return true;
                    }
                }
            }
        }
    }
}
