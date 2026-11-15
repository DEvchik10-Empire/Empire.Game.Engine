-- Приватный бэкенд Platform.App: XCB (X11/xcb). Наружу не виден.
target("Platform.App")
    add_files("*.cpp")
    add_syslinks("xcb")