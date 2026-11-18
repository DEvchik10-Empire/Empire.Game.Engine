-- редактор: запускается как Runtime.Editor
target("Runtime.Editor")
    set_kind("binary")
    add_files("*.cpp")

includes(
    "Editor.Framework",
    "Editor.Gizmo",
    "Editor.Panel",
    "Editor.Plugin",
    "Editor.Tool",
    "Editor.Viewport"
)
