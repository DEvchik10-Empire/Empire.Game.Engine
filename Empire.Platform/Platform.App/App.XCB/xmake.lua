-- Приватный бэкенд Platform.App: XCB. Наружу не виден.
target("Platform.App")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
