@echo off



if "%1" == "--clean" (rmdir /s /q .\out\build\x64-debug)

if %ERRORLEVEL% == 0 (
	cmake -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B .\out\build\x64-debug\ -S .
) else (echo ERROR: Failed to run cmake.)
	
if %ERRORLEVEL% == 0 (
	cd .\out\build\x64-debug\ 
) else (echo ERROR: Failed to cd into build folder.)

if %ERRORLEVEL% == 0 (
	ninja
) else (echo ERROR: Failed to run ninja.)

if %ERRORLEVEL% == 0 (
	.\Spacetorio\Spacetorio.exe
) else (echo ERROR: Failed to run the exe file.)
