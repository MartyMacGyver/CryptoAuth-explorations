The "Distro" doxygen files are used by the updateDoc batch file for patching the distribution.
They use Documentation.txt from SHA204Library, but the source from SHA204LibraryDistributable.
Patching could be done from a tag also.
Don't use the the Doxygen files anymore that don't have "distro" in their names. It is smarter
to create the documentation from files in the distribution folder instead of the original
folders because the path links in the generated document would not work. "updateDistro.bat"
is still using the old method (source files from library and other folders), but
"updateDistro.py" uses the new method (source files from distribution source folder).
