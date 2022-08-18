@echo off

set runexe=%1
set sufile=%2
set /A numexe=%3
SET /A startproc=%4
set /A maxproc=%5
set sufilePath=..\sudokus\%sufile%
if "%runexe%"=="omp" (
	set program='..\omp_projects\x64\Debug\omp.exe %%k %sufilePath%'
)
setlocal EnableDelayedExpansion
if "%runexe%"=="mpi" (
	set mpi="C:\Program Files\Microsoft MPI\Bin\mpiexec.exe" -n %%k
	set program='!mpi! ..\mpi_projects\variante1\x64\Debug\project.exe %sufilePath%'
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
echo "processes";"num_sudokus";"computing time [s]";"solution time [s]";"sudoku_init time [s]">> %csvPath%
setlocal EnableDelayedExpansion
for /L %%k in (%startproc%,1,%maxproc%) do (
	for /L %%i in (1,1,%numexe%) do (
		set linecount=0
		for /f "delims=" %%j in (%program%) do (
				set linecontent[!linecount!]=%%j
				set /a linecount+=1
)
		call echo %%k;!linecontent[0]!;!linecontent[1]!;!linecontent[2]!;!linecontent[3]!>> %csvPath%
)
)

REM pause