#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <iostream>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            class Types
            {
            public:
                // Signed integers
                using i8  = std::int8_t;
                using i16 = std::int16_t;
                using i32 = std::int32_t;
                using i64 = std::int64_t;

                // Unsigned integers
                using u8  = std::uint8_t;
                using u16 = std::uint16_t;
                using u32 = std::uint32_t;
                using u64 = std::uint64_t;

                // Floating point
                using f32 = float;
                using f64 = double;

                // Sizes
                using usize = std::size_t;
                using isize = std::ptrdiff_t;

                // Strings
                using String = std::string;
                using StringView = std::string_view;

                // Memory and pointers
                using Byte = std::byte;
                using CStr = const char*;
                using VoidPtr = void*;

                // Characters
                using Char   = char;
                using Char8  = char8_t;
                using Char16 = char16_t;
                using Char32 = char32_t;

                // Boolean
                using b8 = bool;

                // Integer
                using i = int;

                // Standard streams
                inline static std::istream& Cin = std::cin;
                inline static std::ostream& Cout = std::cout;
                inline static std::ostream& Cerr = std::cerr;
            };
        }
    }
}