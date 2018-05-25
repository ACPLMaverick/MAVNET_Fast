@echo off
cd Shaders

set vkPath = ""

rem for %%I in (Source\*.*) do echo %%~nxI
for %%I in (Source\*.*) do C:\VulkanSDK\1.1.73.0\Bin\glslangValidator.exe -V Source\%%~nxI -o Binary\%%~nxI.spv
