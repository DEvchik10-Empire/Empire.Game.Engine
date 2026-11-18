-- Приватный бэкенд Platform.Monitor: XLib. Наружу не виден.
target("Platform.Monitor")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
