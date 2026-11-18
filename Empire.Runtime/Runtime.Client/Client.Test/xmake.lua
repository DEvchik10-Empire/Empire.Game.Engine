if is_mode("test") then
    target("Client.Test")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
