-- Приватный бэкенд Platform.Surface: Xcb. Наружу не виден.
target("Platform.Surface")
    add_deps("Platform.App", "Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("xcb")
