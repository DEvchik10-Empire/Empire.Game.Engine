-- Приватный бэкенд Platform.App: XLib. Наружу не виден.
target("Platform.App")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("X11")
