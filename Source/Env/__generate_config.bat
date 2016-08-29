@echo off

ECHO This will reset the current config file
ECHO To confirm,
ECHO.
pause

CLS
start /B LUNIServer.exe --gConfig
pause