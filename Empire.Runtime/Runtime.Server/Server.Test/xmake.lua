if is_mode("test") then
    target("Server.Test")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
