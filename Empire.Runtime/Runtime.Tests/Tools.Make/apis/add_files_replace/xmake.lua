add_rules("mode.debug", "mode.release")

-- test table replaces
target("test")
    set_kind("binary")
    add_files("src/main.c", {rules = "utils.replace", replaces = {
        {'"original"', '"replaced"'},
    }})

-- test function replaces
target("test2")
    set_kind("binary")
    add_files("src/main.c", {rules = "utils.replace", replaces = function (content)
        content = content:gsub('"original"', '"func_replaced"')
        return content
    end})
