#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <iostream>

namespace Engine
{
        namespace Kernel
        {
                class Types
                {
                        public:
                                // Signed integers
                                using i8  = std::int8_t;
                                using i16 = std::int16_t;
                                using i32 = std::int32_t; // вроде он является обычным int
                                using i64 = std::int64_t;
                        
                                // Unsigned integers
                                using u8  = std::uint8_t;
                                using u16 = std::uint16_t;
                                using u32 = std::uint32_t;
                                using u64 = std::uint64_t;

                                // Floating point
                                using f32 = float;
                                using f64 = double;
                        
                                using usize = std::size_t;
                                using isize = std::ptrdiff_t;
                        
                                using String = std::string;
                                using StringView = std::string_view;

                                using Byte = std::byte;
                                using CStr = const char*;
                                using VoidPtr = void*;
                        
                                using Char   = char;
                                using Char8  = char8_t;
                                using Char16 = char16_t;
                                using Char32 = char32_t;

                                inline std::istream& Cin  = std::cin;
                                inline std::ostream& Cout = std::cout;
                                inline std::ostream& Cerr = std::cerr;

                                using b8 = bool;
                                using i = int; // на всякий случай пусукай будет
                 }
       }
}