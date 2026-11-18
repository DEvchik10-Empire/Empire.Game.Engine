#!/usr/bin/env bash
# Make — система збирання Empire.Game.Engine. Bash 3.2+ (Linux та macOS).
set -Eeuo pipefail

SCRIPT_INVOCATION="${BASH_SOURCE[0]}"
SCRIPT_SOURCE="${BASH_SOURCE[0]}"
while [[ -L "$SCRIPT_SOURCE" ]]; do
    SCRIPT_DIR="$(cd -P "$(dirname "$SCRIPT_SOURCE")" >/dev/null 2>&1 && pwd)"
    LINK_TARGET="$(readlink "$SCRIPT_SOURCE")"
    if [[ "$LINK_TARGET" = /* ]]; then SCRIPT_SOURCE="$LINK_TARGET"; else SCRIPT_SOURCE="$SCRIPT_DIR/$LINK_TARGET"; fi
done
SCRIPT_DIR="$(cd -P "$(dirname "$SCRIPT_SOURCE")" >/dev/null 2>&1 && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." >/dev/null 2>&1 && pwd -P)"
TOOLS_MAKE="$ROOT/Empire.Runtime/Runtime.Tools/Tools.Make"
PROJECT_FILE="$ROOT/make.lua"

read_project_version() {
    local key="$1"
    [[ -f "$PROJECT_FILE" ]] || return 1
    sed -nE "s/^[[:space:]]*${key}[[:space:]]*=[[:space:]]*['\"]([^'\"]*)['\"].*$/\1/p" "$PROJECT_FILE" | head -n 1
}

if [[ ! -f "$PROJECT_FILE" || ! -f "$TOOLS_MAKE/configure" || ! -d "$TOOLS_MAKE/Make.Runtime" ]]; then
    printf 'Make: структура проєкту неповна. Не знайдено make.lua або Tools.Make.\n' >&2
    exit 2
fi

HOST="$(uname -s 2>/dev/null || printf unknown)"
case "$HOST" in
    Darwin) HOST_PLATFORM="macosx"; HOST_PLATFORM_LABEL="MacOS"; PLATFORM_LABEL="MacOS" ;;
    MINGW*|MSYS*|CYGWIN*) HOST_PLATFORM="windows"; HOST_PLATFORM_LABEL="Windows"; PLATFORM_LABEL="Windows" ;;
    *) HOST_PLATFORM="linux"; HOST_PLATFORM_LABEL="Linux"; PLATFORM_LABEL="Linux" ;;
esac
TARGET_PLATFORM=""
PRODUCT=""
MODE="development"
MODE_LABEL="Development"
MODE_EXPLICIT=0
JOBS=""
VERBOSE=0
QUIET=0
FORCE=0
CLEAN_ALL=0
EDITOR_OPTION=""
TESTS_OPTION=""
SAMPLES_OPTION=""
PORT="7777"
PORT_EXPLICIT=0
APP_ARGS=()
POSITIONAL=()

usage() {
cat <<HELP
Empire.Game.Engine — Make

ШВИДКИЙ СТАРТ
  1. Один раз зберіть інструмент для своєї ОС:
     ${SCRIPT_INVOCATION} Bootstrap Development
  2. Зберіть потрібний продукт і режим:
     ${SCRIPT_INVOCATION} Build Engine Development -j8
     ${SCRIPT_INVOCATION} Build Game DebugGame -j8
  3. Для решти команд дивіться приклади нижче.

КОМАНДИ
  Build       Зібрати один продукт. Обов'язкові продукт і режим.
  Rebuild     Примусово перебудувати лише вибраний продукт. Обов'язкові продукт і режим.
  Bootstrap   Зібрати MakeTool із вихідного коду; режим можна вказати як аргумент.
  Configure   Налаштувати проєкт без компіляції.
  Clean       Очистити артефакти; -All видаляє всі build-каталоги й кеші.
  Run         Зібрати та запустити Game або вказану ціль.
  RunEditor   Запустити редактор.
  RunClient   Запустити клієнт.
  RunServer   Запустити сервер.
  RunDedicated Запустити виділений сервер.
  RunClientServer Запустити клієнт і сервер разом.
  RunTests    Зібрати та запустити тести.
  CopyContent Скопіювати ресурси до збірки Game.
  Info        Показати інформацію про систему та проєкт.
  Version     Показати версії Engine і MakeTool.
  Help        Показати цю довідку.

ПРОДУКТИ ТА РЕЖИМИ
  Engine:   Debug, Development, Test, Shipping
  Game:     Debug, DebugGame, Development, Test, Shipping
  MakeTool: Debug, Development, Test, Shipping

  Для Build/Rebuild вкажіть продукт і режим позиційно:
    Build Engine Shipping
    Build Game DebugGame
    Build MakeTool Development
  Дефіси потрібні лише для параметрів і перемикачів: -j8, -g тощо.
  Старий запис -Engine/-Game/-MakeTool та -Shipping також підтримується.
  DebugGame доступний лише для Game. У цьому режимі Engine використовує
  Development, а Game — DebugGame.
  Release є псевдонімом Shipping.

ВАЖЛИВО ПРО MAKETOOL
  MakeTool збирається окремо для ОС, на якій запускається Make.
  Не додавайте до MakeTool параметри платформи (Linux тощо).
  Engine/Game використовують будь-яку наявну збірку MakeTool для host-ОС.
  Режим MakeTool не зобов'язаний збігатися з режимом Engine/Game.
  Якщо MakeTool ще не зібраний, спочатку виконайте Bootstrap Development.

ПЛАТФОРМИ
  Linux, Windows, MacOS, Android, iOS, WASM
  Платформа задає ціль Engine/Game; для MakeTool вона завжди host-ОС.

ПОШИРЕНІ ПАРАМЕТРИ
  -j8 або -j 8   Кількість паралельних задач.
  -Verbose       Докладний вивід. -Quiet — скорочений.
  Для MakeTool ці два параметри не підтримуються.
  -Force         Примусово перебудувати ціль.
  -Editor        Увімкнути редактор; у Shipping він вимкнений.
  -NoEditor      Вимкнути редактор.
  -Tests         Додати тестові цілі поза режимом Test.
  -Samples       Додати Empire.Samples.
  -Port=7777     Порт RunDedicated.
  -All           З Clean видалити Empire.Build, .MakeTool та build.
  --             Наступні аргументи передати запущеній програмі.

ПРИКЛАДИ
  # MakeTool — усі чотири режими, host-ОС
  ${SCRIPT_INVOCATION} -bs Shipping
  ${SCRIPT_INVOCATION} Build MakeTool Debug
  ${SCRIPT_INVOCATION} Build MakeTool Development
  ${SCRIPT_INVOCATION} Build MakeTool Test
  ${SCRIPT_INVOCATION} Build MakeTool Shipping

  # Окремі збірки Engine та Game (приклад для macOS)
  ${SCRIPT_INVOCATION} Build MacOS Engine Development -j8
  ${SCRIPT_INVOCATION} Build Linux Engine Shipping -j8
  ${SCRIPT_INVOCATION} Build MacOS Game DebugGame -j8
  ${SCRIPT_INVOCATION} Build Linux Game Shipping -j8

  # Повна перебудова лише Engine Shipping
  ${SCRIPT_INVOCATION} Rebuild Engine Shipping -j8

  # Запуск, тести та очищення
  ${SCRIPT_INVOCATION} RunEditor -Development
  ${SCRIPT_INVOCATION} RunTests
  ${SCRIPT_INVOCATION} RunDedicated -Shipping -Port=7777
  ${SCRIPT_INVOCATION} Clean
  ${SCRIPT_INVOCATION} Clean -All

  # Службові команди
  ${SCRIPT_INVOCATION} Info
  ${SCRIPT_INVOCATION} Version
  ${SCRIPT_INVOCATION} Help

ПРИКЛАДИ КАТАЛОГІВ
  Empire.Build/Build.Linux.Engine.Shipping/
  Empire.Build/Build.Linux.Game.DebugGame/
  Empire.Build/Build.Linux.MakeTool.Development/

ВЕРСІЇ
  Публічні версії задаються змінними EngineVersion і MakeToolVersion
  у кореневому make.lua. set_version() — внутрішня версія Xmake.

ПІДКАЗКА
  Запустіть скрипт без аргументів, щоб побачити короткі приклади.
HELP
}

fail() { printf 'Make: %s\n' "$*" >&2; exit 1; }

if (($# == 0)); then
    printf 'Empire.Game.Engine — Make\n'
    printf 'Повна довідка: %s Help\n' "${SCRIPT_INVOCATION}"
    printf '\nПриклади (кожен запускається окремо):\n'
    printf '  %s Bootstrap Development\n' "${SCRIPT_INVOCATION}"
    printf '  %s Build Engine Development -j8\n' "${SCRIPT_INVOCATION}"
    printf '  %s Build Game DebugGame -j8\n' "${SCRIPT_INVOCATION}"
    exit 0
fi
command_name="$1"
shift
case "$command_name" in
    Bootstrap|-bs) COMMAND=Bootstrap ;;
    Configure|-cf) COMMAND=Configure ;;
    Build|-b) COMMAND=Build ;;
    Rebuild|-rb) COMMAND=Rebuild ;;
    Clean|-c) COMMAND=Clean ;;
    CopyContent|-cc) COMMAND=CopyContent ;;
    Info|-i) COMMAND=Info ;;
    Run|-r) COMMAND=Run ;;
    RunEditor|-re) COMMAND=RunEditor ;;
    RunClient|-rc) COMMAND=RunClient ;;
    RunServer|-rs) COMMAND=RunServer ;;
    RunDedicated|-rd) COMMAND=RunDedicated ;;
    RunClientServer|-rcs) COMMAND=RunClientServer ;;
    RunTests|-t) COMMAND=RunTests ;;
    Help|-h|--help|-Help) COMMAND=Help ;;
    Version|--version|-Version) COMMAND=Version ;;
    *) fail "невідома команда '$command_name'. Використайте -Help." ;;
esac

while (($#)); do
    arg="$1"; shift
    case "$arg" in
        -Debug)
            ((MODE_EXPLICIT == 0)) || fail "вкажіть лише один режим збірки"
            MODE=debug; MODE_LABEL=Debug; MODE_EXPLICIT=1 ;;
        -DebugGame)
            ((MODE_EXPLICIT == 0)) || fail "вкажіть лише один режим збірки"
            MODE=debuggame; MODE_LABEL=DebugGame; MODE_EXPLICIT=1 ;;
        -Development)
            ((MODE_EXPLICIT == 0)) || fail "вкажіть лише один режим збірки"
            MODE=development; MODE_LABEL=Development; MODE_EXPLICIT=1 ;;
        -Test)
            ((MODE_EXPLICIT == 0)) || fail "вкажіть лише один режим збірки"
            MODE=test; MODE_LABEL=Test; MODE_EXPLICIT=1 ;;
        -Shipping|-Release)
            ((MODE_EXPLICIT == 0)) || fail "вкажіть лише один режим збірки"
            MODE=shipping; MODE_LABEL=Shipping; MODE_EXPLICIT=1 ;;
        -Engine|-Game|-MakeTool)
            [[ -z "$PRODUCT" ]] || fail "вкажіть лише один продукт: -Engine, -Game або -MakeTool"
            PRODUCT="${arg#-}" ;;
        -Linux)
            [[ -z "$TARGET_PLATFORM" ]] || fail "вкажіть лише одну цільову платформу"
            TARGET_PLATFORM=linux; PLATFORM_LABEL=Linux ;;
        -Windows)
            [[ -z "$TARGET_PLATFORM" ]] || fail "вкажіть лише одну цільову платформу"
            TARGET_PLATFORM=windows; PLATFORM_LABEL=Windows ;;
        -MacOS)
            [[ -z "$TARGET_PLATFORM" ]] || fail "вкажіть лише одну цільову платформу"
            TARGET_PLATFORM=macosx; PLATFORM_LABEL=MacOS ;;
        -Android)
            [[ -z "$TARGET_PLATFORM" ]] || fail "вкажіть лише одну цільову платформу"
            TARGET_PLATFORM=android; PLATFORM_LABEL=Android ;;
        -iOS)
            [[ -z "$TARGET_PLATFORM" ]] || fail "вкажіть лише одну цільову платформу"
            TARGET_PLATFORM=iphoneos; PLATFORM_LABEL=iOS ;;
        -WASM)
            [[ -z "$TARGET_PLATFORM" ]] || fail "вкажіть лише одну цільову платформу"
            TARGET_PLATFORM=wasm; PLATFORM_LABEL=WASM ;;
        -j[0-9]*) JOBS="${arg#-j}" ;;
        -j)
            (($#)) || fail "параметр -j потребує кількість задач"
            JOBS="$1"; shift ;;
        -Verbose) VERBOSE=1 ;;
        -Quiet) QUIET=1 ;;
        -Force) FORCE=1 ;;
        -All) CLEAN_ALL=1 ;;
        -Editor) EDITOR_OPTION="--editor=y" ;;
        -NoEditor) EDITOR_OPTION="--editor=n" ;;
        -Tests) TESTS_OPTION="--tests=y" ;;
        -Samples) SAMPLES_OPTION="--samples=y" ;;
        -Port=*) PORT="${arg#-Port=}"; PORT_EXPLICIT=1 ;;
        -Port)
            (($#)) || fail "параметр -Port потребує номер порту"
            PORT="$1"; PORT_EXPLICIT=1; shift ;;
        --) APP_ARGS=("$@"); break ;;
        -*) fail "невідомий параметр '$arg'. Використайте -Help." ;;
        *) POSITIONAL+=("$arg") ;;
    esac
done

# Product, mode and platform names are values, so their normal form has no leading dash.
# Keep the dashed spellings as backwards-compatible aliases.
if [[ "$COMMAND" == Build || "$COMMAND" == Rebuild || "$COMMAND" == Bootstrap ]]; then
    local_positionals=("${POSITIONAL[@]}")
    POSITIONAL=()
    for value in "${local_positionals[@]}"; do
        case "${value,,}" in
            engine|game|maketool)
                [[ -z "$PRODUCT" ]] || fail "вкажіть лише один продукт: Engine, Game або MakeTool"
                case "${value,,}" in
                    engine) PRODUCT=Engine ;;
                    game) PRODUCT=Game ;;
                    maketool) PRODUCT=MakeTool ;;
                esac
                ;;
            debug|development|test|shipping|release|debuggame)
                ((MODE_EXPLICIT == 0)) || fail "вкажіть лише один режим збірки"
                case "${value,,}" in
                    debug) MODE=debug; MODE_LABEL=Debug ;;
                    development) MODE=development; MODE_LABEL=Development ;;
                    test) MODE=test; MODE_LABEL=Test ;;
                    shipping|release) MODE=shipping; MODE_LABEL=Shipping ;;
                    debuggame) MODE=debuggame; MODE_LABEL=DebugGame ;;
                esac
                MODE_EXPLICIT=1
                ;;
            linux|windows|macos|macosx|android|ios|wasm)
                [[ -z "$TARGET_PLATFORM" ]] || fail "вкажіть лише одну цільову платформу"
                case "${value,,}" in
                    linux) TARGET_PLATFORM=linux; PLATFORM_LABEL=Linux ;;
                    windows) TARGET_PLATFORM=windows; PLATFORM_LABEL=Windows ;;
                    macos|macosx) TARGET_PLATFORM=macosx; PLATFORM_LABEL=MacOS ;;
                    android) TARGET_PLATFORM=android; PLATFORM_LABEL=Android ;;
                    ios) TARGET_PLATFORM=iphoneos; PLATFORM_LABEL=iOS ;;
                    wasm) TARGET_PLATFORM=wasm; PLATFORM_LABEL=WASM ;;
                esac
                ;;
            *) POSITIONAL+=("$value") ;;
        esac
    done
fi

if [[ -n "$JOBS" && ! "$JOBS" =~ ^[1-9][0-9]*$ ]]; then fail "неправильна кількість задач '$JOBS'"; fi
if [[ ! "$PORT" =~ ^[0-9]+$ ]] || ((PORT < 1 || PORT > 65535)); then fail "неправильний номер порту '$PORT'"; fi
((CLEAN_ALL == 0)) || [[ "$COMMAND" == Clean ]] || fail "-All підтримується лише командою Clean"
((PORT_EXPLICIT == 0)) || [[ "$COMMAND" == RunDedicated ]] || fail "-Port підтримується лише командою RunDedicated"
((FORCE == 0)) || [[ "$COMMAND" == Build || "$COMMAND" == Rebuild || "$COMMAND" == Bootstrap ]] || fail "-Force підтримується лише Build, Rebuild та Bootstrap"
[[ -z "$EDITOR_OPTION$TESTS_OPTION$SAMPLES_OPTION" ]] || [[ "$COMMAND" == Build || "$COMMAND" == Rebuild || "$COMMAND" == Configure || "$COMMAND" == Run || "$COMMAND" == RunEditor || "$COMMAND" == RunClient || "$COMMAND" == RunServer || "$COMMAND" == RunDedicated || "$COMMAND" == RunClientServer ]] || fail "параметри Editor/Tests/Samples не підтримуються цією командою"

if [[ -n "${EMPIRE_CONFIG_FLAGS:-}" ]]; then
    # Intentional word splitting: this variable contains additional CLI options.
    read -r -a EXTRA_CONFIG_FLAGS <<< "$EMPIRE_CONFIG_FLAGS"
else
    EXTRA_CONFIG_FLAGS=()
fi
CONFIG_FLAGS=()
[[ -n "$EDITOR_OPTION" ]] && CONFIG_FLAGS+=("$EDITOR_OPTION")
[[ -n "$TESTS_OPTION" ]] && CONFIG_FLAGS+=("$TESTS_OPTION")
[[ -n "$SAMPLES_OPTION" ]] && CONFIG_FLAGS+=("$SAMPLES_OPTION")
CONFIG_FLAGS+=("${EXTRA_CONFIG_FLAGS[@]}")

if [[ "$MODE" == shipping ]]; then
    [[ "$EDITOR_OPTION" != "--editor=y" ]] || fail "-Editor несумісний із Shipping: редактор у Shipping вимкнений"
    [[ "$EDITOR_OPTION" == "--editor=n" ]] || CONFIG_FLAGS+=(--editor=n)
fi
if [[ "$MODE" == test && "$TESTS_OPTION" != "--tests=y" ]]; then CONFIG_FLAGS+=(--tests=y); fi

if ((VERBOSE)); then TOOL_GLOBAL_FLAGS=(-v); else TOOL_GLOBAL_FLAGS=(); fi
if ((QUIET)); then TOOL_GLOBAL_FLAGS+=(-q); fi

cpu_count() {
    local count=""
    if [[ "$HOST" == Darwin ]]; then count="$(sysctl -n hw.ncpu 2>/dev/null || true)"; fi
    if [[ -z "$count" ]] && command -v nproc >/dev/null 2>&1; then count="$(nproc 2>/dev/null || true)"; fi
    if [[ -z "$count" ]]; then count="${NUMBER_OF_PROCESSORS:-2}"; fi
    [[ "$count" =~ ^[1-9][0-9]*$ ]] || count=2
    printf '%s' "$count"
}
[[ -n "$JOBS" ]] || JOBS="$(cpu_count)"

find_build_tool() {
    if [[ -n "${EMPIRE_BUILD_TOOL:-}" && -x "${EMPIRE_BUILD_TOOL}" ]]; then printf '%s' "$EMPIRE_BUILD_TOOL"; return 0; fi
    local candidate=""
    local mode
    local name=MakeTool
    [[ "$HOST_PLATFORM" != windows ]] || name=MakeTool.exe
    # Prefer Development when present, but accept every other built mode too.
    for mode in Development Shipping Test Debug; do
        candidate="$(find "$ROOT/Empire.Build/Build.$HOST_PLATFORM_LABEL.MakeTool.$mode" -type f -name "$name" \
            $( [[ "$HOST_PLATFORM" == windows ]] || printf '%s' '-perm -u+x' ) -print -quit 2>/dev/null || true)"
        [[ -n "$candidate" ]] && { printf '%s' "$candidate"; return 0; }
    done
    candidate="$(find "$ROOT/Empire.Build" -type f -name "$name" -path "$ROOT/Empire.Build/Build.$HOST_PLATFORM_LABEL.MakeTool.*/*" \
        $( [[ "$HOST_PLATFORM" == windows ]] || printf '%s' '-perm -u+x' ) -print -quit 2>/dev/null || true)"
    [[ -n "$candidate" ]] || return 1
    printf '%s' "$candidate"
}

bootstrap_tool() {
    command -v sh >/dev/null 2>&1 || fail "для збирання MakeTool потрібен sh"
    command -v make >/dev/null 2>&1 || fail "для збирання MakeTool потрібен GNU Make"
    local missing=()
    [[ -f "$TOOLS_MAKE/Make.Kernel/TBOX/tbox/src/xmake.sh" ]] || missing+=("TBOX")
    [[ -f "$TOOLS_MAKE/Make.Kernel/Lua/lua/lapi.c" ]] || missing+=("Lua")
    [[ -f "$TOOLS_MAKE/Make.Kernel/CJSON/lua-cjson/lua_cjson.c" ]] || missing+=("Lua-CJSON")
    [[ -f "$TOOLS_MAKE/Make.Kernel/LZ4/lz4/lib/lz4.c" ]] || missing+=("LZ4")
    [[ -d "$TOOLS_MAKE/Make.Kernel/SV/sv/src" ]] && [[ -n "$(find "$TOOLS_MAKE/Make.Kernel/SV/sv/src" -maxdepth 1 -name '*.c' -print -quit 2>/dev/null)" ]] || missing+=("SV")
    [[ -f "$TOOLS_MAKE/Make.Kernel/xxHash/xxhash/xxhash.h" ]] || missing+=("xxHash")
    if ((${#missing[@]})); then
        printf 'Make: вихідні файли MakeTool неповні. Відсутні: %s\n' "${missing[*]}" >&2
        if [[ -f "$ROOT/.gitmodules" || -f "$TOOLS_MAKE/.gitmodules" ]]; then
            printf 'Відновіть вкладені залежності командою: git -C "%s" submodule update --init --recursive\n' "$ROOT" >&2
        else
            printf 'У цьому архіві відсутні деякі залежності. Потрібне повне отримання вихідного коду з усіма вкладеними залежностями.\n' >&2
        fi
        return 1
    fi
    printf 'Збирання MakeTool з вихідного коду: %s\n' "$TOOLS_MAKE"
    local tool_mode="$MODE_LABEL"
    local output="$ROOT/Empire.Build/Build.$HOST_PLATFORM_LABEL.MakeTool.$tool_mode"
    local tool_build_mode
    tool_build_mode="$(printf '%s' "$tool_mode" | tr '[:upper:]' '[:lower:]')"
    mkdir -p "$output"
    local build_plat="$HOST_PLATFORM"
    [[ "$HOST_PLATFORM" != windows ]] || build_plat=mingw
    (
        cd "$TOOLS_MAKE"
        sh ./configure --generator=gmake --plat="$build_plat" --mode="$tool_build_mode" --builddir="../../../Empire.Build/Build.$HOST_PLATFORM_LABEL.MakeTool.$tool_mode"
        if ((FORCE)); then make -B -j"$JOBS"; else make -j"$JOBS"; fi
    ) || fail "не вдалося зібрати MakeTool"
    rm -rf "$output/Make.Runtime"
    cp -R "$TOOLS_MAKE/Make.Runtime" "$output/Make.Runtime"
    read_project_version MakeToolVersion > "$output/Make.Runtime/Make.Version"
    local built="$output/MakeTool"
    [[ "$HOST_PLATFORM" != windows ]] || built="$output/MakeTool.exe"
    [[ -x "$built" || -f "$built" ]] || fail "збирання завершилося, але виконуваний файл MakeTool не знайдено в $output"
    printf 'MakeTool готовий: %s\n' "$built"
}

ensure_tool() {
    # Use any existing host MakeTool build; its mode is independent from the target build mode.
    BUILD_TOOL="$(find_build_tool || true)"
    [[ -n "$BUILD_TOOL" ]] || fail "MakeTool для host-ОС не знайдено. Спочатку зберіть його: $SCRIPT_INVOCATION Bootstrap Development"
    PROGRAM_DIR="$(dirname "$BUILD_TOOL")/Make.Runtime"
    [[ -d "$PROGRAM_DIR" ]] || PROGRAM_DIR="$TOOLS_MAKE/Make.Runtime"
    export MAKETOOL_PROGRAM_DIR="$PROGRAM_DIR"
    export XMAKE_PROGRAM_DIR="$PROGRAM_DIR"
}

run_tool() {
    local action="$1"; shift
    "$BUILD_TOOL" "$action" "${TOOL_GLOBAL_FLAGS[@]}" -P "$ROOT" -F "$PROJECT_FILE" "$@"
}

configure_project() {
    local flags=(-m "$MODE" -y)
    [[ -n "$TARGET_PLATFORM" ]] && flags+=(-p "$TARGET_PLATFORM")
    flags+=("${CONFIG_FLAGS[@]}")
    run_tool f "${flags[@]}"
}

copy_content() {
    local content_mode="$MODE_LABEL"
    [[ "$content_mode" != DebugGame ]] || content_mode=DebugGame
    local out="$ROOT/Empire.Build/Build.$PLATFORM_LABEL.Game.$content_mode"
    mkdir -p "$out"
    rm -rf "$out/Build.Docs" "$out/Build.Content"
    [[ ! -d "$ROOT/Empire.Docs" ]] || cp -R "$ROOT/Empire.Docs" "$out/Build.Docs"
    [[ ! -d "$ROOT/Empire.Content" ]] || cp -R "$ROOT/Empire.Content" "$out/Build.Content"
    [[ ! -f "$ROOT/README.md" ]] || cp "$ROOT/README.md" "$out/README.md"
    printf 'Ресурси скопійовано до %s\n' "$out"
}

build_project() {
    local args=(build -j "$JOBS")
    ((FORCE)) && args+=(-r)
    if [[ "$PRODUCT" == Engine || "$PRODUCT" == Game ]]; then args+=(-g "$PRODUCT"); fi
    args+=("${POSITIONAL[@]}")
    run_tool "${args[@]}"
}

case "$COMMAND" in
    Help) usage; exit 0 ;;
    Version)
        ENGINE_VERSION="$(read_project_version EngineVersion)"
        TOOL_VERSION="$(read_project_version MakeToolVersion)"
        printf 'Версія Empire.Game.Engine: %s\n' "${ENGINE_VERSION:-не задана в make.lua}"
        printf 'Версія MakeTool: %s\n' "${TOOL_VERSION:-не задана в make.lua}"
        exit 0 ;;
    CopyContent) copy_content; exit 0 ;;
esac

if [[ "$COMMAND" == Bootstrap || "$COMMAND" == Build || "$COMMAND" == Rebuild ]]; then
    ((MODE_EXPLICIT == 1)) || fail "режим обов’язковий. Вкажіть Debug, Development, Test або Shipping."
fi
if [[ "$COMMAND" == Bootstrap ]]; then
    ((VERBOSE == 0 && QUIET == 0)) || fail "-Verbose/-Quiet не застосовуються до bootstrap MakeTool"
    [[ -z "$PRODUCT" ]] || fail "Bootstrap збирає лише MakeTool; не вказуйте продукт"
    [[ -z "$TARGET_PLATFORM" ]] || fail "Bootstrap завжди збирає MakeTool для ОС збірочної машини"
    ((${#POSITIONAL[@]} == 0)) || fail "Bootstrap приймає лише режим: Debug, Development, Test або Shipping"
    [[ "$MODE_LABEL" != DebugGame ]] || fail "режим DebugGame доступний лише для Game, а не для MakeTool."
    bootstrap_tool
    exit $?
fi

if [[ "$COMMAND" == Build || "$COMMAND" == Rebuild ]]; then
    [[ -n "$PRODUCT" ]] || fail "для Build/Rebuild обов’язково вкажіть продукт: Engine, Game або MakeTool"
    if [[ "$PRODUCT" == MakeTool ]]; then
        [[ -z "$TARGET_PLATFORM" ]] || fail "-MakeTool збирається лише для ОС збірочної машини; не вказуйте цільову платформу"
        [[ "$MODE_LABEL" != DebugGame ]] || fail "-DebugGame доступний лише для -Game"
        ((${#POSITIONAL[@]} == 0)) || fail "для -MakeTool не можна вказувати цілі проєкту"
        [[ -z "$EDITOR_OPTION$TESTS_OPTION$SAMPLES_OPTION" ]] || fail "-Editor/-NoEditor/-Tests/-Samples не застосовуються до MakeTool"
        ((VERBOSE == 0 && QUIET == 0)) || fail "-Verbose/-Quiet не застосовуються до bootstrap MakeTool"
        [[ "$COMMAND" != Rebuild ]] || FORCE=1
        bootstrap_tool
        exit $?
    fi
    [[ "$PRODUCT" != Engine || "$MODE_LABEL" != DebugGame ]] || fail "-DebugGame доступний лише для -Game"
elif [[ -n "$PRODUCT" ]]; then
    fail "параметри -Engine, -Game та -MakeTool використовуються лише з командою Build"
fi

if [[ "$COMMAND" == RunTests ]]; then MODE=test; MODE_LABEL=Test; fi
ensure_tool
cd "$ROOT"

case "$COMMAND" in
    Configure)
        configure_project ;;
    Build)
        configure_project
        build_project
        [[ "$PRODUCT" != Game ]] || copy_content ;;
    Rebuild)
        configure_project
        FORCE=1
        build_project
        [[ "$PRODUCT" != Game ]] || copy_content ;;
    Clean)
        run_tool clean -a
        if ((CLEAN_ALL)); then rm -rf "$ROOT/Empire.Build" "$ROOT/.MakeTool" "$ROOT/build"; fi ;;
    CopyContent)
        copy_content ;;
    Info)
        printf 'Проєкт: %s\nФайл конфігурації: %s\nMakeTool: %s\n' "$ROOT" "$PROJECT_FILE" "$BUILD_TOOL"
        if [[ "$HOST" == Darwin ]]; then OS_INFO="$(sw_vers -productName 2>/dev/null || printf macOS) $(sw_vers -productVersion 2>/dev/null || true)"; else OS_INFO="$(. /etc/os-release 2>/dev/null && printf '%s' "${PRETTY_NAME:-Linux}" || uname -s) $(uname -r)"; fi
        printf 'Операційна система: %s\nАрхітектура: %s\n' "$OS_INFO" "$(uname -m)"
        if [[ "$HOST" == Darwin ]]; then
            CPU_INFO="$(sysctl -n machdep.cpu.brand_string 2>/dev/null || printf 'невідомо')"
            CPU_THREADS="$(sysctl -n hw.ncpu 2>/dev/null || printf 'невідомо')"
            RAM_BYTES="$(sysctl -n hw.memsize 2>/dev/null || printf 0)"
            printf 'Процесор: %s (%s потоків)\nОперативна пам’ять: %s ГБ\n' "$CPU_INFO" "$CPU_THREADS" "$(awk -v n="$RAM_BYTES" 'BEGIN { if (n>0) printf "%.1f", n/1073741824; else printf "невідомо" }')"
            command -v system_profiler >/dev/null 2>&1 && system_profiler SPDisplaysDataType 2>/dev/null | awk -F': ' '/Chipset Model:/ {print "Відеокарта: " $2; exit}'
        else
            CPU_INFO="$(awk -F: '/model name/ {sub(/^[ \t]+/, "", $2); print $2; exit}' /proc/cpuinfo 2>/dev/null || true)"
            RAM_KB="$(awk '/MemTotal:/ {print $2; exit}' /proc/meminfo 2>/dev/null || true)"
            CPU_THREADS="$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || printf 'невідомо')"
            printf 'Процесор: %s (%s потоків)\n' "${CPU_INFO:-невідомо}" "$CPU_THREADS"
            if [[ "$RAM_KB" =~ ^[0-9]+$ ]]; then printf 'Оперативна пам’ять: %.1f ГБ\n' "$(awk -v n="$RAM_KB" 'BEGIN {print n/1048576}')"; else printf 'Оперативна пам’ять: невідомо\n'; fi
            GPU_INFO=""
            if command -v lspci >/dev/null 2>&1; then
                GPU_INFO="$(lspci 2>/dev/null | awk -F': ' '/VGA compatible controller|3D controller|Display controller/ {print $2; exit}')"
            fi
            if [[ -z "$GPU_INFO" ]] && command -v glxinfo >/dev/null 2>&1; then
                GPU_INFO="$(glxinfo -B 2>/dev/null | sed -n 's/^[[:space:]]*OpenGL renderer string: //p' | head -1)"
            fi
            printf 'Відеокарта: %s\n' "${GPU_INFO:-не визначено (потрібен lspci або glxinfo)}"
        fi
        if command -v df >/dev/null 2>&1; then df -h "$ROOT" | awk 'NR==2 {print "Вільне місце на диску: " $4 " (" $5 " використано)"}'; fi
        ;;
    RunTests)
        MODE=test; MODE_LABEL=Test
        configure_project
        run_tool build -j "$JOBS"
        run_tool test -j "$JOBS" ;;
    RunDedicated)
        if ((MODE_EXPLICIT == 0)); then MODE=shipping; MODE_LABEL=Shipping; fi
        CONFIG_FLAGS+=(--editor=n)
        configure_project
        run_tool build -j "$JOBS" Runtime.Server
        run_tool run Runtime.Server --port "$PORT" "${APP_ARGS[@]}" ;;
    RunClientServer)
        configure_project
        run_tool build -j "$JOBS" Runtime.Server Runtime.Client
        run_tool run Runtime.Server &
        SERVER_PID=$!
        cleanup_server() { kill "$SERVER_PID" 2>/dev/null || true; wait "$SERVER_PID" 2>/dev/null || true; }
        trap cleanup_server EXIT INT TERM
        run_tool run Runtime.Client "${APP_ARGS[@]}"
        ;;
    Run|RunEditor|RunClient|RunServer)
        case "$COMMAND" in
            RunEditor) TARGET=Editor ;;
            RunClient) TARGET=Client ;;
            RunServer) TARGET=Server ;;
            *) TARGET="${POSITIONAL[0]:-Game}"; if ((${#POSITIONAL[@]})); then POSITIONAL=("${POSITIONAL[@]:1}"); fi ;;
        esac
        case "$TARGET" in
            Game|Client|Server|Editor) ;;
            Tests) MODE=test; MODE_LABEL=Test; configure_project; run_tool build -j "$JOBS"; run_tool test -j "$JOBS"; exit $? ;;
            *) fail "невідома ціль Run '$TARGET'. Доступні: Game, Client, Server, Editor." ;;
        esac
        configure_project
        run_tool build -j "$JOBS" "Runtime.$TARGET"
        run_tool run "Runtime.$TARGET" "${APP_ARGS[@]}" ;;
esac
