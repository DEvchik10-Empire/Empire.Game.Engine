-- Приватный бэкенд Platform.Monitor: Xcb. Наружу не виден.
target("Platform.Monitor")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_syslinks("xcb")
