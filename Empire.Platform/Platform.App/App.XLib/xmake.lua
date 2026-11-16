-- Приватный бэкенд Platform.App: XLib. Наружу не виден.
target("Platform.App")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("X11")
