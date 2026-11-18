-- Приватный бэкенд Platform.Dialog: Xcb. Наружу не виден.
target("Platform.Dialog")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("xcb")
