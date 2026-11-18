-- Приватный бэкенд Platform.App: UWP. Наружу не виден.
target("Platform.App")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("windowsapp")
