-- Приватный бэкенд Platform.Events: Xcb. Наружу не виден.
target("Platform.Events")
    add_files("*.cpp")
    add_syslinks("xcb")
