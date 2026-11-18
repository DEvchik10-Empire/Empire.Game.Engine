inherit("test_base")
import("utils.ci.is_running", {alias = "ci_is_running"})

function _build()
    local flags = ""
    if ci_is_running() then
     flags = "-vD"
    end
    try {
        function ()
            os.run("xmake -r " .. flags)
        end,
        catch {
            function (errors)
                errors = tostring(errors)
                if not errors:find("duplicate module name detected", 1, true) then
                    raise("Modules duplicate name detection does not work\n%s", errors)
                end
            end
        }
    }
end

function main(_)
    local clang_options = {compiler = "clang", version = clang_min_ver(), build = _build}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver(), build = _build}
    local msvc_options = {version = msvc_min_ver(), build = _build}
    run_tests(clang_options, gcc_options, msvc_options)
end
