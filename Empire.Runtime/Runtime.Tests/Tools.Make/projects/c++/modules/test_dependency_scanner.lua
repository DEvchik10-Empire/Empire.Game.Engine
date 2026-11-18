inherit("test_base")
import("utils.ci.is_running", {alias = "ci_is_running"})

function _build(platform, toolchain_name, runtimes, policies, flags)
    local flags = ""
    if ci_is_running() then
        flags = "-vD"
    end
    os.exec("xmake f" .. platform .. "--toolchain=" .. toolchain_name .. runtimes .. "-c --yes " .. policies .. " --foo=n" .. " " .. flags)
    local outdata
    outdata = os.iorun("xmake -r " .. flags)
    if outdata then
        if outdata:find("FOO") then
            raise("Modules dependency scanner update does not work\n%s", outdata)
        end
    end
end

function main(t)
    local clang_options = {compiler = "clang", version = clang_min_ver(), flags = {"--foo=y"}, after_build = _build}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver(), flags = {"--foo=y"}, after_build = _build}
    local msvc_options = {version = msvc_min_ver(), flags = {"--foo=y"}, after_build = _build}
    run_tests(clang_options, gcc_options, msvc_options)
end
