-- Приватный бэкенд Platform.Dialog: Xcb. Наружу не виден.
target("Platform.Dialog")
    add_files("*.cpp")
    add_syslinks("xcb")
