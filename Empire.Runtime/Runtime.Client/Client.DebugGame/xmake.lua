if is_mode("debuggame") then
    target("Client.DebugGame")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
