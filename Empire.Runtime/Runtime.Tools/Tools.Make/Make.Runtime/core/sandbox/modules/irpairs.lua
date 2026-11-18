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
-- @file        irpairs.lua
--

-- load modules
local table = require("base/table")

-- irpairs
--
-- e.g.
--
-- @code
--
-- for idx, val in irpairs({"a", "b", "c", "d", "e", "f"}) do
--      print("%d %s", idx, val)
-- end
--
-- for idx, val in irpairs({"a", "b", "c", "d", "e", "f"}, function (v) return v:upper() end) do
--      print("%d %s", idx, val)
-- end
--
-- for idx, val in irpairs({"a", "b", "c", "d", "e", "f"}, function (v, a, b) return v:upper() .. a .. b end, "a", "b") do
--      print("%d %s", idx, val)
-- end
--
-- @endcode
--
-- Implemented as a stateful closure so the loop body can safely
-- reassign the first loop variable under lua 5.4+ (paired with the
-- RDKCONST->VDKREG compile-time patch in core/src/lua/xmake.lua).
function sandbox_irpairs(t, filter, ...)

    -- has filter?
    local has_filter = type(filter) == "function"

    -- stateful closure: keep the index in an upvalue so the loop body
    -- can safely reassign the first loop variable (lua 5.4+ merges the
    -- for-in control slot with the first user variable).
    local args = table.pack(...)
    t = table.wrap(t)
    local i = table.getn(t) + 1
    return function ()
        i = i - 1
        local v = t[i]
        if v ~= nil then
            if has_filter then
                v = filter(v, table.unpack(args, 1, args.n))
            end
            return i, v
        end
    end
end

-- load module
return sandbox_irpairs

