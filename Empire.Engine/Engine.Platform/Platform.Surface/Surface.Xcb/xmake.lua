-- Приватный бэкенд Platform.Surface: Xcb. Наружу не виден.
target("Platform.Surface")
    add_files("*.cpp")
    add_syslinks("xcb")
