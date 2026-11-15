target("Math.Simd")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert")

includes("Simd.Scalar")
if is_arch("x86_64", "i386") then includes("Simd.SSE") end
if is_arch("x86_64", "i386") then includes("Simd.AVX") end
if is_arch("arm64", "arm64-v8a", "arm.*") then includes("Simd.NEON") end
