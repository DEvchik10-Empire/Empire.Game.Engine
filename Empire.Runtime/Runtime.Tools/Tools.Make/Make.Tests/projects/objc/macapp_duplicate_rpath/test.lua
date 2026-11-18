function main(t)
    if not is_host("macosx") then
        return t:skip("wrong host platform")
    end

    os.execv("xmake", {"f", "-p", "macosx", "-a", os.arch(), "-c"})
    os.execv("xmake", {"-vD"})
end
