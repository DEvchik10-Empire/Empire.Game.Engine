#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Algorithms/Algorithms.QuickSort.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"

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
                    // Набор точек с линейной интерполяцией (перенесён из Jolt 5.7.0, Core/LinearCurve.h/.cpp).
                    class LinearCurve
                    {
                        public:
                            struct Point
                            {
                                KernelTypes::Float X = 0.0f;
                                KernelTypes::Float Y = 0.0f;
                            };

                            using Points = Array<Point>;

                            void Clear() noexcept { CurvePoints.Clear(); }
                            void Reserve(KernelTypes::Size PointCount) { CurvePoints.Reserve(PointCount); }
                            void AddPoint(KernelTypes::Float X, KernelTypes::Float Y) { CurvePoints.EmplaceBack(Point{X, Y}); }

                            void Sort()
                            {
                                if (CurvePoints.GetCount() < 2)
                                    return;

                                KernelAlgorithms::QuickSort(
                                    CurvePoints.begin(),
                                    CurvePoints.end(),
                                    [](const Point& Left, const Point& Right) { return Left.X < Right.X; }
                                );
                            }

                            [[nodiscard]] KernelTypes::Float GetMinX() const noexcept
                            {
                                return CurvePoints.IsEmpty() ? 0.0f : CurvePoints.Front().X;
                            }

                            [[nodiscard]] KernelTypes::Float GetMaxX() const noexcept
                            {
                                return CurvePoints.IsEmpty() ? 0.0f : CurvePoints.Back().X;
                            }

                            [[nodiscard]] KernelTypes::Float GetValue(KernelTypes::Float X) const
                            {
                                if (CurvePoints.IsEmpty())
                                    return 0.0f;

                                // lower_bound semantics from Jolt: first point whose X is not less than the sample.
                                KernelTypes::Size First = 0;
                                KernelTypes::Size Last = CurvePoints.GetCount();
                                while (First < Last)
                                {
                                    const KernelTypes::Size Middle = First + (Last - First) / 2;
                                    if (CurvePoints[Middle].X < X)
                                        First = Middle + 1;
                                    else
                                        Last = Middle;
                                }
                                const KernelTypes::Size Right = First;

                                if (Right == 0)
                                    return CurvePoints.Front().Y;
                                if (Right == CurvePoints.GetCount())
                                    return CurvePoints.Back().Y;

                                const Point& LeftPoint = CurvePoints[Right - 1];
                                const Point& RightPoint = CurvePoints[Right];
                                return LeftPoint.Y + (X - LeftPoint.X) * (RightPoint.Y - LeftPoint.Y) / (RightPoint.X - LeftPoint.X);
                            }

                            template <typename Stream>
                            void SaveBinaryState(Stream& Output) const
                            {
                                Output.Write(CurvePoints);
                            }

                            template <typename Stream>
                            void RestoreBinaryState(Stream& Input)
                            {
                                Input.Read(CurvePoints);
                            }

                            [[nodiscard]] const Points& GetPoints() const noexcept { return CurvePoints; }

                        private:
                            Points CurvePoints;
                    };
                }
            }
        }
    }
}
