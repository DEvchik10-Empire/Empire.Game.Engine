-- Empire: корневой файл. Всё общее для сборки живёт здесь.
set_project("Empire")
set_version("1.0.0")
set_xmakever("3.1.1")
set_languages("c++26")
set_warnings("all", "error")
add_rules("mode.debug", "mode.release")

-- Служебное xmake (.build_cache, .deps, .gens) тоже живёт в Empire.Build, без папки build/.
set_config("builddir", "Empire.Build")

-- по умолчанию каждый модуль это .so; Tools.* и Tests.* переопределяют на binary
set_kind("shared")

-- <Empire.Platform/Platform.Time/Time.hpp>
add_includedirs("$(projectdir)")
add_cxxflags("-fvisibility=hidden", "-fvisibility-inlines-hidden", {tools = {"gcc", "clang"}})

-- выбор графических бэкендов
option("vulkan")  set_default(true)                  set_showmenu(true) option_end()
option("opengl")  set_default(true)                  set_showmenu(true) option_end()
option("d3d12")   set_default(is_plat("windows"))    set_showmenu(true) option_end()
option("d3d11")   set_default(is_plat("windows"))    set_showmenu(true) option_end()
option("metal")   set_default(is_plat("macosx"))     set_showmenu(true) option_end()

-- что собирать (xmake f --tests=y --editor=n ...)
option("tests")   set_default(false) set_showmenu(true) set_description("Собирать Empire.Tests")  option_end()
option("editor")  set_default(true)  set_showmenu(true) set_description("Собирать Runtime.Editor") option_end()
option("tools")   set_default(true)  set_showmenu(true) set_description("Собирать Empire.Tools")  option_end()

-- Engine.Application.so, без префикса lib; у каждого модуля свой EXPORT-макрос
rule("Empire.Module")
    on_load(function (target)
        import("core.project.config")

        -- Empire.Build/Build.<Платформа>.<Режим>/Build.<Слой>/  (Empire.Engine -> Build.Engine)
        local plats = {linux = "Linux", windows = "Win", macosx = "MacOS"}
        local plat  = plats[config.get("plat")] or config.get("plat")
        local mode  = (config.get("mode"):gsub("^%l", string.upper))
        local root  = path.join(os.projectdir(), "Empire.Build", "Build." .. plat .. "." .. mode)
        local name  = target:name()
        -- слой: явно через set_values (Empire.Client -> Game), иначе по имени без префикса Empire.
        local layer = target:values("Empire.Layer") or name:gsub("^Empire%.", ""):match("^[^%.]+")

        target:set("targetdir", path.join(root, "Build." .. layer))
        target:set("objectdir", path.join(root, ".Obj", name))

        -- .so лежат в папках слоёв, поэтому rpath на каждый слой
        for _, l in ipairs({"Kernel", "Math", "Platform", "Graphics", "Engine", "Runtime"}) do
            target:add("rpathdirs", "$ORIGIN/../Build." .. l)
        end
        target:add("rpathdirs", "$ORIGIN")

        if target:kind() == "shared" then
            target:set("prefixname", "")
            target:add("defines", (target:name():upper():gsub("%.", "_")) .. "_EXPORT")
        end
    end)

    -- Линкер ищет lib<имя>.so по флагу -l<имя>. Настоящий файл остаётся Kernel.Types.so,
    -- а рядом кладётся симлинк libKernel.Types.so, чтобы слинковаться.
    after_link(function (target)
        if target:kind() == "shared" and not target:is_plat("windows") then
            local file = target:targetfile()
            local link = path.join(path.directory(file), "lib" .. path.filename(file))
            os.tryrm(link)
            os.vrunv("ln", {"-sfn", path.filename(file), link})
        end
    end)
rule_end()

add_rules("Empire.Module")

includes(
    "Empire.Kernel",
    "Empire.Math",
    "Empire.Platform",
    "Empire.Graphics",
    "Empire.Engine",
    "Empire.Runtime"
)

if has_config("tools")  then includes("Empire.Tools")  end
if has_config("tests")  then includes("Empire.Tests")  end

-- xmake run Empire.Test  : собрать и прогнать Unit, затем Integration
-- xmake run Empire.Bench : собрать и прогнать бенчмарки
-- (работают только при --tests=y; режим Debug/Release задаётся через xmake f -m)
if has_config("tests") then
    target("Empire.Test")
        set_kind("phony")
        add_deps("Empire.Tests.Unit", "Empire.Tests.Integration")
        on_run(function (target)
            import("core.project.project")
            for _, name in ipairs({"Empire.Tests.Unit", "Empire.Tests.Integration"}) do
                print("==> " .. name)
                os.execv(project.target(name):targetfile())
            end
        end)
    target_end()

    target("Empire.Bench")
        set_kind("phony")
        add_deps("Empire.Tests.Benchmark")
        on_run(function (target)
            import("core.project.project")
            print("==> Empire.Tests.Benchmark")
            os.execv(project.target("Empire.Tests.Benchmark"):targetfile())
        end)
    target_end()
end

-- xmake run Empire.Release : Release-сборка и запуск Empire.Game
-- xmake run Empire.Debug   : Debug-сборка и запуск Empire.Game под отладчиком (нужен gdb или lldb)
-- Режим на время запуска переключается и потом возвращается обратно.
-- Если сборка или запуск упадёт, режим останется переключённым: вернуть xmake f -m <режим>.
target("Empire.Release")
    set_kind("phony")
    on_run(function (target)
        import("core.project.config")
        local xmake    = os.programfile()
        local root     = os.projectdir()
        local previous = config.get("mode")
        if previous ~= "release" then
            os.execv(xmake, {"f", "-m", "release", "-y"}, {curdir = root})
        end
        os.execv(xmake, {"build", "Empire.Game"}, {curdir = root})
        os.execv(xmake, {"run", "Empire.Game"}, {curdir = root})
        if previous ~= "release" then
            os.execv(xmake, {"f", "-m", previous, "-y"}, {curdir = root})
        end
    end)
target_end()

target("Empire.Debug")
    set_kind("phony")
    on_run(function (target)
        import("core.project.config")
        local xmake    = os.programfile()
        local root     = os.projectdir()
        local previous = config.get("mode")
        if previous ~= "debug" then
            os.execv(xmake, {"f", "-m", "debug", "-y"}, {curdir = root})
        end
        os.execv(xmake, {"build", "Empire.Game"}, {curdir = root})
        os.execv(xmake, {"run", "-d", "Empire.Game"}, {curdir = root})
        if previous ~= "debug" then
            os.execv(xmake, {"f", "-m", previous, "-y"}, {curdir = root})
        end
    end)
target_end()
