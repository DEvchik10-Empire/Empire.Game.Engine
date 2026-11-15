includes(
    "Runtime.Shared",
    "Runtime.Client",
    "Runtime.Server",
    "Runtime.Standalone",
    "Runtime.Game"
)

if has_config("editor") then includes("Runtime.Editor") end
