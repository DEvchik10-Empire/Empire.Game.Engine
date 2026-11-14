-- Приватный бэкенд Platform.Events: XCB. Наружу не виден.
target("Platform.Events")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
