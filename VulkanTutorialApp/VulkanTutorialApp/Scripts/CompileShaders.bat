@echo off
cd Resources\Shaders

for %%I in (Source\*.*) do %VK_SDK_PATH%\Bin\glslangValidator.exe -V Source\%%~nxI -o Binary\%%~nxI.spv
