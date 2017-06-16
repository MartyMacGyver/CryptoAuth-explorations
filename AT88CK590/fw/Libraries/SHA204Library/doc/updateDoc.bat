@echo off
echo updating SHA204 documentation using src from Distributable folder (patching)
echo Make sure you incremented the project version in the SHA204...Distro.doxyfile files.

setlocal

::   Set destination top folders
set sha204_dest_folder=..\..\SHA204LibraryDistributable
set sam9_folder=%sha204_dest_folder%\SHA204_SAM9G45
set avr8_folder=%sha204_dest_folder%\SHA204_90USB1287


:: **************************************************
:: documentation files
echo Creating documentation for library...

:: This folder is not in SVN but a temp folder. Create it locally.
set src_folder=..\DoxygenLibraryDistro\latex

::goto sam9

:: Create library documentation, no example module and no hardware dependent modules
doxygen SHA204distro.doxyfile > doxyLogLib.txt
if errorlevel 1 goto error
pdflatex -interaction=batchmode -include-directory=%src_folder% -output-directory=%src_folder% %src_folder%\refman
if errorlevel 1 goto error

:: Copy library documentation to avr8 and sam9 folders
set dest_folder=%sam9_folder%\doc
del %dest_folder%\* /q
if errorlevel 1 goto error
copy ..\..\..\doc\*.rtf %dest_folder%\*
if errorlevel 1 goto error
copy %src_folder%\refman.pdf %dest_folder%\SHA204Library.*
if errorlevel 1 goto error

set dest_folder=%avr8_folder%\doc
del %dest_folder%\* /q
if errorlevel 1 goto error
copy ..\..\..\doc\*.rtf %dest_folder%\*
if errorlevel 1 goto error
copy %src_folder%\refman.pdf %dest_folder%\SHA204Library.*
if errorlevel 1 goto error

:: Create AVR8 example documentation and copy to doc folder
set src_folder=..\DoxygenExampleAT90USB1287distro\latex
doxygen SHA204exampleDistro.doxyfile > doxyLogAvr8.txt
if errorlevel 1 goto error
pdflatex -interaction=batchmode -include-directory=%src_folder% -output-directory=%src_folder% %src_folder%\refman
if errorlevel 1 goto error
copy %src_folder%\refman.pdf %dest_folder%\SHA204LibraryExampleAT90USB1287.*
if errorlevel 1 goto error

:sam9
:: Create SAM9 example documentation and copy to doc folder
set src_folder=..\DoxygenExampleAT91SAM9distro\latex
set dest_folder=%sam9_folder%\doc
doxygen SHA204exampleSAM9G45distro.doxyfile > doxyLogSam9.txt
if errorlevel 1 goto error
pdflatex -interaction=batchmode -include-directory=%src_folder% -output-directory=%src_folder% %src_folder%\refman
if errorlevel 1 goto error
copy %src_folder%\refman.pdf %dest_folder%\SHA204LibraryExampleAT91SAM9G45.*
if errorlevel 1 goto error

echo SHA204 documentation updated.

echo Distribution folder updated. 
echo !!! Info: This batch file does not check for all errors. Files might be missing. !!!
goto end

:error
echo !!! Error updating distribution folder !!!

:end

endlocal

pause
