@echo off
if "%~1" == "noinc" set NO_INCREASE_BUILDNO=1
call ddkbuild.cmd -WNET2K fre . -cZ
set NO_INCREASE_BUILDNO=1
call ddkbuild.cmd -WNETAMD64 fre . -cZ
for %%i in (*.zip *.zip.asc bin\*.ilk bin\*.idb bin\*_dbg.*) do @del /f %%i
call ollisign.cmd "%~dp0\bin\*.exe" "http://assarbad.net" "Application to investigate alternate data streams (ADS) on Windows"
:: Find 7-Zip
set SEVENZIP=%ProgramFiles%\7-Zip\7z.exe
if not exist "%SEVENZIP%" set SEVENZIP=%ProgramFiles(x86)%\7-Zip\7z.exe
if not exist "%SEVENZIP%" ( echo ERROR: Could not find 7z.exe & goto :EOF )
set ARCHIVE=lads.zip
for %%i in (%ARCHIVE% %ARCHIVE%.asc) do @del /f %%i
"%SEVENZIP%" a -tzip %ARCHIVE% bin BUILD makefile sources *.cmd *.cpp *.h *.rc *.rst *.sln *.vcproj *.vcxproj *.vsprops
sha1sum %ARCHIVE%
md5sum %ARCHIVE%
gpg -a --detach-sign %ARCHIVE%
