@echo off
echo Building ALF-EWG C++ Core Engine...
C:\MinGW\bin\g++.exe -std=c++14 -O2 -Wall -I./include src/sensor_station.cpp src/main.cpp -o ../bin/alf_ewg_engine.exe
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] ALF-EWG C++ Core Engine compiled successfully: bin/alf_ewg_engine.exe
) else (
    echo [ERROR] Compilation failed with error code %ERRORLEVEL%
)
