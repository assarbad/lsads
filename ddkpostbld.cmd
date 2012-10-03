@echo off
setlocal
:: Perform post-build steps
set BINDIR=.\bin
if not EXIST "%BINDIR%" md "%BINDIR%"
for /d %%i in (.\obj%BUILD_ALT_DIR%\*) do @(
  xcopy /y "%%i\*.exe" "%BINDIR%"
  xcopy /y "%%i\*.pdb" "%BINDIR%"
)
endlocal
