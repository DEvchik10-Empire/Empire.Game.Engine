inherit(".test_base")
import("utils.ci.is_running", {alias = "ci_is_running"})

function _build(check_outdata)
    local flags = ""
    if ci_is_running() then
        flags = "-vD"
    end
    os.run("xmake -r " .. flags)
    os.run("xmake b -r " .. flags .. " module_test1")
    os.run("xmake install " .. flags .. " --installdir=out")
end

function main(_)
    local clang_options = {compiler = "clang", version = clang_min_ver(), build = _build}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver(), build = _build}
    local msvc_options = {version = msvc_min_ver(), build = _build}
    run_tests(clang_options, gcc_options, msvc_options)
end
