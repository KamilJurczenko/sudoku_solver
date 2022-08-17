@echo off
set maxproc=16

set runexe=%1
set sufile=%2
set /A numexe=%3

for /L %%i in (1,1,%maxproc%) do (
	call runwproc.bat %runexe% %%i %sufile% %numexe%
)

pause