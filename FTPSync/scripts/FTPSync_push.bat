@echo off
setlocal

set /p yes="Are you sure you want to perform PUSH operation? You may accidentally delete or mess up something! [Y/N]: "

if %yes% == y call FTPSync.bat PUSH
if %yes% == Y call FTPSync.bat PUSH

:eof

endlocal