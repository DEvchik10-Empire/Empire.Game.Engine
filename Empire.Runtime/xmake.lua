includes(
    "Runtime.Shared",
    "Runtime.Game"
)

if has_config("editor") then includes("Runtime.Editor") end
if has_config("tools") then includes("Runtime.Tools") end
