import("core.project.config")
import("core.platform.platform")
import("core.tool.toolchain")
import("lib.detect.find_tool")
import("detect.sdks.find_dotnet")

function test_vsxmake(t)

    if not is_subhost("windows") then
        return t:skip("wrong host platform")
    end

    local projname = "testproj"
    local tempdir = os.tmpfile()
    os.mkdir(tempdir)
    os.cd(tempdir)

    -- create project
    os.vrunv("xmake", {"create", projname})
    os.cd(projname)

    -- set config
    local arch = os.getenv("platform") or "x86"
    config.set("arch", arch, {readonly = true, force = true})
    platform.load(config.plat(), arch):check()

    -- create sln & vcxproj
    local vs = config.get("vs")
    local vstype = "vsxmake" .. vs
    os.execv("xmake", {"project", "-k", vstype, "-a", arch})
    os.cd(vstype)

    -- run msbuild
    try
    {
        function ()
            local runenvs = toolchain.load("msvc"):runenvs()
            local msbuild = find_tool("msbuild", {envs = runenvs})
            os.execv(msbuild.program, {"/P:XmakeDiagnosis=true", "/P:XmakeVerbose=true"}, {envs = runenvs})
        end,
        catch
        {
            function ()
                print("--- sln file ---")
                io.cat(projname .. ".sln")
                print("--- vcx file ---")
                io.cat(projname .. "/" .. projname .. ".vcxproj")
                print("--- filter file ---")
                io.cat(projname .. "/" .. projname .. ".vcxproj.filters")
                raise("msbuild failed")
            end
        }
    }

    -- clean up
    os.cd(os.scriptdir())
    os.tryrm(tempdir)
end

function test_vsxmake_csharp(t)

    if not is_subhost("windows") then
        return t:skip("wrong host platform")
    end

    local dotnet = find_dotnet()
    if not dotnet or not dotnet.sdkver then
        return t:skip("dotnet sdk not found")
    end

    local projname = "testcsproj"
    local tempdir = os.tmpfile()
    os.mkdir(tempdir)
    os.cd(tempdir)

    -- create a C# console project using xmake create
    os.vrunv("xmake", {"create", "-l", "csharp", "-t", "console", projname})
    os.cd(projname)

    -- create sln & csproj
    local arch = os.getenv("platform") or "x64"
    local vs = config.get("vs")
    local vstype = "vsxmake" .. vs
    os.execv("xmake", {"project", "-k", vstype, "-a", arch})
    os.cd(vstype)

    -- verify a .csproj was generated (not .vcxproj)
    local csproj_path = path.join(projname, projname .. ".csproj")
    assert(os.isfile(csproj_path), "expected .csproj to be generated at " .. csproj_path)
    assert(not os.isfile(path.join(projname, projname .. ".vcxproj")),
        "unexpected .vcxproj generated for C# target")

    -- verify .sln references the .csproj with the C# project type GUID
    local sln_text = io.readfile(projname .. ".sln")
    assert(sln_text:find("FAE04EC0-301F-11D3-BF4B-00C04F79EFBC", 1, true),
        "C# project type GUID missing from .sln")
    assert(sln_text:find(projname .. ".csproj", 1, true),
        ".csproj reference missing from .sln")

    -- verify the .csproj imports Xmake.CSharp.targets and has a Compile item
    local csproj_text = io.readfile(csproj_path)
    assert(csproj_text:find("Xmake.CSharp.targets", 1, true),
        "Xmake.CSharp.targets import missing from .csproj")
    assert(csproj_text:find("<Compile Include=", 1, true),
        "<Compile> item missing from .csproj")

    -- clean up
    os.cd(os.scriptdir())
    os.tryrm(tempdir)
end

function test_compile_commands(t)
    local projname = "testproj"
    local tempdir = os.tmpfile()
    os.mkdir(tempdir)
    os.cd(tempdir)

    -- create project
    os.vrunv("xmake", {"create", projname})
    os.cd(projname)

    -- generate compile_commands
    os.vrunv("xmake", {"project", "-k", "compile_commands"})

    -- test autoupdate
    io.insert("xmake.lua", 1, 'add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode", lsp = "clangd"})')
    os.vrun("xmake")

    -- clean up
    os.cd(os.scriptdir())
    os.tryrm(tempdir)
end

function test_cmake(t)
    local cmake = find_tool("cmake")
    if not cmake then
        return t:skip("cmake not found")
    end
    local projname = "testproj"
    local tempdir = os.tmpfile()
    os.mkdir(tempdir)
    os.cd(tempdir)

    -- create project
    os.vrunv("xmake", {"create", projname})
    os.cd(projname)

    -- generate compile_commands
    os.vrunv("xmake", {"project", "-k", "cmake"})

    -- test build
    os.mkdir("build")
    os.cd("build")
    os.vrunv(cmake.program, {".."})
    os.vrunv(cmake.program, {"--build", "."})

    -- clean up
    os.cd(os.scriptdir())
    os.tryrm(tempdir)
end
