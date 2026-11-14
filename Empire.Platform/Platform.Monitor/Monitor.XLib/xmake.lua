-- Приватный бэкенд Platform.Monitor: XLib. Наружу не виден.
target("Platform.Monitor")
    add_files("*.cpp")
    add_deps("Platform.ThirdParty")
