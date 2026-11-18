-- Приватный бэкенд Platform.Cursor: Xcb. Наружу не виден.
target("Platform.Cursor")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("xcb")
