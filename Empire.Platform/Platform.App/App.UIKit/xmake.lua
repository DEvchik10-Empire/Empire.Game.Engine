-- Приватный бэкенд Platform.App: UIKit. Наружу не виден.
target("Platform.App")
    add_deps("Platform.ThirdParty")
    add_files("*.mm")
    add_headerfiles("*.hpp")
    add_frameworks("UIKit")
