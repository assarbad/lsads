-- The below is used to insert the .vs(2005|2008|2010|2012|2013) into the file names for projects and solutions
local action = _ACTION or ""
do
    -- This is mainly to support older premake4 builds
    if not premake.project.getbasename then
        print "Magic happens ..."
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
    -- Name the project files after their VS version
    local orig_getbasename = premake.project.getbasename
    premake.project.getbasename = function(prjname, pattern)
        if _ACTION then
            name_map = {vs2005 = "vs8", vs2008 = "vs9", vs2010 = "vs10", vs2012 = "vs11", vs2013 = "vs12"}
            if name_map[_ACTION] then
                pattern = pattern:gsub("%%%%", "%%%%." .. name_map[_ACTION])
            else
                pattern = pattern:gsub("%%%%", "%%%%." .. _ACTION)
            end
        end
        return orig_getbasename(prjname, pattern)
    end
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
        if filename:find('.vcproj.user') or filename:find('.vcxproj.user') then
            return
        end
        orig_generate(obj, filename, callback)
    end
end

solution ("lsads")
    configurations  {"Debug", "Release"}
    platforms       {"x32", "x64"}
    location        ('.')

    project ("lsads")
        local int_dir   = "intermediate/" .. action .. "_" .. "$(PlatformName)_$(ConfigurationName)"
        uuid            ("86AFA312-9D89-4FAE-8201-B02492AA7D64")
        language        ("C++")
        kind            ("ConsoleApp")
        flags           {"StaticRuntime", "Unicode", "NativeWChar", "ExtraWarnings", "NoRTTI", "WinMain", "NoMinimalRebuild"}
        defines         ("WINVER=0x0500", "_CONSOLE", "WIN32")
        targetdir       ("bin")
        objdir          (int_dir)
        libdirs         {"$(IntDir)"}
        resoptions      {"/nologo", "/l409"}

        files
        {
            "*.cpp",
            "*.h",
            "*.rc",
            "premake4.lua",
            "*.rst", "*.txt",
        }
        
        vpaths
        {
            ["Header Files/*"] = { "*.h" },
            ["Source Files/*"] = { "*.cpp" },
            ["Resource Files/*"] = { "*.rc" },
            ["*"] = { "premake4.lua", "*.rst", "*.txt", },
        }

        configuration {"Debug"}
            defines         ("_DEBUG")
            flags           {"Symbols"}

        configuration {"Release"}
            defines         ("NDEBUG")
            flags           {"Optimize"}
            linkoptions     {"/release"}
            buildoptions    {"/Oi", "/Ot"}

        configuration {"Debug", "x32"}
            targetsuffix    ("32_dbg")

        configuration {"Debug", "x64"}
            targetsuffix    ("64_dbg")

        configuration {"Release", "x32"}
            targetsuffix    ("32")

        configuration {"Release", "x64"}
            targetsuffix    ("64")
