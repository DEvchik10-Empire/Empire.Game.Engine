-- Приватный бэкенд Platform.Clipboard: Xcb. Наружу не виден.
target("Platform.Clipboard")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("xcb")
