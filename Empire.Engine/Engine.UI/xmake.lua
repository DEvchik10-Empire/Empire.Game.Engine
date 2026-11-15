target("Engine.UI")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.Assets", "Engine.Input", "Math.Geometry", "Math.Color")

includes(
    "UI.Layout",
    "UI.Text",
    "UI.Style",
    "UI.Widget",
    "UI.Binding",
    "UI.Navigation",
    "UI.Animation",
    "UI.Draw",
    "UI.Canvas"
)
