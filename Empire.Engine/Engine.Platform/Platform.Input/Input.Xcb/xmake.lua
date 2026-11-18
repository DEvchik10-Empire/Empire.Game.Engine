-- Приватный бэкенд Platform.Input: Xcb. Наружу не виден.
target("Platform.Input")
    add_files("*.cpp")
    add_syslinks("xcb")
