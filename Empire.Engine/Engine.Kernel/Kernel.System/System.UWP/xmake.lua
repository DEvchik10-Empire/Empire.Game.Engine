-- Приватный бэкенд Kernel.System: UWP. Наружу не виден.
target("Kernel.System")
    add_files("*.cpp")
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN")
