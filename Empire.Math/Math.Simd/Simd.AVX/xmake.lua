-- Приватный бэкенд Math.Simd: AVX. Наружу не виден.
target("Math.Simd")
    add_files("*.cpp", {cxxflags = {"-mavx2", "-mfma"}})
