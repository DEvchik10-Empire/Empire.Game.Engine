
set_project("Empire.Shaders")

-- Скомпилированные .spv складываются в <корень проекта>/build/shaders
-- (этот путь знает тест сцены через define EMPIRE_SHADER_DIR).

-- HLSL -> SPIR-V через DXC (DirectXShaderCompiler).
-- *.vs.hlsl -> vs_6_0, *.ps.hlsl -> ps_6_0, точка входа всегда main.
-- Путь к dxc можно задать переменной окружения DXC.

rule("hlsl2spv")
    set_extensions(".hlsl")

    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        import("lib.detect.find_tool")

        local outdir = path.join(os.projectdir(), "build", "shaders")

        local dxc = os.getenv("DXC")
            or path.join(os.projectdir(), "ThirdParty", "dxc", "bin", "dxc")

        if not os.isfile(dxc) then
            dxc = (find_tool("dxc") or {}).program
        end

        assert(dxc, "dxc not found: install DirectXShaderCompiler or set DXC=/path/to/dxc")

        local name = path.basename(path.basename(sourcefile))
        local stage = path.extension(path.basename(sourcefile))
        local profile = (stage == ".vs") and "vs_6_0" or "ps_6_0"
        local kind = stage:sub(2)
        local output = path.join(outdir, name .. "." .. kind .. ".spv")

        batchcmds:show_progress(
            opt.progress,
            "${color.build.object}compiling.hlsl %s",
            sourcefile
        )

        batchcmds:mkdir(outdir)

        batchcmds:vrunv(dxc, {
            "-spirv",
            "-fspv-target-env=vulkan1.1",
            "-T", profile,
            "-E", "main",
            "-I", path.directory(sourcefile),
            sourcefile,
            "-Fo", output
        })

        batchcmds:add_depfiles(sourcefile)
        batchcmds:add_depfiles(
            os.files(path.join(path.directory(sourcefile), "Common", "*.hlsli"))
        )

        batchcmds:set_depmtime(os.mtime(output))
        batchcmds:set_depcache(target:dependfile(output))
    end)

target("Empire.Shaders")
    set_kind("object")
    add_rules("hlsl2spv")
    add_files("*.hlsl")
