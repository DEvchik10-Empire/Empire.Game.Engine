-- Приватная системная зависимость backend-а XCB.
target("Platform.App")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_syslinks("xcb")
