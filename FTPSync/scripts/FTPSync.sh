#!/bin/bash

# This script and program are provided "as is", without any warranties. You use them on your own responsibility.
# Basically all you need to do is to put three scripts and FTPSync executable in your minecraft root folder, e.g. C:\Users\Janusz\AppData\Roaming\.minecraft
# Then you need to edit following variables so they reflect your environment: 

# IP Address of the FTP server machine. If you use ZeroTier VPN, this should be a proper IP in our VPN network.
serverIP=192.168.0.101

# User defined specifications end here.


# These variables should normally stay as they are. You can try to check them if something doesn't work though.
# "-m PUSH -a 192.168.0.101 2221 -u sew327 -p dupablada123 -d mods -rd MinecraftModfolder/mods -f *.jar"

serverPort=2221
serverLogin=sew327
serverPassword=dupablada123
localWorkingDir=mods
remoteWorkingDir=MinecraftModfolder/mods
extensionFilters=.jar                       # On Linux filters CANNOT HAVE ASTERISKS.

# And here the main program is called with previously set parameters.
./FTPSync -m $1 -a $serverIP $serverPort -u $serverLogin -p $serverPassword -d $localWorkingDir -rd $remoteWorkingDir -f $extensionFilters