target("Platform.Audio")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("Audio.ALSA") end
if is_plat("macosx") then includes("Audio.CoreAudio") end
if is_plat("linux") then includes("Audio.PipeWire") end
if is_plat("windows") then includes("Audio.WASAPI") end
