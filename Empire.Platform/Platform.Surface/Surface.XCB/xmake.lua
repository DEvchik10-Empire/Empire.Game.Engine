-- Приватный бэкенд Platform.Surface: XCB. Наружу не виден.
target("Platform.Surface")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
