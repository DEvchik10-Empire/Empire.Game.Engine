-- Tools.Make (xmake) собирается отдельно: см. ../Tools.Make/Make.Kernel/xmake.lua
target("Tools.Build")
    set_kind("binary")
    add_files("*.cpp")
