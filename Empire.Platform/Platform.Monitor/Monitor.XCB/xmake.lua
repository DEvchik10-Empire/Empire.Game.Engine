-- Приватный бэкенд Platform.Monitor: XCB. Наружу не виден.
target("Platform.Monitor")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
