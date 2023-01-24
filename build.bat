@echo off



if "%1" == "--clean" (
	rmdir /s /q .\out\build\x64-debug-commandline
	rmdir /s /q .\out\build\x64-release-commandline
	echo Done cleaning.
) else (
	if %ERRORLEVEL% == 0 (
		if "%1" == "--debug" (
			cmake --preset=x64-debug-commandline  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S .
			cd .\out\build\x64-debug-commandline\
		) else (
			cmake --preset=x64-release-commandline  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S .
			cd .\out\build\x64-release-commandline\
		)
	) else (echo ERROR: Failed to run cmake.)

	if %ERRORLEVEL% == 0 (
		ninja
	) else (echo ERROR: Failed to run ninja.)

	if %ERRORLEVEL% == 0 (
		.\Spacetorio\Spacetorio.exe
	) else (echo ERROR: Failed to run the exe file.)
)
