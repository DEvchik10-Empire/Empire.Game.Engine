#pragma once

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelAlgorithms
            {
                // Сравнение по умолчанию для сортировок и кучи: Left идёт раньше Right.
                struct Less
                {
                    template <typename Left, typename Right>
                    constexpr bool operator()(const Left& LeftValue, const Right& RightValue) const
                    {
                        return LeftValue < RightValue;
                    }
                };

                struct Greater
                {
                    template <typename Left, typename Right>
                    constexpr bool operator()(const Left& LeftValue, const Right& RightValue) const
                    {
                        return LeftValue > RightValue;
                    }
                };
            }
        }
    }
}
