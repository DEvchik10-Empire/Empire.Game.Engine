-- Приватный бэкенд Platform.Surface: XLib. Наружу не виден.
target("Platform.Surface")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
