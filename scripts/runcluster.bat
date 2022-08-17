@echo off

set runexe=%1
set sufile=%2
set /A numexe=%3
SET /A startproc=%4
set /A maxproc=%5

set exeFile=none

if "%runexe%"=="omp" (
	set exeFile=..\omp_projects\x64\Debug\omp.exe
)
if "%runexe%"=="mpi1" (
	set exeFile=..\mpi_projects\variante1\x64\Debug\project.exe
)
if "%runexe%"=="mpi2" (
	set exeFile=..\mpi_projects\variante1\x64\Debug\project.exe
)

set rawDataPath=..\rawdata

:: Timestamp as unique foldername
set hh=%TIME:~0,2%
IF "%hh:~0,1%" == " " SET hh=0%hh:~1,1%
set Timestamp=%DATE:~0,2%%DATE:~3,2%%DATE:~6,8%%hh%%time:~3,2%%time:~6,2%

::set path=%rawDataPath%\%runexe%\%sufile:.txt=%\
set path=%rawDataPath%\%runexe%\
mkdir %path%
set csvPath=%path%%sufile:.txt=%_%Timestamp%.csv
set step=0
echo "processes";"computing time [s]">> %csvPath%
for /L %%k in (%startproc%,%step%,%maxproc%) do (
	set step=%step%*2
	for /L %%i in (1,1,%numexe%) do (
		for /f %%j in ('%exeFile% %%k %sufile%') do (
				call echo %%k;%%j>> %csvPath%
)
)
)

REM pause