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
-- @file        depgraph.lua
--

-- imports
import("core.base.task")
import("core.base.option")
import("core.base.json")
import("core.project.project")
import("private.action.require.impl.package")
import("private.action.require.impl.repository")
import("private.action.require.impl.environment")
import("private.action.require.impl.utils.get_requires")

-- collect single package entry with its direct dependencies
function _collect_package_entry(instance)
    local deps = {}
    local plaindeps = instance:plaindeps()
    if plaindeps then
        for _, dep in ipairs(plaindeps) do
            table.insert(deps, dep:fullname())
        end
    end
    json.mark_as_array(deps)
    return {
        name = instance:fullname(),
        version = instance:version_str(),
        configs_str = package.get_configs_str(instance),
        deps = deps
    }
end

-- collect the full dependency graph from loaded package instances
--
-- returns a table with:
--   root_packages: packages that are not depended on by others
--   packages: all package entries with their direct deps
--
function _collect_package_graph(instances)
    local targets = {}
    local roots = {}
    local all_deps = {}
    for _, instance in ipairs(instances) do
        local entry = _collect_package_entry(instance)
        table.insert(targets, entry)
        for _, dep in ipairs(entry.deps) do
            all_deps[dep] = true
        end
    end
    for _, entry in ipairs(targets) do
        if not all_deps[entry.name] then
            table.insert(roots, entry.name)
        end
    end
    json.mark_as_array(roots)
    return {
        root_packages = roots,
        packages = targets
    }
end

-- format version and configs as a suffix string for plain output
-- e.g. " v1.3.2 [shared:y, debug:n]"
function _format_package_suffix(entry)
    local parts = {}
    if entry.version then
        table.insert(parts, entry.version)
    end
    if entry.configs_str and #entry.configs_str > 0 then
        table.insert(parts, entry.configs_str)
    end
    if #parts > 0 then
        return " " .. table.concat(parts, " ")
    end
    return ""
end

-- print dependency tree recursively
--
-- e.g.
--   libpng v1.6.56
--   \-- zlib v1.3.2
--
-- already expanded subtrees are marked with (*) to avoid duplication
--
function _print_dep_tree(packages_map, name, prefix, expanded)
    expanded[name] = true
    local entry = packages_map[name]
    local deps = entry and entry.deps or {}
    for i, dep in ipairs(deps) do
        local is_last = (i == #deps)
        local connector = is_last and "\\-- " or "|-- "
        local next_prefix = prefix .. (is_last and "    " or "|   ")
        local dep_entry = packages_map[dep]
        local suffix = dep_entry and _format_package_suffix(dep_entry) or ""
        if expanded[dep] then
            -- already expanded, show with (*) and list direct children as references
            local dep_deps = dep_entry and dep_entry.deps or {}
            if #dep_deps > 0 then
                cprint("%s%s${color.dump.reference}%s${clear}${dim}%s${clear}", prefix, connector, dep, suffix)
                for j, subdep in ipairs(dep_deps) do
                    local sub_is_last = (j == #dep_deps)
                    local sub_connector = sub_is_last and "\\-- " or "|-- "
                    local sub_entry = packages_map[subdep]
                    local sub_suffix = sub_entry and _format_package_suffix(sub_entry) or ""
                    cprint("%s%s${dim}%s%s (*)${clear}", next_prefix, sub_connector, subdep, sub_suffix)
                end
            else
                cprint("%s%s${color.dump.reference}%s${clear}${dim}%s (*)${clear}", prefix, connector, dep, suffix)
            end
        else
            cprint("%s%s${color.dump.reference}%s${clear}${dim}%s${clear}", prefix, connector, dep, suffix)
            _print_dep_tree(packages_map, dep, next_prefix, expanded)
        end
    end
end

-- print the package dependency graph as a tree
function _print_package_graph(graph)
    local packages_map = {}
    for _, pkg in ipairs(graph.packages) do
        packages_map[pkg.name] = pkg
    end
    local expanded = {}
    for _, root in ipairs(graph.root_packages) do
        local entry = packages_map[root]
        local suffix = entry and _format_package_suffix(entry) or ""
        cprint("${color.dump.string}%s${clear}${dim}%s${clear}", root, suffix)
        _print_dep_tree(packages_map, root, "", expanded)
    end
end

-- print the package dependency graph in graphviz DOT format
--
-- e.g.
--   digraph {
--       "zlib"
--       "libpng" -> "zlib"
--   }
--
function _print_dot_graph(graph)
    print("digraph {")
    for _, pkg in ipairs(graph.packages) do
        if #pkg.deps == 0 then
            print(string.format("    \"%s\"", pkg.name))
        else
            for _, dep in ipairs(pkg.deps) do
                print(string.format("    \"%s\" -> \"%s\"", pkg.name, dep))
            end
        end
    end
    print("}")
end

-- show the given package dependency graph
--
-- supported output formats (via --format):
--   plain (default): ASCII tree view
--   json: structured JSON output
--   dot:  graphviz DOT format
--
function main(requires_raw)

    -- get requires and extra config
    local requires, requires_extra = get_requires(requires_raw)
    if not requires or #requires == 0 then
        return
    end

    -- enter environment
    environment.enter()

    -- pull all repositories first if not exists
    if not repository.pulled() then
        task.run("repo", {update = true})
    end

    -- load all packages and collect dependency graph
    local instances = package.load_packages(requires, {requires_extra = requires_extra})
    local graph = _collect_package_graph(instances)

    -- output in the specified format
    local format = option.get("format") or "plain"
    if format == "json" then
        -- strip configs_str, it's only for plain display
        for _, pkg in ipairs(graph.packages) do
            pkg.configs_str = nil
        end
        print(json.encode(graph, {pretty = true, orderkeys = true}))
    elseif format == "dot" then
        _print_dot_graph(graph)
    else
        _print_package_graph(graph)
    end

    -- leave environment
    environment.leave()
end
