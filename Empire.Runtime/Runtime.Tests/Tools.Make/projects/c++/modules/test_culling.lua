inherit("test_base")

function main(_)
    local check_outdata = {str = "culled", format_string = "Modules culling does not work"}
    local clang_options = {compiler = "clang", version = clang_min_ver(), check_outdata = check_outdata}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver(), check_outdata = check_outdata}
    local msvc_options = {version = msvc_min_ver(), check_outdata = check_outdata}
    run_tests(clang_options, gcc_options, msvc_options)
end
