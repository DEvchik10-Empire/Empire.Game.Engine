add_rules("mode.debug", "mode.release")

target("libalpha")
    set_kind("shared")
    add_files("src/libalpha/*.cs")

target("libbeta")
    set_kind("shared")
    add_deps("libalpha")
    add_files("src/libbeta/*.cs")

target("sample")
    set_kind("binary")
    add_deps("libbeta")
    add_files("src/sample/*.cs")
