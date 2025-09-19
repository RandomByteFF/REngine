@echo off
setlocal

set "OUTPUT_DIR=."
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

for %%f in (*.vert *.frag) do (
    if exist "%%f" (
        set "file=%%f"
        set "base=%%~nxf"
        set "name=%%~nf"
        call :compile
    )
)

goto :eof

:compile
glslc "%file%" -o "%OUTPUT_DIR%\%name%.spv"
goto :eof