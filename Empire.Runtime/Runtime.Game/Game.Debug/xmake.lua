if is_mode("debug") then
    target("Game.Debug")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
