inherit("test_base")

local _CLANG_MIN_VER = is_subhost("windows") and "19" or "18"
local _GCC_MIN_VER = "13"
local _MSVC_MIN_VER = "14.30"

function clang_min_ver()
    return _CLANG_MIN_VER
end
function gcc_min_ver()
    return _GCC_MIN_VER
end
function msvc_min_ver()
    return _MSVC_MIN_VER
end

function main(_)
    local clang_options = {compiler = "clang", version = clang_min_ver()}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver()}
    local msvc_options = {version = msvc_min_ver()}
    run_tests(clang_options, gcc_options, msvc_options)
end
