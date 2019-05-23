@echo off
setlocal

rem This script and program are provided "as is", without any warranties. You use them on your own responsibility.
rem Basically all you need to do is to put three scripts and FTPSync.exe in your minecraft root folder, e.g. C:\Users\Janusz\AppData\Roaming\.minecraft
rem Then you need to edit following variables so they reflect your environment: 

rem IP Address of the FTP server machine. If you use ZeroTier VPN, this should be a proper IP in our VPN network.
set serverIP=192.168.0.101

rem User defined specifications end here.


rem These variables should normally stay as they are. You can try to check them if something doesn't work though.
rem "-m PUSH -a 192.168.0.101 2221 -u sew327 -p dupablada123 -d mods -rd MinecraftModfolder/mods -f *.jar"

set serverPort=2221
set serverLogin=sew327
set serverPassword=dupablada123
set localWorkingDir=mods
set remoteWorkingDir=MinecraftModfolder/mods
set extensionFilters=*.jar

rem And here the main program is called with previously set parameters.

call FTPSync.exe -m %1 -a %serverIP% %serverPort% -u %serverLogin% -p %serverPassword% -d %localWorkingDir% -rd %remoteWorkingDir% -f %extensionFilters%

endlocal