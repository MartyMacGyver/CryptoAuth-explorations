@echo off
echo updating SHA204 documentation

setlocal

::   Set source top folders
set example_path=..\..\..\LibraryExamples\SHA204
set example_path_sam9=%example_path%\EclipseAT91SAM9G45
set example_path_avr8=%example_path%\EclipseAT90USB1287

::   Set destination top folders
set sha204_dest_folder=..\..\SHA204LibraryDistributable
set sam9_folder=%sha204_dest_folder%\SHA204_SAM9G45
set avr8_folder=%sha204_dest_folder%\SHA204_90USB1287

::goto skip_pdf


:: **************************************************
:: documentation files
:: These folders are not in SVN but temp folders. Create them locally.
echo Creating documentation for library...

set src_folder=..\DoxygenLibrary\latex

::goto sam9

:: Create library documentation
doxygen SHA204.doxyfile
if errorlevel 1 goto error
pdflatex -interaction=batchmode -include-directory=%src_folder% -output-directory=%src_folder% %src_folder%\refman
if errorlevel 1 goto error

:: Copy license agreement and library documentation to avr8 and sam9 folders
set dest_folder=%sam9_folder%\doc
del %dest_folder%\* /q
if errorlevel 1 goto error
copy ..\..\..\*.rtf %dest_folder%\*
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
set src_folder=..\DoxygenExampleAT90USB1287\latex
doxygen SHA204example.doxyfile
if errorlevel 1 goto error
pdflatex -interaction=batchmode -include-directory=%src_folder% -output-directory=%src_folder% %src_folder%\refman
if errorlevel 1 goto error
copy %src_folder%\refman.pdf %dest_folder%\SHA204LibraryExampleAT90USB1287.*
if errorlevel 1 goto error

:sam9
:: Create SAM9 example documentation and copy to doc folder
set src_folder=..\DoxygenExampleAT91SAM9\latex
set dest_folder=%sam9_folder%\doc
doxygen SHA204exampleSAM9G45.doxyfile
if errorlevel 1 goto error
pdflatex -interaction=batchmode -include-directory=%src_folder% -output-directory=%src_folder% %src_folder%\refman
if errorlevel 1 goto error
copy %src_folder%\refman.pdf %dest_folder%\SHA204LibraryExampleAT91SAM9G45.*
if errorlevel 1 goto error

echo SHA204 documentation updated.


:: **************************************************
:skip_pdf
echo Updating distribution folders for AVR8 example
:: Updating the at91lib folders is not part of this script.

:: Copy library modules for AVR8
set src_folder=..
set dest_folder=%avr8_folder%\src
del %dest_folder%\* /q
if errorlevel 1 goto error

copy %src_folder%\sha204_*.* %dest_folder%\*

:: Copy hardware modules
set src_folder=..\..\..\LibraryExamples\Hardware\AVR_AT
copy %src_folder%\bitbang_config.h %dest_folder%\*
copy %src_folder%\bitbang_phys.c %dest_folder%\*
copy %src_folder%\uart_config.h %dest_folder%\*
copy %src_folder%\uart_phys.c %dest_folder%\*
copy %src_folder%\swi_phys.* %dest_folder%\*
copy %src_folder%\i2c_phys.* %dest_folder%\*
copy %src_folder%\avr_compatible.h %dest_folder%\*
set src_folder=..\..\..\LibraryExamples\Hardware\Utilities

:: Copy utility module
set src_folder=..\..\Utilities
copy %src_folder%\timer_utilities.* %dest_folder%\*
pause
:: **************************************************

echo Updating distribution folders for SAM9 example

:: Copy library modules for SAM9
set src_folder=..
set dest_folder=%sam9_folder%\src
del %dest_folder%\* /q
copy %src_folder%\sha204_comm*.* %dest_folder%\*
copy %src_folder%\sha204_physical.h %dest_folder%\* 
copy %src_folder%\sha204_config.h %dest_folder%\* 
copy %src_folder%\sha204_lib_return_codes.h %dest_folder%\* 

:: Copy main module and i2c module
set src_folder=%example_path_sam9%\SHA204_SAM9MG45
set dest_folder=%sam9_folder%\src
copy %src_folder%\*.c %dest_folder%\*


:: **************************************************

echo Distribution folder updated. 
echo !!! Info: This batch file does not check for all errors. Files might be missing. !!!
goto end

:error
echo !!! Error updating distribution folder !!!

:end

endlocal

pause
