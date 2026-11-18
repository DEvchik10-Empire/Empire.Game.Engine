inherit("test_base")
import("utils.ci.is_running", {alias = "ci_is_running"})

function run_xmake_test(...)
    local flags = ""
    if ci_is_running() then
        flags = "-vD"
    end
    local outdata, errdata = os.iorun("xmake test " .. flags)
    assert(outdata, errdata)
end

function main(t)
    local clang_options = {compiler = "clang", version = clang_min_ver(), after_build = run_xmake_test}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver(), after_build = run_xmake_test}
    local msvc_options = {version = msvc_min_ver(), after_build = run_xmake_test}
    run_tests(clang_options, gcc_options, msvc_options)
end
