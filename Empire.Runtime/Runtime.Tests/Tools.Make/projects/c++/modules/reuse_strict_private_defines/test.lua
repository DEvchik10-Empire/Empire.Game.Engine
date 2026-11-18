inherit(".test_base")
import("utils.ci.is_running", {alias = "ci_is_running"})

local PRIVATE_DEFINE = "PRIVATE_DEP_DEFINE_DO_NOT_PROPAGATE"
local PUBLIC_SYSINCLUDEDIR = path.translate("src/include")

function _build()
    local flags = ""
    if ci_is_running() then
        flags = "-vD"
    end
    local leaked = false
    local missing_sysincludedir = true
    local outdata = try { function () return os.iorun("xmake -rv") end }
    if outdata then
        for line in outdata:gmatch("[^\r\n]+") do
            if line:find("Consumer", 1, true) then
                if line:find(PRIVATE_DEFINE, 1, true) then
                    leaked = true
                end
                if line:find(PUBLIC_SYSINCLUDEDIR, 1, true) then
                    missing_sysincludedir = false
                end
            end
        end
        if leaked then
            raise("Private dependency defines leaked into Consumer module rebuilds under reuse.strict\n%s", outdata)
        end
        if missing_sysincludedir then
            raise("Missing public sysincludedir in Consumer module rebuilds under reuse.strict\n%s", outdata)
        end
    else
        -- maybe build failed, we need to see verbose errors
        os.run("xmake " .. flags)
    end
end

function main(_)
    local clang_options = {compiler = "clang", version = clang_min_ver(), build = _build}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver(), build = _build}
    local msvc_options = {version = msvc_min_ver(), build = _build}
    run_tests(clang_options, gcc_options, msvc_options)
end
