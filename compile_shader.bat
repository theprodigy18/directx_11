@echo off
setlocal

REM Paths
set SHADER_INPUT=assets\shaders

REM Compile shader
echo Compiling vertex shader...
fxc.exe /T vs_4_0_level_9_3 /E main /Fo "vertex_shader.cso" "%SHADER_INPUT%\vertex_shader.hlsl"
fxc.exe /T ps_4_0_level_9_3 /E main /Fo "pixel_shader.cso" "%SHADER_INPUT%\pixel_shader.hlsl"

if %ERRORLEVEL% NEQ 0 (
    echo Shader compilation failed!
    exit /b %ERRORLEVEL%
) else (
    echo Shader compiled successfully!
)

endlocal
