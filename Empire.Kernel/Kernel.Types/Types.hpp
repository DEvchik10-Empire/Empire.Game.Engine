#pragma once

#include <cstddef>
#include <cstdint>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Types
            {
                using Int8 = std::int8_t;
                using Int16 = std::int16_t;
                using Int32 = std::int32_t;
                using Int64 = std::int64_t;

                using UInt8 = std::uint8_t;
                using UInt16 = std::uint16_t;
                using UInt32 = std::uint32_t;
                using UInt64 = std::uint64_t;

                using Float = float;
                using Double = double;

                using Size = std::size_t;
                using SignedSize = std::ptrdiff_t;

                using Byte = std::byte;
                using Char = char;
                using Char16 = char16_t;
                using Char32 = char32_t;
                using Bool = bool;
            }
        }
    }
}
