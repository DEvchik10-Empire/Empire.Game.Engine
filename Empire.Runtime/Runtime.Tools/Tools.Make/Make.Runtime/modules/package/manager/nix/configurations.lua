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
-- @author      ZZBaron
-- @file        configurations.lua
--

-- get configurations
--
-- Source priority order (highest to lowest):
--   1. nix_shell   - nix-shell / nix develop / stdenv.mkDerivation environment
--   2. homemanager - home-manager profile (/etc/profiles/per-user/$USER)
--   3. nix_profile - nix profile / nix-env (~/.nix-profile)
--   4. nixos_system - NixOS system profile (/run/current-system/sw)
--
-- Usage:
--   add_requires("nix::zlib")                                                              -- defaults to nix_shell
--   add_requires("nix::zlib", {configs = {source = "homemanager"}})
--   add_requires("nix::zlib", {configs = {source = "nix_shell|nix_profile|nixos_system"}}) -- multiple sources
--
function main()
    return {
        source = {description = "Nix source(s) to search packages from. Pipe-separated list of: nix_shell, homemanager, nix_profile, nixos_system.", default = "nix_shell"},
    }
end