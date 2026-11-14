-- Приватный бэкенд Platform.Window: XLib. Наружу не виден.
target("Platform.Window")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
