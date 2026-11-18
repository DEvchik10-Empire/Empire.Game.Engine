if is_mode("development") then
    target("Game.Development")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
