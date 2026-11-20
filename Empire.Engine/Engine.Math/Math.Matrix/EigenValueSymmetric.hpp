#pragma once

// Jacobi eigenvalue solver ported from Jolt Physics 5.7.0 (Math/EigenValueSymmetric.h).
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
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

                    template <typename VectorType, typename MatrixType>
                    [[nodiscard]] inline KernelTypes::Bool EigenValueSymmetric(
                        const MatrixType& Input,
                        MatrixType& EigenVectors,
                        VectorType& EigenValues
                    )
                    {
                        KernelCompiler::FPExceptionDisableInvalid DisableInvalid;
                        constexpr KernelTypes::Int32 MaximumSweeps = 50;
                        const KernelTypes::UInt32 Dimension = Input.GetRows();
                        KernelAssert::DebugCheck(Input.GetCols() == Dimension, "Eigenvalue input matrix is square");
                        KernelAssert::DebugCheck(EigenVectors.GetRows() == Dimension && EigenVectors.GetCols() == Dimension,
                            "Eigenvector matrix dimensions match");
                        KernelAssert::DebugCheck(EigenValues.GetRows() == Dimension, "Eigenvalue vector dimension matches");
                        KernelAssert::DebugCheck(EigenVectors.IsIdentity(), "Eigenvector matrix starts as identity");

                        MatrixType A = Input;
                        VectorType B;
                        VectorType Z;
                        for (KernelTypes::UInt32 Index = 0; Index < Dimension; ++Index)
                        {
                            B[Index] = A(Index, Index);
                            EigenValues[Index] = A(Index, Index);
                            Z[Index] = 0.0f;
                        }

                        const auto Rotate = [](MatrixType& Matrix, KernelTypes::UInt32 I, KernelTypes::UInt32 J,
                            KernelTypes::UInt32 K, KernelTypes::UInt32 L, KernelTypes::Float S, KernelTypes::Float Tau)
                        {
                            const KernelTypes::Float G = Matrix(I, J);
                            const KernelTypes::Float H = Matrix(K, L);
                            Matrix(I, J) = G - S * (H + G * Tau);
                            Matrix(K, L) = H + S * (G - H * Tau);
                        };

                        for (KernelTypes::Int32 Sweep = 0; Sweep < MaximumSweeps; ++Sweep)
                        {
                            KernelTypes::Float Sum = 0.0f;
                            for (KernelTypes::UInt32 P = 0; P + 1 < Dimension; ++P)
                                for (KernelTypes::UInt32 Q = P + 1; Q < Dimension; ++Q)
                                    Sum += Abs(A(P, Q));
                            const KernelTypes::Float AverageSum = Sum / Square(static_cast<KernelTypes::Float>(Dimension));

                            if (AverageSum < 1.17549435e-38f)
                            {
                                if constexpr (KernelCompiler::IsDebugBuild)
                                    for (KernelTypes::UInt32 Column = 0; Column < Dimension; ++Column)
                                    {
                                        KernelAssert::DebugCheck(EigenVectors.GetColumn(Column).IsNormalized(),
                                            "Eigenvector is normalized");
                                        const VectorType MatrixEigenVector = Input * EigenVectors.GetColumn(Column);
                                        const VectorType EigenValueEigenVector = EigenValues[Column] * EigenVectors.GetColumn(Column);
                                        KernelAssert::DebugCheck(
                                            MatrixEigenVector.IsClose(
                                                EigenValueEigenVector,
                                                Max(MatrixEigenVector.LengthSq(), EigenValueEigenVector.LengthSq()) * 1.0e-6f
                                            ),
                                            "Eigenpair satisfies the input matrix"
                                        );
                                    }
                                return true;
                            }

                            const KernelTypes::Float Threshold = Sweep < 4 ? 0.2f * AverageSum : 1.17549435e-38f;
                            for (KernelTypes::UInt32 P = 0; P + 1 < Dimension; ++P)
                                for (KernelTypes::UInt32 Q = P + 1; Q < Dimension; ++Q)
                                {
                                    KernelTypes::Float& APQ = A(P, Q);
                                    KernelTypes::Float& EigenValueP = EigenValues[P];
                                    KernelTypes::Float& EigenValueQ = EigenValues[Q];
                                    const KernelTypes::Float AbsoluteAPQ = Abs(APQ);
                                    const KernelTypes::Float G = 100.0f * AbsoluteAPQ;

                                    if (Sweep > 4
                                        && Abs(EigenValueP) + G == Abs(EigenValueP)
                                        && Abs(EigenValueQ) + G == Abs(EigenValueQ))
                                    {
                                        APQ = 0.0f;
                                    }
                                    else if (AbsoluteAPQ > Threshold)
                                    {
                                        KernelTypes::Float H = EigenValueQ - EigenValueP;
                                        const KernelTypes::Float AbsoluteH = Abs(H);
                                        KernelTypes::Float T;
                                        if (AbsoluteH + G == AbsoluteH)
                                        {
                                            T = APQ / H;
                                        }
                                        else
                                        {
                                            const KernelTypes::Float Theta = 0.5f * H / APQ;
                                            T = 1.0f / (Abs(Theta) + Sqrt(1.0f + Theta * Theta));
                                            if (Theta < 0.0f)
                                                T = -T;
                                        }

                                        const KernelTypes::Float C = 1.0f / Sqrt(1.0f + T * T);
                                        const KernelTypes::Float S = T * C;
                                        const KernelTypes::Float Tau = S / (1.0f + C);
                                        H = T * APQ;
                                        APQ = 0.0f;
                                        Z[P] -= H;
                                        Z[Q] += H;
                                        EigenValueP -= H;
                                        EigenValueQ += H;

                                        for (KernelTypes::UInt32 J = 0; J < P; ++J)
                                            Rotate(A, J, P, J, Q, S, Tau);
                                        for (KernelTypes::UInt32 J = P + 1; J < Q; ++J)
                                            Rotate(A, P, J, J, Q, S, Tau);
                                        for (KernelTypes::UInt32 J = Q + 1; J < Dimension; ++J)
                                            Rotate(A, P, J, Q, J, S, Tau);
                                        for (KernelTypes::UInt32 J = 0; J < Dimension; ++J)
                                            Rotate(EigenVectors, J, P, J, Q, S, Tau);
                                    }
                                }

                            for (KernelTypes::UInt32 P = 0; P < Dimension; ++P)
                            {
                                B[P] += Z[P];
                                EigenValues[P] = B[P];
                                Z[P] = 0.0f;
                            }
                        }

                        KernelAssert::DebugCheck(false, "Jacobi eigenvalue solver converged within 50 sweeps");
                        return false;
                    }
                }
            }
        }
    }
}
