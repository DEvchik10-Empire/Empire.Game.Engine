if is_mode("debug") then
    target("Client.Debug")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
