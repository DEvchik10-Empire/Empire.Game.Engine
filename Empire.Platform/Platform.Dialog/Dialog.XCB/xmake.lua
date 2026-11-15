-- Приватный бэкенд Platform.Dialog: XCB. Наружу не виден.
target("Platform.Dialog")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
