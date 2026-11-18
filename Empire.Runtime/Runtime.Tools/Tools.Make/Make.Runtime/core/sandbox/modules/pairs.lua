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
-- @file        pairs.lua
--

-- load modules
local table = require("base/table")

-- improve pairs, wrap nil/single value
--
-- Unlike the stock lua `pairs`, this sandbox version tolerates the loop
-- body reassigning the first loop variable, e.g.:
--
--   for k, v in pairs(t) do
--       k = k:gsub("_", "-")   -- safe here
--       ...
--   end
--
-- This works together with the `RDKCONST -> VDKREG` compile-time patch
-- applied to lparser.c in core/src/lua/xmake.lua (and xmake.sh): that
-- patch lifts lua 5.4+'s ban on writing to the for-in control variable,
-- and this stateful closure makes such writes harmless at runtime.
function sandbox_pairs(t)

    -- exists the custom ipairs?
    local is_table = type(t) == "table"
    if is_table and t.pairs then
        return t:pairs()
    end

    -- wrap table and return iterator
    if not is_table then
        t = t ~= nil and {t} or {}
    end
    -- keep `next`'s key in an upvalue so the loop body can safely reassign
    -- the first loop variable. In lua 5.4+ the for-in control slot is
    -- merged with the first user variable; if we threaded the key through
    -- the loop it would corrupt `next` on the following iteration.
    local k = nil
    return function ()
        local nk, nv = next(t, k)
        k = nk
        return nk, nv
    end
end

-- load module
return sandbox_pairs

