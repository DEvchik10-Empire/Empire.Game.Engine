-- Приватный бэкенд Platform.Window: XCB. Наружу не виден.
target("Platform.Window")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
