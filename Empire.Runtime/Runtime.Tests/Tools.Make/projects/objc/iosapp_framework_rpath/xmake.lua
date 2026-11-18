add_rules("mode.release", "mode.debug")

target("test")
    add_rules("xcode.framework")
    add_files("src/framework/test.m")
    add_files("src/framework/Info.plist")
    add_headerfiles("src/framework/test.h")

target("demo")
    add_rules("xcode.application")
    add_deps("test")
    add_files("src/app/*.m")
    add_files("src/app/Info.plist")
