-- Приватный бэкенд Platform.Input: XCB. Наружу не виден.
target("Platform.Input")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
