function main(t)
    if is_host("macosx") and os.arch() ~= "arm64" then
        t:build()
        local targetfile = os.files("build/**/release/test")[1]
        assert(targetfile and os.isfile(targetfile), "target file not found!")
        local mtime = os.mtime(targetfile)
        os.sleep(1000)
        os.touch("src/foo.rs", {mtime = os.time()})
        t:build()
        assert(os.mtime(targetfile) > mtime, "target file should be rebuilt after rust dependency changes!")
    else
        return t:skip("wrong host platform")
    end
end
