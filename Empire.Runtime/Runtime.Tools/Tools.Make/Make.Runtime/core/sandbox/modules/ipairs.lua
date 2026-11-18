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
-- @file        ipairs.lua
--

-- load modules
local table = require("base/table")

-- improve ipairs, wrap nil and single value
--
-- Like sandbox `pairs`, this is a stateful closure so the loop body can
-- safely reassign the first loop variable under lua 5.4+ (paired with
-- the RDKCONST->VDKREG compile-time patch in core/src/lua/xmake.lua).
function sandbox_ipairs(t)

    -- exists the custom ipairs?
    local is_table = type(t) == "table"
    if is_table and t.ipairs then
        return t:ipairs()
    end

    -- wrap table and return iterator
    if not is_table then
        t = t ~= nil and {t} or {}
    end
    -- keep the index in an upvalue so the loop body can safely reassign
    -- the first loop variable. In lua 5.4+ the for-in control slot is
    -- merged with the first user variable; writes to it would otherwise
    -- silently skip or repeat entries on the next iteration.
    local i = 0
    return function ()
        i = i + 1
        local v = t[i]
        if v ~= nil then
            return i, v
        end
    end
end


-- load module
return sandbox_ipairs

