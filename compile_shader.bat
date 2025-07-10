@echo off
setlocal enabledelayedexpansion

REM Paths
set SHADER_INPUT=assets\shaders
set OUTPUT_DIRS=.\ build\x64-debug build\x64-release

REM Shader model
set VS_TARGET=vs_4_0_level_9_3
set PS_TARGET=ps_4_0_level_9_3

REM Create output directories if not exist
for %%D in (%OUTPUT_DIRS%) do (
    if not exist "%%D" (
        mkdir "%%D"
    )
)

REM Compile all .hlsl files
for %%F in (%SHADER_INPUT%\*.hlsl) do (
    set FILE_NAME=%%~nF

    echo.
    echo Compiling: !FILE_NAME!.hlsl

    for %%D in (%OUTPUT_DIRS%) do (
        REM Compile vertex shader
        fxc.exe /T %VS_TARGET% /E VSMain /Fo "%%D\!FILE_NAME!_vs.cso" "%%F"
        if !ERRORLEVEL! NEQ 0 (
            echo Failed to compile vertex shader: !FILE_NAME!.hlsl
            exit /b !ERRORLEVEL!
        )

        REM Compile pixel shader
        fxc.exe /T %PS_TARGET% /E PSMain /Fo "%%D\!FILE_NAME!_ps.cso" "%%F"
        if !ERRORLEVEL! NEQ 0 (
            echo Failed to compile pixel shader: !FILE_NAME!.hlsl
            exit /b !ERRORLEVEL!
        )
    )

    echo Successfully compiled: !FILE_NAME!.hlsl
)

endlocal
