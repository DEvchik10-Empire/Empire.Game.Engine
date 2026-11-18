includes(
    "Runtime.Game",
    "Runtime.Client",
    "Runtime.Server",
    "Runtime.Scripting",
    "Runtime.Shared",
    "Runtime.Tools"
)

if has_config("editor") and not is_mode("shipping") then
    includes("Runtime.Editor")
end

if is_mode("test") or has_config("tests") then
    includes("Runtime.Tests")
end
