if is_mode("development") then
    target("Client.Development")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
