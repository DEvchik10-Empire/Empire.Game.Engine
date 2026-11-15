-- Приватный бэкенд Platform.App: XLib. Наружу не виден.
target("Platform.App")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
