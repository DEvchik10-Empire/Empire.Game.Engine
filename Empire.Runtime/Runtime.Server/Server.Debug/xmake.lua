if is_mode("debug") then
    target("Server.Debug")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
