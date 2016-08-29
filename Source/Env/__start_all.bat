@echo off
start LUNIServer.exe --console
start LUNIServer.exe --auth
start /B LUNIServer.exe --master_World
pause

REM start __start_console.bat
REM start __start_auth.bat
REM start __start_world.bat