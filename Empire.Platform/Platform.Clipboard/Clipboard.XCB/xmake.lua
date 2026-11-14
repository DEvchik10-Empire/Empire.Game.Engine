-- Приватный бэкенд Platform.Clipboard: XCB. Наружу не виден.
target("Platform.Clipboard")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
