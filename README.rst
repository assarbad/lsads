=========================================
 ``CAlternateDataStreams`` and ``lsads``
=========================================

About
-----
``CAlternateDataStreams`` is a C++ utility class without dependencies to the STL,
MFC, ATL or some such. It allows you to list all the alternate data streams (ADS)
on an NTFS file or folder.

``lsads`` is a simple tool to enumerate the ADS from the command line.

Installation
------------
No installation required. Download the binaries (32 and 64bit) from `Bitbucket`_.
Use an archiver to unpack the archive, I recommend `7-Zip`_, and then place the
``.exe`` files into a folder listed in your ``PATH`` environment variable.

As for the utility class all you need is to place the files
``AlternateDataStreams.hpp`` and ``VerySimpleBuf.hpp`` into your project,
``#include`` the former and you should be set to use the ``CAlternateDataStreams``
C++ class.

Build instructions
------------------
If you prefer to use Visual Studio, a solution/project for Visual Studio 2005 is
included and you can generate any of the other supported projects yourself using
the `premake4`_ that comes with the WinDirStat source code. Use a tool such as
``sigcheck`` to verify the code signature on it, though.

The way you generate the projects is by calling ``premake4`` one of the
following ways::

    premake4 vs2005
    premake4 vs2008
    premake4 vs2010
    premake4 vs2012
    premake4 vs2013
    premake4 vs2015
    premake4 vs2017

The earlier Visual Studio versions may work, but no guarantees.

My prepared premake4.lua can take a number of arguments. Most notably it'll take

  * ``--xp`` to generate XP-compatible projects on newer VS versions.
  * ``--msvcrt`` to use the ``msvcrt.dll`` import Windows Vista WDK which makes
    the resulting binary statically import ``msvcrt.dll``, but in the version
    backwards compatible until Windows 2000 (and 2003 for x64).
    In order to make use of this, you have to set the environment variable
    ``WLHBASE`` to point to your WDK (e.g. ``WLHBASE=C:\WINDDK\6001.18002``).
  * ``--release`` will generate the release solution (no ``Debug`` configuration)
    and is used by the ``relbuild.cmd`` script. This option implies ``--xp`` and
    ``--msvcrt`` at the time of this writing.

Requirements
------------
The program should run on Windows 2000 and later for x86-32, and on Windows 2003
and later for x86-64, aka x64 (this includes Windows XP x64, which used the same
code base as Windows 2003 Server). Itanium CPUs are not supported, but feel free
to build that yourself, if needed.

The ``CAlternateDataStreams`` class should work starting on Windows 2000, too.
If you happen to have some old NT4 box around, try it there. It's likely it will
still work even there (except if your linker does funny things).

Usage
-----

Syntax::

    lsads [--nologo] <path>

License
-------
The tool and the classes are placed into the PUBLIC DOMAIN/CC0, with the
exception of CVersionInfo (inside ``VersionInfo.hpp``), which is licensed under
the very liberal MIT license. Licensing information can usually be found at the
top of each source file. Anything not explicitly licensed in the source file can
be assumed to have been placed into the PUBLIC DOMAIN/CC0.

.. _premake4: https://bitbucket.org/windirstat/premake-stable/downloads/
.. _Bitbucket: https://bitbucket.org/assarbad/lsads/downloads/
.. _7-Zip: http://7-zip.org/
