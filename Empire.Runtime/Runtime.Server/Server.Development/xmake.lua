if is_mode("development") then
    target("Server.Development")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
