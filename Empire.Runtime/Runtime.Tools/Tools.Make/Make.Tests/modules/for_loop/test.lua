-- Regression tests for the Lua 5.4+ for-loop hazard: the parser merges
-- the generic-for control slot with the user's first loop variable and
-- marks it `RDKCONST`, so code like
--
--     for k, v in pairs(t) do
--         k = k:gsub("_", "-")
--         ...
--     end
--
-- normally fails to compile with "attempt to assign to const variable",
-- and if we only relax the const check the runtime silently corrupts the
-- iterator state (next iteration gets the modified key and `next` bails
-- with "invalid key to 'next'" — or worse, silently skips entries).
--
-- xmake works around this in two coordinated places:
--
--   1. Compile-time: `core/src/lua/xmake.lua` (and xmake.sh) replace
--      `RDKCONST);` with `VDKREG);` in lparser.c so assignment to the
--      first loop variable is allowed through.
--
--   2. Runtime: the sandbox iterators (`pairs`, `ipairs`, `irpairs`) and
--      the base containers (`list:items/ritems`, `hashset:items/keys`)
--      are implemented as stateful closures keeping their cursor in an
--      upvalue, so user writes to the first loop variable cannot reach
--      the iterator's internal state.
--
-- These tests pin both behaviors: that reassigning the first loop
-- variable (a) compiles and (b) produces correct, non-lossy iteration.

import("core.base.list")
import("core.base.hashset")

function test_numeric_for_reassign(t)
    -- The point of this test is purely compile-time: without the
    -- RDKCONST->VDKREG replace in core/src/lua/xmake.lua the write to
    -- `i` would raise "attempt to assign to const variable" and this
    -- file wouldn't even parse.
    --
    -- We do NOT assert on the value of `i` during iteration. Lua 5.4
    -- merges the numeric-for control slot with the user's first loop
    -- variable while 5.5 splits them, so the observable sequence of
    -- `i` values after a reassignment differs between versions. The
    -- iteration count lives in its own slot and stays stable, so that
    -- is what we pin.
    local n = 0
    for i = 1, 5 do
        i = i * 10
        n = n + 1
    end
    t:require(n == 5)
end

function test_generic_for_reassign_key(t)
    local tbl = {foo_a = 1, foo_b = 2, foo_c = 3}
    local seen = {}
    local count = 0
    for k, v in pairs(tbl) do
        k = k:gsub("_", "-")
        seen[k] = v
        count = count + 1
    end
    t:require(count == 3)
    t:require(seen["foo-a"] == 1)
    t:require(seen["foo-b"] == 2)
    t:require(seen["foo-c"] == 3)
end

function test_generic_for_reassign_many_keys(t)
    -- Stress the iterator with enough keys that a broken stateless
    -- iterator would deterministically trip `next` on the second round.
    local tbl = {}
    for i = 1, 64 do
        tbl["key_" .. i] = i
    end
    local total = 0
    for name, value in pairs(tbl) do
        name = name:gsub("_", "-")  -- would corrupt `next`'s key arg
        t:require(name:find("^key%-%d+$") ~= nil)
        total = total + value
    end
    t:require(total == (1 + 64) * 64 / 2)
end

function test_list_items_reassign(t)
    local l = list.new()
    for i = 1, 5 do
        l:push({name = "n" .. i})
    end
    for item in l:items() do
        item = nil  -- would corrupt list:next on the next iteration
    end
    local names = {}
    for item in l:items() do
        table.insert(names, item.name)
    end
    t:require(#names == 5)
    t:require(names[1] == "n1" and names[5] == "n5")
end

function test_irpairs_reassign_index(t)
    local arr = {"a", "b", "c", "d", "e"}
    local collected = {}
    for i, v in irpairs(arr) do
        i = -1  -- would corrupt the index on the next iteration
        table.insert(collected, v)
    end
    t:require(#collected == 5)
    t:require(collected[1] == "e")
    t:require(collected[5] == "a")
end

function test_hashset_items_reassign(t)
    local set = hashset.from({"key_1", "key_2", "key_3", "key_4", "key_5"})
    local seen = {}
    for item in set:items() do
        item = item:gsub("_", "-")  -- would corrupt `next`'s key arg
        seen[item] = true
    end
    local count = 0
    for _ in pairs(seen) do count = count + 1 end
    t:require(count == 5)
    for i = 1, 5 do
        t:require(seen["key-" .. i] == true)
    end
end

function test_hashset_items_skip_nil_member(t)
    -- A nil member lives in hashset under the `_NIL` sentinel; iteration
    -- must skip it rather than terminate, so real entries after it are
    -- still visited regardless of `next`'s order.
    local set = hashset.new()
    set:insert("a")
    set:insert(nil)
    set:insert("b")
    set:insert("c")
    local seen = {}
    for item in set:items() do
        seen[item] = true
    end
    local count = 0
    for _ in pairs(seen) do count = count + 1 end
    t:require(count == 3)
    t:require(seen.a and seen.b and seen.c)
end

function test_hashset_orderitems_skip_nil_member(t)
    -- use numeric members so the existing orderitems sort (which coerces
    -- `_NIL` to math.inf) stays within one comparable type.
    local set = hashset.new()
    set:insert(1)
    set:insert(nil)
    set:insert(2)
    set:insert(3)
    local collected = {}
    for item in set:orderitems() do
        table.insert(collected, item)
    end
    t:require(#collected == 3)
    t:require(collected[1] == 1 and collected[2] == 2 and collected[3] == 3)
end

function test_ipairs_reassign_index(t)
    -- With the stock Lua 5.4 `ipairs`, writing to the first loop variable
    -- would silently shift the index on the next iteration (no error,
    -- just wrong results). Sandbox `ipairs` hides the counter in an
    -- upvalue so the body's write is harmless.
    local list = {10, 20, 30, 40, 50}
    local seen = {}
    for i, v in ipairs(list) do
        i = -1  -- would corrupt iteration if `i` were the control slot
        table.insert(seen, v)
    end
    t:require(#seen == 5)
    for idx = 1, 5 do
        t:require(seen[idx] == list[idx])
    end
end

function test_generic_for_reassign_value(t)
    -- Writing to the second loop variable is always safe (it isn't the
    -- iterator control), but exercise it anyway to pin the behavior.
    local tbl = {a = 1, b = 2, c = 3}
    local total = 0
    for _, v in pairs(tbl) do
        v = v * 2
        total = total + v
    end
    t:require(total == 12)
end
