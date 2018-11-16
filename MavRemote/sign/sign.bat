@echo off
set pathApp=..\app\build\outputs\apk\debug\
set nameApp=app-debug.apk
set nameAppSigned=app-debug_signed.apk
java -jar signapk.jar platform.x509.pem platform.pk8 %pathApp%%nameApp% %pathApp%%nameAppSigned%