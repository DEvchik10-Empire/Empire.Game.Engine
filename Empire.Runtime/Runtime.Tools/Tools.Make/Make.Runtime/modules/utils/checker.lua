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
-- @file        checker.lua
--

-- imports
import("core.cache.memcache")

-- is the given checker running?
--
-- @param name      the checker name, e.g. "syntax", "clang.tidy"
--
-- @usage
--
--   import("utils.checker")
--   if checker.is_running("syntax") then
--       -- we are in `xmake check syntax` mode
--   end
--
function is_running(name)
    return memcache.get("__checker_running", name) or false
end
