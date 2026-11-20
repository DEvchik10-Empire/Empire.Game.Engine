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
-- @file        icx.lua
--

-- https://github.com/xmake-io/xmake/issues/7438
-- https://www.intel.com/content/www/us/en/docs/dpcpp-cpp-compiler/get-started-guide/2024-1/get-started-on-windows.html
if is_host("windows") then
    inherit("clang_cl")
else
    inherit("clang")
end
