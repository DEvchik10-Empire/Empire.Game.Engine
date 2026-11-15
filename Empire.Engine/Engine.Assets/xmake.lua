target("Engine.Assets")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.FileSystem", "Engine.Jobs", "Kernel.Containers", "Kernel.Reflection")

includes(
    "Assets.Handle",
    "Assets.Database",
    "Assets.Package",
    "Assets.Loader",
    "Assets.Importer",
    "Assets.Cache",
    "Assets.Stream",
    "Assets.HotReload"
)
