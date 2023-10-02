--[[
        This premake4.lua _requires_ windirstat/premake-stable to work properly.
        If you don't want to use the code-signed build that can be found in the
        ./common/ subfolder, you can build from the WDS-branch over at:

        https://sourceforge.net/projects/windirstat/

        Prebuilt, signed binaries are available from:
          https://github.com/windirstat/premake-stable
          https://sourceforge.net/projects/windirstat/files/premake-stable/
          https://osdn.net/projects/windirstat/storage/historical/premake-stable/

// SPDX-License-Identifier: Unlicense
  ]]
local action = _ACTION or ""
local release = false
local tgtname = "lsads"
newoption { trigger = "release", description = "Creates a solution suitable for a release build." }
newoption { trigger = "msvcrt", description = "Uses the Vista/2008 WDK to link dynamically against msvcrt.dll." }
newoption { trigger = "xp", description = "Enable XP-compatible build for newer Visual Studio versions." }
if _OPTIONS["release"] then
    print "INFO: Creating release build solution."
    release = true
    _OPTIONS["xp"] = ""
    _OPTIONS["msvcrt"] = ""
end
if _OPTIONS["msvcrt"] then
    wdkbasedir = nil
    for k,v in ipairs({"WLHBASE", "WNETBASE", "W7BASE"}) do
        local basedir = os.getenv(v)
        print("BASEDIR = " .. (basedir or "<null>") .. " for environment variable: " .. v)
        if basedir ~= nil and os.isdir(basedir) then
            local basedirvar = "$(" .. v .. ")"
            print("Using value of " .. basedirvar .. "=" .. basedir .. " as WDK location to find msvcrt import library.")
            wdkbasedir = basedirvar
            break
        end
    end
    if not wdkbasedir then
        print("ERROR: None of the eligible environment variables for the base directory was set" .. (wdkbasedir or "<null>") .. "does not exist.")
        os.exit(1)
    end
end

solution (tgtname .. iif(release, "_release", ""))
    configurations  (iif(release, {"Release"}, {"Debug", "Release"}))
    platforms       (iif(_ACTION < "vs2005", {"x32"}, {"x32", "x64"}))
    location        ('.')

    project (tgtname .. iif(release, "_release", ""))
        uuid            ("86AFA312-9D89-4FAE-8201-B02492AA7D64")
        language        ("C++")
        kind            ("ConsoleApp")
        targetname      (tgtname)
        flags           {"Unicode", "NativeWChar", "ExtraWarnings", "WinMain", "NoRTTI",}
        targetdir       (iif(release, tgtname .. "_release", "bin"))
        libdirs         {"$(IntDir)"}
        links           {"version"}
        linkoptions     {"/delay:nobind","/delayload:version.dll"}
        resoptions      {"/nologo", "/l409"}
        defines         {"_CONSOLE", "WIN32", "_WINDOWS", "STRICT"}
        if not _OPTIONS["msvcrt"] then
            flags       {"StaticRuntime"}
        end

        excludes
        {
            "hgid.h",
            "Backup*.*",
            "*.vsprops",
        }

        files
        {
            "*.cpp",
            "*.h",
            "*.hpp",
            "*.mc",
            "*.rc",
            "*.manifest",
            "*.cmd", "*.txt", "*.md", "*.rst", "premake4.lua",
            "*.manifest", "*.props", "*.targets", "*.ruleset", ".editorconfig", ".clang-format",
            ".gitignore",
        }

        vpaths
        {
            ["Header Files/*"] = { "*.h", "*.hpp" },
            ["Source Files/*"] = { "*.cpp" },
            ["Resource Files/*"] = { "*.rc" },
            ["Special Files/*"] = { "**.cmd", "premake4.lua", "*.rst", "*.txt", },
        }

        configuration {"*"}
            prebuildcommands{"\"$(ProjectDir)\\hgid.cmd\"",}

        configuration {"Debug", "x32"}
            targetsuffix    ("32D")

        configuration {"Debug", "x64"}
            targetsuffix    ("64D")

        configuration {"Release", "x32"}
            targetsuffix    ("32")
            if _OPTIONS["msvcrt"] then
                links       {"comdlg32", wdkbasedir .. "\\lib\\crt\\i386\\msvcrt.lib", wdkbasedir .. "\\lib\\w2k\\i386\\strsafe.lib", wdkbasedir .. "\\lib\\w2k\\i386\\msvcrt_win2000.obj"}
                defines     {"USE_W2K_COMPAT"}
            end

        configuration {"Release", "x64"}
            targetsuffix    ("64")
            if _OPTIONS["msvcrt"] then
                links       {"comdlg32", wdkbasedir .. "\\lib\\crt\\amd64\\msvcrt.lib", wdkbasedir .. "\\lib\\wnet\\amd64\\strsafe.lib", wdkbasedir .. "\\lib\\wnet\\amd64\\msvcrt_win2003.obj"}
                defines     {"USE_W2K_COMPAT"}
            end

        configuration {"Debug"}
            defines         {"_DEBUG"}
            flags           {"Symbols",}

        configuration {"Release"}
            defines         {"NDEBUG"}
            flags           {"Optimize", "Symbols", "NoMinimalRebuild", "NoIncrementalLink", "NoEditAndContinue"}
            linkoptions     {"/release"}
            buildoptions    {"/Oi", "/Os", "/Gy"}

        configuration {"vs2002 or vs2003 or vs2005 or vs2008", "Release"}
            buildoptions    {"/Oy"}

        configuration {"vs2002 or vs2003 or vs2005 or vs2008 or vs2010", "Release", "x32"}
            linkoptions     {"/subsystem:console,5.00"}

        configuration {"vs2012 or vs2013 or vs2015 or vs2017", "Release", "x32"}
            linkoptions     {"/subsystem:console,5.01"}

        configuration {"Release", "x64"}
            linkoptions     {"/subsystem:console,5.02"}

        configuration {"vs2013 or vs2015 or vs2017"}
            defines         {"WINVER=0x0501", "_ALLOW_RTCc_IN_STL"}

        configuration {"vs2002 or vs2003 or vs2005 or vs2008 or vs2010 or vs2012", "x32"}
            defines         {"WINVER=0x0500"}

        configuration {"vs2005 or vs2008 or vs2010 or vs2012", "x64"}
            defines         {"WINVER=0x0501"}

        configuration {"vs2005 or vs2008", "Release"}
            linkoptions     {"/opt:nowin98"}

do
    -- Some tags we suppress as we override those from the project.props file
    local suppress_tags2 = {
        ['<OutDir>%s\\</OutDir>'] = 0,
        ['<IntDir>%s\\</IntDir>'] = 0,
        ['<TargetName>%s</TargetName>'] = 0,
        ['<TargetExt>%s</TargetExt>'] = 0,
        ['<PrecompiledHeader></PrecompiledHeader>'] = 0,
    }
    local suppress_tags3 = {
        ['<PrecompiledHeader></PrecompiledHeader>'] = 0,
        ['<MinimalRebuild>false</MinimalRebuild>'] = 0,
        ['<MinimalRebuild>true</MinimalRebuild>'] = 0,
        ['<Optimization>%s</Optimization>'] = 0,
        ['<BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>'] = 0,
        ['<RuntimeLibrary>%s</RuntimeLibrary>'] = 0,
        ['<FunctionLevelLinking>true</FunctionLevelLinking>'] = 0,
        ['<DebugInformationFormat>%s</DebugInformationFormat>'] = 0,
        ['<StringPooling>true</StringPooling>'] = 0,
        ['<OptimizeReferences>true</OptimizeReferences>'] = 0,
        ['<TargetMachine>%s</TargetMachine>'] = 0,
        ['<EnableCOMDATFolding>true</EnableCOMDATFolding>'] = 0,
        ['<OutputFile>$(OutDir)%s</OutputFile>'] = 0,
        ['<AdditionalLibraryDirectories>%s;%%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>'] = 0,
        ['<WarningLevel>Level3</WarningLevel>'] = 0,
        ['<SmallerTypeCheck>true</SmallerTypeCheck>'] = 0,
        ['<ImportLibrary>%s</ImportLibrary>'] = 0,
    }
    -- Embed the property sheet
    previousmsg = nil
    _G.override_vcxproj = function(prj, orig_p, indent, msg, first, ...)
        oldpreviousmsg = previousmsg
        previousmsg = msg
        if indent == 1 then
            if msg == [[<ImportGroup Label="ExtensionSettings">]] then
                orig_p(indent, msg, first, ...) -- pass through original line
                orig_p(indent, [[</ImportGroup>]])
                orig_p(indent, [[<ImportGroup Label="PropertySheets">]])
                orig_p(indent+1, [[<Import Project="$(SolutionDir)project.early.props" Condition="exists('$(SolutionDir)project.early.props')" Label="ProjectSpecific (solution/early)" />]])
                orig_p(indent+1, [[<Import Project="$(ProjectDir)project.early.props" Condition="exists('$(ProjectDir)project.early.props') AND '$(SolutionDir)' != '$(ProjectDir)'" Label="Project-specific (local/early)" />]])
                return true
            end
            if msg == [[<ItemGroup>]] and oldpreviousmsg == [[</ItemDefinitionGroup>]] then
                orig_p(indent, [[<ImportGroup Label="PropertySheets">]])
                orig_p(indent+1, [[<Import Project="$(SolutionDir)project.late.props" Condition="exists('$(SolutionDir)project.late.props')" Label="ProjectSpecific (solution/late)" />]])
                orig_p(indent+1, [[<Import Project="$(ProjectDir)project.late.props" Condition="exists('$(ProjectDir)project.late.props') AND '$(SolutionDir)' != '$(ProjectDir)'" Label="Project-specific (local/late)" />]])
                orig_p(indent, [[</ImportGroup>]])
                orig_p(indent, msg, first, ...) -- pass through original line
                return true
            end
            if msg == [[<ImportGroup Label="ExtensionTargets">]] then
                orig_p(indent, [[<ImportGroup Label="PropertySheets">]])
                orig_p(indent+1, [[<Import Project="$(SolutionDir)project.targets" Condition="exists('$(SolutionDir)project.targets')" Label="ProjectSpecific (solution/targets)" />]])
                orig_p(indent+1, [[<Import Project="$(ProjectDir)project.targets" Condition="exists('$(ProjectDir)project.targets') AND '$(SolutionDir)' != '$(ProjectDir)'" Label="Project-specific (local/targets)" />]])
                orig_p(indent, [[</ImportGroup>]])
                orig_p(indent, msg, first, ...) -- pass through original line
                return true
            end
        end
        if (indent == 2) and (msg == '<Keyword>Win32Proj</Keyword>') then
            orig_p(indent, msg, first, ...) -- pass through original line
            orig_p(indent, '<ProjectName>%s</ProjectName>', prj.name)
            return true
        end
        if (indent == 2) and (suppress_tags2[msg] ~= nil) then -- Suppress these, our property sheet takes care of those
            return true
        end
        if (indent == 3) and (suppress_tags3[msg] ~= nil) then -- Suppress these, our property sheet takes care of those
            return true
        end
    end
end

--[[
    This part of the premake4.lua modifies the core premake4 behavior a little.

    It does the following (in order of appearence below):

    - New option --sdkver to override <WindowsTargetPlatformVersion> on modern VS
    - New option --clang to request ClangCL toolset on modern VS
    - New option --xp to request XP-compatible toolset on modern VS
    - On older premake4 versions it will provide a premake.project.getbasename
      function, furthermore two other functions get patched to make use of it
    - premake.project.getbasename() gets overridden to insert a marker into the
      created file name, based on the chosen action
      Example: foobar.vcxproj becomes foobar.vs2022.vcxproj etc ...
      The purpose of this exercise is to allow for projects/solutions of several
      Visual Studio versions to reside in the same folder
    - Options "dotnet" gets removed
    - The "platform" option has some allowed values removed
    - The "os" option has some allowed values removed
    - The actions are trimmed to what we know can work
]]

newoption { trigger = "sdkver", value = "SDKVER", description = "Allows to override SDK version (VS2015 through VS2022)" }
newoption { trigger = "clang", description = "Allows to use clang-cl as compiler and lld-link as linker (VS2019 and VS2022)" }
newoption { trigger = "xp", description = "Allows to use a supported XP toolset for some VS versions" }

do
    -- This is mainly to support older premake4 builds
    if not premake.project.getbasename then
        print "Magic happens for old premake4 versions without premake.project.getbasename() ..."
        -- override the function to establish the behavior we'd get after patching Premake to have premake.project.getbasename
        premake.project.getbasename = function(prjname, pattern)
            return pattern:gsub("%%%%", prjname)
        end
        -- obviously we also need to overwrite the following to generate functioning VS solution files
        premake.vstudio.projectfile = function(prj)
            local pattern
            if prj.language == "C#" then
                pattern = "%%.csproj"
            else
                pattern = iif(_ACTION > "vs2008", "%%.vcxproj", "%%.vcproj")
            end

            local fname = premake.project.getbasename(prj.name, pattern)
            fname = path.join(prj.location, fname)
            return fname
        end
        -- we simply overwrite the original function on older Premake versions
        premake.project.getfilename = function(prj, pattern)
            local fname = premake.project.getbasename(prj.name, pattern)
            fname = path.join(prj.location, fname)
            return path.getrelative(os.getcwd(), fname)
        end
    end
    -- Make UUID generation for filters deterministic
    if os.str2uuid ~= nil then
        local vc2010 = premake.vstudio.vc2010
        vc2010.filteridgroup = function(prj)
            local filters = { }
            local filterfound = false

            for file in premake.project.eachfile(prj) do
                -- split the path into its component parts
                local folders = string.explode(file.vpath, "/", true)
                local path = ""
                for i = 1, #folders - 1 do
                    -- element is only written if there *are* filters
                    if not filterfound then
                        filterfound = true
                        _p(1,"<ItemGroup>")
                    end

                    path = path .. folders[i]

                    -- have I seen this path before?
                    if not filters[path] then
                        local seed = path .. (prj.uuid or "")
                        local deterministic_uuid = os.str2uuid(seed)
                        filters[path] = true
                        _p(2, '<Filter Include="%s">', path)
                        _p(3, "<UniqueIdentifier>{%s}</UniqueIdentifier>", deterministic_uuid)
                        _p(2, "</Filter>")
                    end

                    -- prepare for the next subfolder
                    path = path .. "\\"
                end
            end

            if filterfound then
                _p(1,"</ItemGroup>")
            end
        end
    end
    -- Name the project files after their VS version
    local orig_getbasename = premake.project.getbasename
    premake.project.getbasename = function(prjname, pattern)
        -- The below is used to insert the .vs(8|9|10|11|12|14|15|16|17) into the file names for projects and solutions
        if _ACTION then
            name_map = {vs2005 = "vs8", vs2008 = "vs9", vs2010 = "vs10", vs2012 = "vs11", vs2013 = "vs12", vs2015 = "vs14", vs2017 = "vs15", vs2019 = "vs16", vs2022 = "vs17"}
            if name_map[_ACTION] then
                pattern = pattern:gsub("%%%%", "%%%%." .. name_map[_ACTION])
            else
                pattern = pattern:gsub("%%%%", "%%%%." .. _ACTION)
            end
        end
        return orig_getbasename(prjname, pattern)
    end
    -- Premake4 sets the PDB file name for the compiler's PDB to the default
    -- value used by the linker's PDB. This causes error C1052 on VS2017. Fix it.
    -- But this also fixes up certain other areas of the generated project. The idea
    -- here is to catch the original _p() invocations, evaluate the arguments and
    -- then act based on those, using orig_p() as a standin during a call to the
    -- underlying premake.vs2010_vcxproj() function ;-)
    local orig_premake_vs2010_vcxproj = premake.vs2010_vcxproj
    premake.vs2010_vcxproj = function(prj)
        -- The whole stunt below is necessary in order to modify the resource_compile()
        -- output. Given it's a local function we have to go through hoops.
        local orig_p = _G._p
        local besilent = false
        -- We patch the global _p() function
        _G._p = function(indent, msg, first, ...)
            -- Look for non-empty messages and narrow it down by the indent values
            if msg ~= nil then
                -- Allow this logic to be hooked and the hook to preempt any action hardcoded below
                if (_G.override_vcxproj ~= nil) and (type(_G.override_vcxproj) == 'function') then
                    if _G.override_vcxproj(prj, orig_p, indent, msg, first, ...) then
                        return -- suppress further output
                    end
                end
                if msg:match("<ProgramDataBaseFileName>[^<]+</ProgramDataBaseFileName>") then
                    return -- we want to suppress these
                end
                if indent == 2 then
                    if msg == "<RootNamespace>%s</RootNamespace>" then
                        local sdkmap = {vs2015 = "8.1", vs2017 = "10.0.17763.0", vs2019 = "10.0", vs2022 = "10.0"}
                        if (not _ACTION) or (not sdkmap[_ACTION]) then -- should not happen, but tread carefully anyway
                            orig_p(indent, msg, first, ...) -- what was originally supposed to be output
                            return
                        end
                        local sdkver = _OPTIONS["sdkver"] or sdkmap[_ACTION]
                        orig_p(indent, msg, first, ...) -- what was originally supposed to be output
                        orig_p(indent, "<WindowsTargetPlatformVersion>%s</WindowsTargetPlatformVersion>", sdkver)
                        return
                    end
                    if msg == "<PlatformToolset>%s</PlatformToolset>" then
                        if (_OPTIONS["clang"] ~= nil) and (_ACTION == "vs2017") then
                            if _OPTIONS["xp"] ~= nil then
                                print "WARNING: The --clang option takes precedence over --xp, therefore picking v141_clang_c2 toolset."
                            end
                            print "WARNING: If you are used to Clang support from VS2019 and newer, be sure to review your choice. It's not the same on older VS versions."
                            orig_p(indent, msg, "v141_clang_c2")
                            return
                        elseif (_OPTIONS["clang"] ~= nil) and (_ACTION >= "vs2019") then
                            if _OPTIONS["xp"] ~= nil then
                                print "WARNING: The --clang option takes precedence over --xp, therefore picking ClangCL toolset."
                            end
                            orig_p(indent, msg, "ClangCL")
                            return
                        elseif _OPTIONS["xp"] ~= nil then
                            local toolsets = { vs2012 = "v110", vs2013 = "v120", vs2015 = "v140", vs2017 = "v141", vs2019 = "v142", vs2022 = "v143" }
                            local toolset = toolsets[_ACTION]
                            if toolset then
                                if _OPTIONS["xp"] and toolset >= "v141" then
                                    toolset = "v141" -- everything falls back to the VS2017 XP toolset for more recent VS
                                end
                                orig_p(indent,"<PlatformToolset>%s_xp</PlatformToolset>", toolset)
                                return
                            end
                        end
                    end
                elseif indent == 3 then
                    -- This is what vanilla VS would output it as, so let's try to align with that
                    if msg == "<PrecompiledHeader></PrecompiledHeader>" then
                        orig_p(indent, "<PrecompiledHeader>")
                        orig_p(indent, "</PrecompiledHeader>")
                        return
                    end
                end
            end
            if not besilent then -- should we be silent (i.e. suppress default output)?
                orig_p(indent, msg, first, ...)
            end
        end
        orig_premake_vs2010_vcxproj(prj)
        _G._p = orig_p -- restore in any case
    end
    -- ... same as above but for VS200x this time
    local function wrap_remove_pdb_attribute(origfunc)
        local fct = function(cfg)
            local old_captured = io.captured -- save io.captured state
            io.capture() -- this sets io.captured = ""
            origfunc(cfg)
            local captured = io.endcapture()
            assert(captured ~= nil)
            captured = captured:gsub('%s+ProgramDataBaseFileName=\"[^"]+\"', "")
            if old_captured ~= nil then
                io.captured = old_captured .. captured -- restore outer captured state, if any
            else
                io.write(captured)
            end
        end
        return fct
    end
    premake.vstudio.vc200x.VCLinkerTool = wrap_remove_pdb_attribute(premake.vstudio.vc200x.VCLinkerTool)
    premake.vstudio.vc200x.toolmap.VCLinkerTool = premake.vstudio.vc200x.VCLinkerTool -- this is important as well
    premake.vstudio.vc200x.VCCLCompilerTool = wrap_remove_pdb_attribute(premake.vstudio.vc200x.VCCLCompilerTool)
    premake.vstudio.vc200x.toolmap.VCCLCompilerTool = premake.vstudio.vc200x.VCCLCompilerTool -- this is important as well
    -- Override the object directory paths ... don't make them "unique" inside premake4
    local orig_gettarget = premake.gettarget
    premake.gettarget = function(cfg, direction, pathstyle, namestyle, system)
        local r = orig_gettarget(cfg, direction, pathstyle, namestyle, system)
        if (cfg.objectsdir) and (cfg.objdir) then
            cfg.objectsdir = cfg.objdir
        end
        return r
    end
    -- Silently suppress generation of the .user files ...
    local orig_generate = premake.generate
    premake.generate = function(obj, filename, callback)
        if filename:find(".vcproj.user") or filename:find(".vcxproj.user") then
            return
        end
        orig_generate(obj, filename, callback)
    end
    -- Fix up premake.getlinks() to not do stupid stuff with object files we pass
    local orig_premake_getlinks = premake.getlinks
    premake.getlinks = function(cfg, kind, part)
        local origret = orig_premake_getlinks(cfg, kind, part)
        local ret = {}
        for k,v in ipairs(origret) do
            local dep = v:gsub(".obj.lib", ".obj")
            dep = dep:gsub(".lib.lib", ".lib")
            table.insert(ret, dep)
        end
        return ret
    end

    -- Remove an option altogether or some otherwise accepted values for that option
    local function remove_allowed_optionvalues(option, values_toremove)
        if premake.option.list[option] ~= nil then
            if values_toremove == nil then
                premake.option.list[option] = nil
                return
            end
            if premake.option.list.platform["allowed"] ~= nil then
                local allowed = premake.option.list[option].allowed
                for i = #allowed, 1, -1 do
                    if values_toremove[allowed[i][1]] then
                        table.remove(allowed, i)
                    end
                end
            end
        end
    end

    local function remove_action(action)
        if premake.action.list[action] ~= nil then
            premake.action.list[action] = nil
        end
    end

    -- Remove some unwanted/outdated options
    remove_allowed_optionvalues("dotnet")
    remove_allowed_optionvalues("platform", { universal = 0, universal32 = 0, universal64 = 0, ps3 = 0, xbox360 = 0, })
    remove_allowed_optionvalues("os") -- ... , { bsd = 0, haiku = 0, linux = 0, macosx = 0, solaris = 0, }
    remove_allowed_optionvalues("cc")
    -- ... and actions (mainly because they are untested)
    for k,v in pairs({codeblocks = 0, codelite = 0, gmake = 0, xcode3 = 0, xcode4 = 0, vs2002 = 0, vs2003 = 0, vs2008 = 0, vs2010 = 0, vs2012 = 0, vs2013 = 0, vs2015 = 0}) do
        remove_action(k)
    end
end
