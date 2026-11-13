-- Приватный бэкенд Kernel.System: Windows (Win32). Наружу не виден.
target("Kernel.System")
    add_files("*.cpp")
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN")
