=========================================
 ``CAlternateDataStreams`` and ``lsads``
=========================================
:Author: Oliver Schneider

About
-----
``CAlternateDataStreams`` is a C++ utility class without dependencies
to the STL, MFC, ATL or some such. It allows you to list all the
alternate data streams (ADS) on an NTFS file or folder.

``lsads`` is a simple tool to enumerate the ADS from the command line.

Installation
------------
No installation required. Download the binaries (32 and 64bit) from
`my website`_ or `Bitbucket`_. Use an archiver to unpack the archive,
I recommend `7-Zip`_, and then place the ``.exe`` files into a folder
listed in your ``PATH`` environment variable.

As for the utility class all you need is to place the files
``AlternateDataStreams.h`` and ``VerySimpleBuf.h`` into your project,
``#include`` the former and you should be set to use the
``CAlternateDataStreams`` C++ class.

Build instructions
------------------
In order to build the tool yourself, you'll either need the Windows 2003
Server DDK or Visual Studio. If you use the DDK, you can simply rely on
the ``relbuild.cmd`` script, but you may have to set the ``WNETBASE``
environment variable to a sensible value (e.g. ``WNETBASE=C:\WINDDK\3790.1830``)
to match the path to the installed DDK.

If you prefer to use Visual Studio, a solution/project for Visual Studio
2005 is included and you can generate any of the other supported projects
yourself using the `premake4`_ that comes with the WinDirStat source code.
Use a tool such as ``sigcheck`` to verify the code signature on it, though.

The way you generate the projects is by calling ``premake4`` one of the
following ways::

    premake4 vs2005
    premake4 vs2008
    premake4 vs2010
    premake4 vs2012
    premake4 vs2013

The earlier Visual Studio versions may work, but no guarantees.

Requirements
------------
The program should run on Windows 2000 and later. Itanium CPUs are not
supported, but feel free to build that yourself, if needed.

The ``CAlternateDataStreams`` class should work starting on Windows 2000,
too. If you happen to have some old NT4 box around, try it there. It's
likely it will still work even there (except if your linker does funny
things).

Usage
-----

Syntax::

    lsads <filename>

License
-------
The tools and the classes are placed into the PUBLIC DOMAIN (CC0).

.. _my website: https://assarbad.net/stuff/lads.zip
.. _premake4: https://bitbucket.org/windirstat/windirstat/src/tip/common/premake4.exe
.. _Bitbucket: https://bitbucket.org/assarbad/lsads/downloads
.. _7-Zip: http://7-zip.org/
