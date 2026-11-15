set_project("Empire.Shaders")

-- Скомпилированные .spv складываются в <корень проекта>/build/shaders
-- Путь используется тестом сцены через define EMPIRE_SHADER_DIR.

-- HLSL -> SPIR-V через DXC (DirectXShaderCompiler).
--   *.vs.hlsl -> vs_6_0
--   *.ps.hlsl -> ps_6_0
-- Точка входа всегда main.

rule("hlsl2spv")
    set_extensions(".hlsl")

    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        import("lib.detect.find_tool")

        local root = os.projectdir()
        local outdir = path.join(root, "build", "shaders")

        -- Поиск DXC в ThirdParty/dxc/bin.
        local dxc_dir = path.join(root, "ThirdParty", "dxc", "bin")
        local dxc = nil

        for _, candidate in ipairs({
            path.join(dxc_dir, "dxc"),
            path.join(dxc_dir, "dxc.exe")
        }) do
            if os.isfile(candidate) then
                dxc = candidate
                break
            end
        end

        -- Переменная окружения DXC.
        if not dxc then
            local env_dxc = os.getenv("DXC")
            if env_dxc and os.isfile(env_dxc) then
                dxc = env_dxc
            end
        end

        -- Системный PATH.
        if not dxc then
            dxc = (find_tool("dxc") or {}).program
        end

        assert(dxc, "dxc not found: check ThirdParty/dxc/bin or DXC")

        -- Например, path.basename(sourcefile) возвращает Sky.vs.
        local filename = path.basename(sourcefile)
        local name, stage = filename:match("^(.*)%.([^.]+)$")

        assert(
            name and (stage == "vs" or stage == "ps"),
            "Unsupported HLSL shader stage: " .. filename
        )

        local profile = (stage == "vs") and "vs_6_0" or "ps_6_0"
        local output = path.join(outdir, name .. "." .. stage .. ".spv")

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
