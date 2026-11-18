-- Приватный бэкенд Platform.Window: XLib. Наружу не виден.
target("Platform.Window")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
