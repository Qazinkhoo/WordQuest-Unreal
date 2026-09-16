@echo off
setlocal

set "UE_ROOT=D:\Epic Games\UE_5.8"
set "PROJECT=%~dp0WordQuest.uproject"
set "OUTPUT=%~dp0Packaged\WordQuest-Windows"

if not exist "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" (
    echo ERROR: Unreal Engine 5.8 was not found at:
    echo %UE_ROOT%
    echo.
    echo Update UE_ROOT in this file if Unreal Engine is installed elsewhere.
    pause
    exit /b 1
)

if not exist "%PROJECT%" (
    echo ERROR: WordQuest.uproject was not found.
    pause
    exit /b 1
)

echo ============================================================
echo WORD QUEST - WINDOWS SHIPPING PACKAGE
echo ============================================================
echo Project: %PROJECT%
echo Output : %OUTPUT%
echo.

echo Building, cooking, compressing and packaging Word Quest...
call "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
 -project="%PROJECT%" ^
 -noP4 ^
 -platform=Win64 ^
 -clientconfig=Shipping ^
 -build ^
 -cook ^
 -stage ^
 -package ^
 -pak ^
 -iostore ^
 -compressed ^
 -archive ^
 -archivedirectory="%OUTPUT%" ^
 -utf8output

if errorlevel 1 (
    echo.
    echo ============================================================
    echo PACKAGE FAILED
    echo ============================================================
    echo Copy the first red/error lines from the output and send them to ChatGPT.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo PACKAGE SUCCEEDED
    echo ============================================================
echo Your export is in:
echo %OUTPUT%
echo.
echo Zip the Windows folder inside that output when you are ready to share it.
pause
endlocal
