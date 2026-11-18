-- Приватный бэкенд Platform.Events: XLib. Наружу не виден.
target("Platform.Events")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
