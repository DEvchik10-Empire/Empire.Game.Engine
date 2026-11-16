-- Приватный бэкенд Platform.App: UWP. Наружу не виден.
target("Platform.App")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
    add_defines("WINAPI_FAMILY=WINAPI_FAMILY_APP")
    add_syslinks("windowsapp")
