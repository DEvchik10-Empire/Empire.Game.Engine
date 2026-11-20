#pragma once

// Fixed-size matrix ported from Jolt Physics 5.7.0 (Math/Matrix.h).
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/GaussianElimination.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Vector.hpp"

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

                    template <KernelTypes::UInt32 Rows, KernelTypes::UInt32 Columns>
                    class [[nodiscard]] Matrix
                    {
                    public:
                        constexpr Matrix() noexcept = default;
                        constexpr Matrix(const Matrix& Other) noexcept { *this = Other; }

                        [[nodiscard]] static constexpr KernelTypes::UInt32 GetRows() noexcept { return Rows; }
                        [[nodiscard]] static constexpr KernelTypes::UInt32 GetCols() noexcept { return Columns; }

                        constexpr void SetZero() noexcept
                        {
                            for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                mColumns[Column].SetZero();
                        }

                        [[nodiscard]] static constexpr Matrix sZero() noexcept
                        {
                            Matrix Result;
                            Result.SetZero();
                            return Result;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool IsZero() const noexcept
                        {
                            for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                if (!mColumns[Column].IsZero())
                                    return false;
                            return true;
                        }

                        constexpr void SetIdentity() noexcept
                        {
                            SetZero();
                            constexpr KernelTypes::UInt32 DiagonalSize = Rows < Columns ? Rows : Columns;
                            for (KernelTypes::UInt32 Index = 0; Index < DiagonalSize; ++Index)
                                mColumns[Index][Index] = 1.0f;
                        }

                        [[nodiscard]] static constexpr Matrix sIdentity() noexcept
                        {
                            Matrix Result;
                            Result.SetIdentity();
                            return Result;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool IsIdentity() const noexcept { return *this == sIdentity(); }

                        constexpr void SetDiagonal(const Vector<(Rows < Columns ? Rows : Columns)>& Diagonal) noexcept
                        {
                            SetZero();
                            constexpr KernelTypes::UInt32 DiagonalSize = Rows < Columns ? Rows : Columns;
                            for (KernelTypes::UInt32 Index = 0; Index < DiagonalSize; ++Index)
                                mColumns[Index][Index] = Diagonal[Index];
                        }

                        [[nodiscard]] static constexpr Matrix sDiagonal(
                            const Vector<(Rows < Columns ? Rows : Columns)>& Diagonal
                        ) noexcept
                        {
                            Matrix Result;
                            Result.SetDiagonal(Diagonal);
                            return Result;
                        }

                        template <typename OtherMatrix>
                        constexpr void CopyPart(
                            const OtherMatrix& Source,
                            KernelTypes::UInt32 SourceRow,
                            KernelTypes::UInt32 SourceColumn,
                            KernelTypes::UInt32 RowCount,
                            KernelTypes::UInt32 ColumnCount,
                            KernelTypes::UInt32 DestinationRow,
                            KernelTypes::UInt32 DestinationColumn
                        ) noexcept
                        {
                            for (KernelTypes::UInt32 Column = 0; Column < ColumnCount; ++Column)
                                for (KernelTypes::UInt32 Row = 0; Row < RowCount; ++Row)
                                    mColumns[DestinationColumn + Column][DestinationRow + Row]
                                        = Source(SourceRow + Row, SourceColumn + Column);
                        }

                        [[nodiscard]] constexpr KernelTypes::Float operator()(
                            KernelTypes::UInt32 Row,
                            KernelTypes::UInt32 Column
                        ) const noexcept
                        {
                            KernelAssert::DebugCheck(Row < Rows && Column < Columns, "Matrix indices are in range");
                            return mColumns[Column][Row];
                        }

                        constexpr KernelTypes::Float& operator()(
                            KernelTypes::UInt32 Row,
                            KernelTypes::UInt32 Column
                        ) noexcept
                        {
                            KernelAssert::DebugCheck(Row < Rows && Column < Columns, "Matrix indices are in range");
                            return mColumns[Column][Row];
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const Matrix& Other) const noexcept
                        {
                            for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                if (mColumns[Column] != Other.mColumns[Column])
                                    return false;
                            return true;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const Matrix& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        constexpr Matrix& operator=(const Matrix& Other) noexcept
                        {
                            for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                mColumns[Column] = Other.mColumns[Column];
                            return *this;
                        }

                        template <KernelTypes::UInt32 OtherColumns>
                        [[nodiscard]] constexpr Matrix<Rows, OtherColumns> operator*(
                            const Matrix<Columns, OtherColumns>& Other
                        ) const noexcept
                        {
                            Matrix<Rows, OtherColumns> Result;
                            for (KernelTypes::UInt32 Column = 0; Column < OtherColumns; ++Column)
                                for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                {
                                    KernelTypes::Float Dot = 0.0f;
                                    for (KernelTypes::UInt32 Index = 0; Index < Columns; ++Index)
                                        Dot += mColumns[Index][Row] * Other.mColumns[Column][Index];
                                    Result.mColumns[Column][Row] = Dot;
                                }
                            return Result;
                        }

                        [[nodiscard]] constexpr Vector<Rows> operator*(const Vector<Columns>& Value) const noexcept
                        {
                            Vector<Rows> Result;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                            {
                                KernelTypes::Float Dot = 0.0f;
                                for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                    Dot += mColumns[Column][Row] * Value[Column];
                                Result[Row] = Dot;
                            }
                            return Result;
                        }

                        [[nodiscard]] constexpr Matrix operator*(KernelTypes::Float Scale) const noexcept
                        {
                            Matrix Result;
                            for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                Result.mColumns[Column] = mColumns[Column] * Scale;
                            return Result;
                        }

                        [[nodiscard]] constexpr Matrix operator+(const Matrix& Other) const noexcept
                        {
                            Matrix Result;
                            for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                Result.mColumns[Column] = mColumns[Column] + Other.mColumns[Column];
                            return Result;
                        }

                        [[nodiscard]] constexpr Matrix operator-(const Matrix& Other) const noexcept
                        {
                            Matrix Result;
                            for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                Result.mColumns[Column] = mColumns[Column] - Other.mColumns[Column];
                            return Result;
                        }

                        [[nodiscard]] constexpr Matrix<Columns, Rows> Transposed() const noexcept
                        {
                            Matrix<Columns, Rows> Result;
                            for (KernelTypes::UInt32 Row = 0; Row < Rows; ++Row)
                                for (KernelTypes::UInt32 Column = 0; Column < Columns; ++Column)
                                    Result.mColumns[Row][Column] = mColumns[Column][Row];
                            return Result;
                        }

                        [[nodiscard]] KernelTypes::Bool SetInversed(const Matrix& Value) noexcept
                        {
                            if constexpr (Rows != Columns)
                                KernelAssert::DebugCheck(false, "Only square matrices can be inverted");
                            Matrix Copy(Value);
                            SetIdentity();
                            return GaussianElimination(Copy, *this);
                        }

                        [[nodiscard]] Matrix Inversed() const noexcept
                        {
                            Matrix Result;
                            Result.SetInversed(*this);
                            return Result;
                        }

                        [[nodiscard]] constexpr const Vector<Rows>& GetColumn(KernelTypes::UInt32 Index) const noexcept
                        {
                            KernelAssert::DebugCheck(Index < Columns, "Matrix column index is in range");
                            return mColumns[Index];
                        }

                        constexpr Vector<Rows>& GetColumn(KernelTypes::UInt32 Index) noexcept
                        {
                            KernelAssert::DebugCheck(Index < Columns, "Matrix column index is in range");
                            return mColumns[Index];
                        }

                        Vector<Rows> mColumns[Columns];
                    };

                    template <KernelTypes::UInt32 Rows, KernelTypes::UInt32 Columns>
                    [[nodiscard]] constexpr Matrix<Rows, Columns> operator*(
                        KernelTypes::Float Scale,
                        const Matrix<Rows, Columns>& Value
                    ) noexcept
                    {
                        return Value * Scale;
                    }

                    template <>
                    inline KernelTypes::Bool Matrix<2, 2>::SetInversed(const Matrix<2, 2>& Value) noexcept
                    {
                        const KernelTypes::Float A = Value.mColumns[0][0];
                        const KernelTypes::Float B = Value.mColumns[1][0];
                        const KernelTypes::Float C = Value.mColumns[0][1];
                        const KernelTypes::Float D = Value.mColumns[1][1];
                        const KernelTypes::Float Determinant = DifferenceOfProducts(A, D, B, C);
                        if (Determinant == 0.0f)
                            return false;
                        mColumns[0][0] = D / Determinant;
                        mColumns[1][0] = -B / Determinant;
                        mColumns[0][1] = -C / Determinant;
                        mColumns[1][1] = A / Determinant;
                        return true;
                    }
                }
            }
        }
    }
}
