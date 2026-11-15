-- Приватный бэкенд Platform.Cursor: XCB. Наружу не виден.
target("Platform.Cursor")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
