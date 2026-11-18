if is_mode("shipping") then
    target("Server.Shipping")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
