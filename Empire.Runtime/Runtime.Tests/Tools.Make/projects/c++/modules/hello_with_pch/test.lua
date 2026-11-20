inherit(".test_base")

function main(_)
    -- clang-cl doesn't support mixing pch and C++ module atm
    local clang_options = {compiler = "clang", version = clang_min_ver(), disable_clang_cl = true}
    local gcc_options = {compiler = "gcc", version = gcc_min_ver()}
    local msvc_options = {version = msvc_min_ver()}
    run_tests(clang_options, gcc_options, msvc_options)
end
