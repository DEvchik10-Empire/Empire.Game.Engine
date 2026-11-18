-- Empire: корневой файл. Всё общее для сборки живёт здесь.
set_project("Empire")
set_version("1.0.0")
set_xmakever("3.1.1")
set_languages("c++26")
set_warnings("all", "error")

-- Режимы сборки (xmake f -m <режим>), по умолчанию development:
--   debug        : -O0, символы
--   debuggame    : движок оптимизирован (+символы), код слоя Runtime без оптимизаций
--   development  : оптимизация + символы
--   test         : как development + сборка тестов (xmake test)
--   shipping     : максимальная оптимизация + LTO, без символов; редактор не собирается
-- Различия между конфигурациями — только в том, какие модули собираются (Game.Debug, Game.Shipping и т.д.).
set_allowedmodes("debug", "debuggame", "development", "test", "shipping")
set_defaultmode("development")

-- Служебное xmake (.build_cache, .deps, .gens) тоже живёт в Empire.Build, без папки build/.
set_config("builddir", "Empire.Build")

-- по умолчанию каждый модуль это .so/.dll; Runtime.*, Tools.*, Tests.*, Samples.* — binary
set_kind("shared")

-- "Empire.Engine/Engine.Kernel/Kernel.Time/Time.hpp"
add_includedirs("$(projectdir)")

-- на Windows у shared-библиотек экспортируются все символы автоматически (без макросов в коде)
if is_plat("windows") then
    add_rules("utils.symbols.export_all", {export_classes = true})
end

-- ---------- опции ----------
-- графические бэкенды (пока только выбор, без макросов; код бэкендов подключается через сборку)
option("vulkan") set_default(true)                set_showmenu(true) option_end()
option("opengl") set_default(true)                set_showmenu(true) option_end()
option("d3d12")  set_default(is_plat("windows"))  set_showmenu(true) option_end()
option("d3d11")  set_default(is_plat("windows"))  set_showmenu(true) option_end()
option("metal")  set_default(is_plat("macosx"))   set_showmenu(true) option_end()
option("uwp")    set_default(false)               set_showmenu(true) set_description("Собирать UWP-бэкенды (только Windows)") option_end()

-- что собирать
option("editor")  set_default(true)  set_showmenu(true) set_description("Собирать редактор (Runtime.Editor, Nodes.Editor); в shipping выключен всегда") option_end()
option("tests")   set_default(false) set_showmenu(true) set_description("Собирать тесты вне режима test") option_end()
option("samples") set_default(false) set_showmenu(true) set_description("Собирать Empire.Samples") option_end()

-- ---------- настройки режимов ----------
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    elseif is_mode("debuggame") then
    set_symbols("debug")
    set_optimize("faster")
    elseif is_mode("development") then
    set_symbols("debug")
    set_optimize("fast")
    elseif is_mode("test") then
    set_symbols("debug")
    set_optimize("fast")
    elseif is_mode("shipping") then
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
    set_policy("build.optimization.lto", true)
end

-- ---------- общее правило для всех таргетов ----------
-- Раскладка: Empire.Build/Build.<Платформа>.<Режим>/Build.<Слой>/   (слой = Empire.<Слой> папки модуля)
--            объектники:  Empire.Build/Build.<Платформа>.<Режим>/.Obj/<таргет>/
-- Empire.Samples складываются в Build.Runtime.
rule("Empire.Module")
    on_load(function (target)
        import("core.project.config")

        local plats = {linux = "Linux", windows = "Windows", macosx = "MacOS"}
        local modes = {debug = "Debug", debuggame = "DebugGame", development = "Development", test = "Test", shipping = "Shipping"}
        local plat  = plats[config.get("plat")] or config.get("plat")
        local mode  = modes[config.get("mode")] or config.get("mode")
        local root  = path.join(os.projectdir(), "Empire.Build", "Build." .. plat .. "." .. mode)
        local name  = target:name()

        -- слой: явно через set_values("Empire.Layer", ...), иначе по папке (Empire.Engine -> Engine)
        local rel   = path.relative(target:scriptdir(), os.projectdir())
        local layer = target:values("Empire.Layer") or rel:match("^Empire%.([^/\\]+)") or "Engine"
        if layer == "Samples" then layer = "Runtime" end

        target:set("targetdir", path.join(root, "Build." .. layer))
        target:set("objectdir", path.join(root, ".Obj", name))

        -- debuggame: оптимизируется всё, кроме слоя Runtime
        if config.get("mode") == "debuggame" and layer == "Runtime" then
            target:set("optimize", "none")
        end

        -- библиотеки лежат в папках слоёв Engine и Runtime, поэтому rpath на оба (пригодится, когда появятся связи)
        local origin = target:is_plat("macosx") and "@loader_path" or "$ORIGIN"
        for _, l in ipairs({"Engine", "Runtime"}) do
            target:add("rpathdirs", origin .. "/../Build." .. l)
        end
        target:add("rpathdirs", origin)
    end)
rule_end()

add_rules("Empire.Module")

-- ---------- модули ----------
includes(
    "Empire.Engine",
    "Empire.Runtime"
)

if has_config("samples") then
    includes("Empire.Samples")
end

-- Запуск:   xmake run Runtime.Game | Runtime.Client | Runtime.Server | Runtime.Editor
-- Тесты:    xmake f -m test -y && xmake build && xmake test
-- Удобнее:  Empire.Scripts/<Платформа>/Build.* [Режим] [Таргет]   и   Run.* [Game|Client|Server|Editor|Tests] [Режим] [аргументы]
