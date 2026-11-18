-- Приватный бэкенд Platform.App: Xcb. Наружу не виден.
target("Platform.App")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("xcb")
