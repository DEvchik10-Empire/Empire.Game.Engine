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
-- @file        xmake.lua
--

-- define module: xmake
local xmake = xmake or {}

-- load modules
local semver = require("base/semver")

-- get xmake program name
--
-- @return      the name string, e.g. "xmake"
--
function xmake.name()
    return xmake._NAME or "xmake"
end

-- get xmake version
--
-- @return      the semver version object, e.g. xmake.version():ge("3.0.0")
--
function xmake.version()
    if xmake._VERSION_CACHE == nil then
        xmake._VERSION_CACHE = semver.new(xmake._VERSION) or false
    end
    return xmake._VERSION_CACHE or nil
end

-- get the xmake binary architecture
--
-- @return      the architecture string, e.g. "x86_64", "arm64"
--
function xmake.arch()
    return xmake._XMAKE_ARCH
end

-- get the git branch and commit of xmake version
--
-- @return      the branch string and commit hash
--
function xmake.branch()
    return xmake.version():build()[1]
end

-- get the xmake program scripts directory
--
-- @return      the program directory path
--
function xmake.programdir()
    return xmake._PROGRAM_DIR
end

-- get the xmake program binary file path
--
-- @return      the program file path
--
function xmake.programfile()
    return xmake._PROGRAM_FILE
end

-- is using LuaJIT runtime?
--
-- @return      true if LuaJIT
--
function xmake.luajit()
    return xmake._LUAJIT
end

-- is embedded via libxmake (xmake.cli)?
--
-- @return      true if embedded
--
function xmake.is_embed()
    return xmake._EMBED or false
end

-- is running in the main thread?
--
-- @return      true if in main thread
--
function xmake.in_main_thread()
    return xmake._THREAD_CALLBACK == nil
end

-- get the command line arguments
--
-- @return      the arguments array
--
function xmake.argv()
    return xmake._ARGV
end

-- return module: xmake
return xmake
