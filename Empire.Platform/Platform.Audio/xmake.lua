target("Platform.Audio")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

if is_plat("windows") then includes("Audio.WASAPI") end
if is_plat("linux") then includes("Audio.PipeWire") end
if is_plat("linux") then includes("Audio.ALSA") end
if is_plat("macosx") then includes("Audio.CoreAudio") end
