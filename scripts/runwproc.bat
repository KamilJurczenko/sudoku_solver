@echo off

set runexe=%1
set /A numprocs=%2
set sufile=%3
set /A numexe=%4

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

set path=%rawDataPath%\%runexe%\%sufile:.txt=%\
mkdir %path%
set csvPath=%path%%Timestamp%.csv
echo "computing time [s]">> %csvPath%
for /L %%i in (1,1,%numexe%) do (
	for /f %%j in ('%exeFile% %numprocs% %sufile%') do (
			call echo %%j>> %csvPath%
)
)

REM pause