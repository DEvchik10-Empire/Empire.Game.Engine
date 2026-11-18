if is_mode("shipping") then
    target("Client.Shipping")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
