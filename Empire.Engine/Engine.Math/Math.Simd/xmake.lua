target("Math.Simd")
    -- файлы добавляют бэкенды, см. includes ниже

includes("Simd.Scalar")
if is_arch("x86_64", "i386") then includes("Simd.AVX") end
if is_arch("arm64", "arm64-v8a", "arm.*") then includes("Simd.NEON") end
if is_arch("x86_64", "i386") then includes("Simd.SSE") end
