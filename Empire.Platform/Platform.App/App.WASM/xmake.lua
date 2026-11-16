-- Приватный бэкенд Platform.App: WASM. Наружу не виден.
target("Platform.App")
    add_deps("Platform.ThirdParty")
    add_files("*.cpp")
    add_headerfiles("*.hpp")
