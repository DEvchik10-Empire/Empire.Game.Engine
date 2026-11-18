if is_mode("shipping") then
    target("Game.Shipping")
        add_headerfiles("*.hpp")
        add_files("*.cpp")
end
