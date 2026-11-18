import("utils.binary.rpath")

function main(t)
    if not is_host("macosx") then
        return t:skip("wrong host platform")
    end

    os.execv("xmake", {"f", "-p", "iphoneos", "-a", "arm64", "--appledev=simulator", "-c"})
    os.execv("xmake", {"-vD"})
    local appfile = "build/iphoneos/arm64/release/demo.app/demo"
    local rpaths = rpath.list(appfile)
    local found_ios = false
    local found_macos = false
    if rpaths then
        for _, p in ipairs(rpaths) do
            if p == "@executable_path/Frameworks" then
                found_ios = true
            end
            if p == "@executable_path/../Frameworks" then
                found_macos = true
            end
        end
    end
    if not found_ios then
        raise("missing iOS framework rpath @executable_path/Frameworks")
    end
    if found_macos then
        raise("found macOS-style framework rpath in iOS app")
    end
end
