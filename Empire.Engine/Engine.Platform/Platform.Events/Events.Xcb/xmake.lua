-- Приватный бэкенд Platform.Events: Xcb. Наружу не виден.
target("Platform.Events")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("xcb")
