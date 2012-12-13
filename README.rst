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
`my website`_. Use an archiver to unpack the archive, I recommend 7zip,
and then place the ``.exe`` files into a folder listed in your ``PATH``
environment variable.

As for the utility class all you need is to place the files
``AlternateDataStreams.h`` and ``VerySimpleBuf.h`` into your project,
``#include`` the former and you should be set to use the
``CAlternateDataStreams`` C++ class.

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
