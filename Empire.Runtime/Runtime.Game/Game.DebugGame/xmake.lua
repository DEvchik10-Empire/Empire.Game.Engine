if is_mode("debuggame") then
    target("Game.DebugGame")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
