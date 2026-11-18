if is_mode("debuggame") then
    target("Server.DebugGame")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
