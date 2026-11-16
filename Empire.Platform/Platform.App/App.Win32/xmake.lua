-- Приватный бэкенд Platform.App: Win32. Наружу не виден.
target("Platform.App")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("kernel32")
