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

-- @usage
--
-- e.g. replace with lua pattern (default)
--
--   add_files("src/foo.c", {rules = "utils.replace", replaces = {{"RDKCONST%);", "VDKREG);"}}})
--
-- e.g. replace with plain text
--
--   add_files("src/foo.c", {rules = "utils.replace", replaces = {{"RDKCONST);", "VDKREG);"}}, replace_plain = true})
--
-- e.g. replace with custom function
--
--   add_files("src/foo.c", {rules = "utils.replace", replaces = function (content)
--       content = content:gsub("old", "new")
--       return content
--   end})
--
rule("utils.replace")
    on_prepare_file(function (target, sourcefile, opt)
        import("utils.progress")
        import("core.base.option")
        import("core.project.depend")

        -- get replace config from fileconfig
        local fileconfig = target:fileconfig(sourcefile)
        if not fileconfig or not fileconfig.replaces then
            return
        end
        local replaces = fileconfig.replaces
        local use_plain = fileconfig.replace_plain

        -- get the replaced file path
        local replacefile = target:autogenfile(sourcefile)

        -- add the original source directory as includedir,
        -- so that relative #include paths still work after replacing
        local sourcedir = path.directory(path.absolute(sourcefile))
        target:add("includedirs", sourcedir)

        -- replace the sourcefile in the build sourcebatch,
        -- so that the compiler uses the replaced file instead of the original
        for _, sourcebatch in pairs(target:sourcebatches()) do
            if sourcebatch.rulename ~= "utils.replace" then
                for i, sf in ipairs(sourcebatch.sourcefiles) do
                    if sf == sourcefile then
                        sourcebatch.sourcefiles[i] = replacefile
                    end
                end
            end
        end

        -- build values list from replace config for change detection
        local depvalues = {}
        local is_function = type(replaces) == "function"
        if not is_function then
            for _, item in ipairs(replaces) do
                table.insert(depvalues, item[1])
                table.insert(depvalues, item[2])
            end
            if use_plain then
                table.insert(depvalues, "plain")
            end
        end

        -- do replace if source file or replace config is changed
        depend.on_changed(function ()
            progress.show(opt.progress, "${color.build.object}replacing.$(mode) %s", sourcefile)
            vprint("replace %s to %s", sourcefile, replacefile)
            local content = io.readfile(sourcefile)
            if is_function then
                content = replaces(content)
            else
                if option.get("diagnosis") then
                    for _, item in ipairs(replaces) do
                        cprint("${dim}  > \"%s\" -> \"%s\"%s", item[1], item[2], use_plain and " (plain)" or "")
                    end
                end
                for _, item in ipairs(replaces) do
                    if use_plain then
                        content = content:replace(item[1], item[2], {plain = true})
                    else
                        content = content:gsub(item[1], item[2])
                    end
                end
            end
            io.writefile(replacefile, content)
        end, {dependfile = target:dependfile(replacefile),
              files = {sourcefile},
              values = depvalues,
              changed = target:is_rebuilt()})
    end)
