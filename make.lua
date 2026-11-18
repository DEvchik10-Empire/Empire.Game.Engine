-- Empire: корневой файл. Всё общее для сборки живёт здесь.
-- Публічні версії проєкту. Змінюйте значення повністю: номер, канал, дату та суфікс.
-- Приклад Git-тега: v2.1.0-Dev-26.11.18.
EngineVersion = "v2.2.0-Dev-26.11.18"
MakeToolVersion = "v1.0.0-Dev-26.11.18"

-- set_version нижче — внутрішня числова версія Xmake-проєкту, не публічний Git-тег.
set_project("Empire")
set_version("2.2.0")
set_xmakever("3.1.2")
set_languages("c++26")
set_warnings("all", "error")

-- Режимы сборки (MakeTool f -m <режим>), по умолчанию development:
--   debug        : -O0, символы
--   debuggame    : движок оптимизирован (+символы), код слоя Runtime без оптимизаций
--   development  : оптимизация + символы
--   test         : как development + сборка тестов (MakeTool test)
--   shipping     : максимальная оптимизация + LTO, без символов; редактор не собирается
-- DebugGame применяется только к игровому продукту; Engine в этом случае использует Development.
-- Корневой файл называется make.lua и передаётся инструменту через -F make.lua.
set_allowedmodes("debug", "debuggame", "development", "test", "shipping")
set_defaultmode("development")

-- Служебное состояние Build Tool (.build_cache, .deps, .gens) тоже живёт в Empire.Build, без папки build/.
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
option("editor")  set_default(not is_mode("shipping")) set_showmenu(true) set_description("Собирать редактор (Runtime.Editor, Nodes.Editor); по умолчанию выключен в Shipping") option_end()
option("tests")   set_default(is_mode("test")) set_showmenu(true) set_description("Собирать тесты; включены по умолчанию в режиме Test") option_end()
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

-- ---------- правила размещения артефактов ----------
-- Продукты разделены физически: Engine и Game не перезаписывают друг друга.
-- DebugGame собирает игровой код без оптимизации, а Engine использует Development.
rule("Empire.Module")
    on_load(function (target)
        import("core.project.config")

        local plats = {linux = "Linux", windows = "Windows", macosx = "MacOS", android = "Android", iphoneos = "iOS", wasm = "WASM"}
        local modes = {debug = "Debug", debuggame = "DebugGame", development = "Development", test = "Test", shipping = "Shipping"}
        local plat = plats[config.get("plat")] or config.get("plat")
        local mode = config.get("mode")
        local name = target:name()
        local rel = path.relative(target:scriptdir(), os.projectdir()):gsub("\\", "/")

        local is_game = rel:match("^Empire%.Runtime/Runtime%.Game")
            or rel:match("^Empire%.Runtime/Runtime%.Client")
            or rel:match("^Empire%.Runtime/Runtime%.Server")
            or rel:match("^Empire%.Runtime/Runtime%.Shared")
            or rel:match("^Empire%.Samples")
            or name:match("^Runtime%.Game") or name:match("^Runtime%.Client") or name:match("^Runtime%.Server")
            or name:match("^Game%.") or name:match("^Client%.") or name:match("^Server%.")

        local product = is_game and "Game" or "Engine"
        target:set("group", product)

        local output_mode = mode
        if mode == "debuggame" and not is_game then output_mode = "development" end
        local mode_label = modes[output_mode] or output_mode
        local root = path.join(os.projectdir(), "Empire.Build", "Build." .. plat .. "." .. product .. "." .. mode_label)

        local layer = target:values("Empire.Layer") or rel:match("^Empire%.([^/]+)") or "Engine"
        if layer == "Samples" then layer = "Game" end
        if is_game then layer = "Game" end

        target:set("targetdir", path.join(root, "Build." .. layer))
        target:set("objectdir", path.join(root, ".Obj", mode, name))

        -- У DebugGame лише ігрові модулі не оптимізуються; Engine залишається Development.
        if mode == "debuggame" then
            if is_game then target:set("optimize", "none") else target:set("optimize", "fast") end
        end

        local origin = target:is_plat("macosx") and "@loader_path" or "$ORIGIN"
        target:add("rpathdirs", origin)
        if is_game then
            local engine_mode = modes[mode == "debuggame" and "development" or mode] or mode
            target:add("rpathdirs", origin .. "/../../Build." .. plat .. ".Engine." .. engine_mode .. "/Build.Engine")
        end
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

-- Запуск:   MakeTool -F make.lua run Runtime.Game | Runtime.Client | Runtime.Server | Runtime.Editor
-- Тесты:    MakeTool -F make.lua f -m test -y && MakeTool -F make.lua build && MakeTool -F make.lua test
-- Удобнее:  Empire.Scripts/Make.sh|Make.bat|Make.command [Команда] [Опции]
