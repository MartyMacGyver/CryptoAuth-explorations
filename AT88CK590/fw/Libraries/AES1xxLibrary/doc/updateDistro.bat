@echo off
:: --------------------------------!!! UNFINISHED !!!
echo updating AES132 documentation

setlocal

::   Set source top folders
set example_path=..\..\..\LibraryExamples\AESxx\AVRStudio5

::   Set destination top folders
set aes132_dest_folder=..\..\AES132LibraryDistributable\aes132_library

::goto skip_pdf


:: **************************************************
:: documentation files
:: These folders are not in SVN but temp folders. Create them locally.
:: The latex to pdf conversion (make...) takes a couple of seconds.
echo Creating documentation for library...

set src_folder=..\DoxygenLibrary\latex

::goto skip_pdf

::goto skip_doxygen

:: Create library documentation
doxygen AES132.doxyfile
if errorlevel 1 goto error
make --directory=%src_folder% pdf
::We cannot check for errors because make returns also non-zero
::when there were only warnings
::if errorlevel 1 goto error

:: Copy license agreement and library documentation
:skip_doxygen
set dest_folder=%aes132_dest_folder%\doc
del %dest_folder%\* /q
if errorlevel 1 goto error
copy ..\..\..\*.rtf %dest_folder%\*
if errorlevel 1 goto error
copy %src_folder%\refman.pdf %dest_folder%\aes132_library.*
if errorlevel 1 goto error

echo AES132 documentation updated.


:: **************************************************
:skip_pdf
echo Updating distribution folders for AVR8 example

:: Copy library modules
set src_folder=..
set dest_folder=%aes132_dest_folder%\aes132_library
del %dest_folder%\*.h /q
if errorlevel 1 goto error
del %dest_folder%\*.c /q
if errorlevel 1 goto error

copy %src_folder%\aes132_example_main.c %dest_folder%\*.*
copy %src_folder%\aes132_comm.c %dest_folder%\*.*
copy %src_folder%\aes132_comm.h %dest_folder%\*.*
copy %src_folder%\aes132_lib_return_codes.h %dest_folder%\*.*
copy %src_folder%\aes132_physical.h %dest_folder%\*.*
copy %src_folder%\aes132_i2c.c %dest_folder%\*.*
copy %src_folder%\aes132_spi.c %dest_folder%\*.*

:: Copy hardware modules
set src_folder=..\..\..\LibraryExamples\Hardware\AVR_AT
copy %src_folder%\i2c_phys.* %dest_folder%\*
copy %src_folder%\spi_phys.* %dest_folder%\*

:: Copy utility module
set src_folder=..\..\Utilities
copy %src_folder%\timer_utilities.* %dest_folder%\*

:: **************************************************

echo Distribution folder updated. 
echo !!! This batch file did not check for all errors. Files might be missing. !!!
goto end

:error
echo !!! Error updating distribution folder !!!

:end

endlocal

pause
