@echo off
call pyinstaller -F -w dobble.py
rd /s /q build
del /f /q dobble.spec
copy dist\dobble.exe \\banan\depot\other\dobble
copy install.bat \\banan\depot\other\dobble
rd /s /q dist