inherit("test_base")

local _MSVC_MIN_VER = "14.30"

function msvc_min_ver()
    return _MSVC_MIN_VER
end

function main(t)
    local gcc_options = {fallbackscanner = true, compiler = "gcc", version = gcc_min_ver()}
    -- gcc/arm64: internal compiler error: in core_vals, at cp/module.cc:6108
    -- on windows, mingw modulemapper doesn't handle headeunit path correctly, but it's working with mingw on macOS / Linux
    if os.arch() == "arm64" or is_subhost("msys") then
        gcc_options = nil
    end
    local msvc_options = {version = msvc_min_ver()}
    -- skip clang tests, headerunits with clang is not stable
    run_tests(nil, gcc_options, msvc_options)
end
