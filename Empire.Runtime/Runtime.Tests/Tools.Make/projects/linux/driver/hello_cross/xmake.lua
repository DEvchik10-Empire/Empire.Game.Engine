set_plat("linux")

option("linux-headers", {description = "Set kernel headers/source tree path."})
option("linux-builddir", {description = "Set kernel build/output tree path."})

-- Example:
-- xmake f -p linux -a arm64 --cc=/path/to/aarch64-linux-gnu-gcc \
--     --ld=/path/to/aarch64-linux-gnu-ld \
--     --linux-headers=/path/to/linux \
--     --linux-builddir=/path/to/linux-out

target("hello")
    add_rules("platform.linux.module")
    add_files("src/*.c")
    set_license("GPL-2.0")
    if has_config("linux-headers") then
        set_values("linux.driver.linux-headers", get_config("linux-headers"))
    end
    if has_config("linux-builddir") then
        set_values("linux.driver.linux-builddir", get_config("linux-builddir"))
    end
