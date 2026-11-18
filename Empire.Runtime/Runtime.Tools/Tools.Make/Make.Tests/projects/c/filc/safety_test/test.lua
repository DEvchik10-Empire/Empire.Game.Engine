import("utils.ci.is_running", {alias = "ci_is_running"})

function main(t)
    -- filc only supports Linux x86_64
    if os.host() ~= "linux" or os.arch() ~= "x86_64" then
        return t:skip("filc is only supported on linux/x86_64")
    end

    local flags = ci_is_running() and "-vD" or ""

    -- configure and install the filc package; skip if unavailable
    local configured = try
    {
        function ()
            os.exec("xmake f -c -y " .. flags)
            return true
        end,
        catch { function () end }
    }
    if not configured then
        return t:skip("filc package not available or install failed")
    end

    os.exec("xmake -r " .. flags)

    -- find the compiled binary
    local binfile = path.join("build", os.host(), os.arch(), "release", "safety_test")
    assert(os.isfile(binfile), "binary not found: " .. binfile)

    -- run the binary — it MUST exit non-zero and emit a filc safety error
    -- os.iorunv raises on non-zero exit in the sandbox, so catch the failure
    local filc_output = nil
    try
    {
        function ()
            os.iorunv(binfile, {})
        end,
        catch
        {
            function (errors)
                filc_output = tostring(errors)
            end
        }
    }
    assert(filc_output, "expected safety_test to exit non-zero, but it succeeded")
    assert(filc_output:find("filc safety error", 1, true),
        "expected 'filc safety error' in output, got:\n" .. filc_output)
    assert(filc_output:find("filc panic", 1, true),
        "expected 'filc panic' in output, got:\n" .. filc_output)

    cprint("${green}filc safety check triggered as expected${reset}")
end
