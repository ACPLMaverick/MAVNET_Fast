@echo off

rem #### One can change this parameter to influence the quality of image extraction. ####
set default_density=300

set file_name=%1
set file_name_short=%~n1
set file_name_short_ext=%~nx1
set output_dir=.\%file_name_short%_output
set output_file_name=%output_dir%\%file_name_short%-%d.jpg

if not exist %file_name% (
	echo Error: File "%file_name%" does not exist.
	goto eof
) 

set resolution=%2
if "%resolution%"==""  (
	set resize_command=
) else (
	set resize_command=-resize %resolution%
)

echo Processing file: [%file_name%]
echo Please wait...

if not exist %output_dir% mkdir %output_dir%

call magick -density %default_density% "%file_name%" %resize_command% "%output_file_name%"

rem Please comment line below to be able to see imagemagick errors.
robocopy %output_dir% %output_dir% /S /NJH /NJS /NS /NC /NFL /NDL /NP /MOVE

exit

:eof
pause