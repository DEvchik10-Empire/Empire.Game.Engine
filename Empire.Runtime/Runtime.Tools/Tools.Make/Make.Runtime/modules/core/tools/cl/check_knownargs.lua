--!A cross-platform build utility based on Lua
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- Copyright (C) 2015-present, Xmake Open Source Community.
--
-- @author      ruki
-- @file        check_knownargs.lua
--

-- imports
import("core.base.hashset")

function main(flags)
    local flag = flags[1]:gsub("/", "-")
    local known_flags = _g.known_flags
    if known_flags == nil then
        known_flags = hashset.from({
            "-Ox", "-O1", "-O2", "-Od",
            "-MT", "-MD", "-MTd", "-MDd",
            "-EHsc", "-EHa", "-EHs",
            "-W0", "-W1", "-W2", "-W3", "-W4", "-Wall", "-WX",
            "-Z7", "-Zi", "-ZI",
            "-c", "-nologo", "-FS", "-FC", "-Gm-",
            "-GR", "-GR-", "-GS", "-GS-",
            "-Gy", "-Gy-", "-GL",
            "-Gd", "-Gr", "-Gz", "-Gv",
            "-TC", "-TP",
            "-utf-8", "-bigobj", "-MP",
            "-showIncludes"
        })
        _g.known_flags = known_flags
    end
    if known_flags:has(flag) then
        return true
    end
    -- check flags with known prefixes (all MSVC versions)
    if flag:startswith("-wd") or   -- e.g. /wd4251
       flag:startswith("-we") or   -- e.g. /we4062
       flag:startswith("-wo") then
        return true
    end
end
