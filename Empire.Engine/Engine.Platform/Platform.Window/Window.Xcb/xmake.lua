-- Приватный бэкенд Platform.Window: Xcb. Наружу не виден.
target("Platform.Window")
    add_files("*.cpp")
    add_syslinks("xcb")
