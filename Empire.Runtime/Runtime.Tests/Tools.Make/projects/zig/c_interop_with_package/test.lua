import("lib.detect.find_tool")

function test_build(t)
    local zig = find_tool("zig")
    if zig then
        t:build()
    else
        return t:skip("zig not found")
    end
end
