if is_mode("test") then
    target("Game.Test")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
