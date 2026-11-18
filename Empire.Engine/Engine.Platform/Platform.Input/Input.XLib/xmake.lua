-- Приватный бэкенд Platform.Input: XLib. Наружу не виден.
target("Platform.Input")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
