-- Приватный бэкенд Platform.App: Cocoa. Наружу не виден.
target("Platform.App")
    add_deps("Platform.ThirdParty")
    add_files("*.mm")
    add_frameworks("AppKit")
