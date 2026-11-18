-- Приватный бэкенд Platform.App: Wayland. Наружу не виден.
target("Platform.App")
    add_deps("Kernel.Containers", "Kernel.Memory", "Platform.ThirdParty", "Platform.ThirdParty.Wayland")
    add_files("*.cpp")
    add_syslinks("wayland-client")
