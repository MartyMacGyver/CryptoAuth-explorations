import os
import sys
from shutil import copy

# For now, this script updates only the documentation.
example_path = "../../../LibraryExamples/ECC108"
example_path_avr8 = example_path + "/EclipseAT90USB1287"
lib_path = "../"

ecc108_dest_folder = "../../ECC108LibraryDistributable/"
avr8_sub_folder = "ECC108_90USB1287/"
avr8_folder = ecc108_dest_folder + avr8_sub_folder 


def delete_files(path):
    for filename in os.listdir(path):
        filepath = os.path.join(path, filename)
#        print(filepath)
        status = os.remove(filepath)
        if status:
            return status
    return 0


def generate_pdf(doxyfile, src_path, dest_folder):
    status = os.system("doxygen -u %s" % (doxyfile))
    if status:
        return status

    pdf_latex = "pdflatex -interaction=batchmode -include-directory=%s -output-directory=%s %s/refman"
    src_folder = src_path + "/latex"    
    return os.system(pdf_latex % (src_folder, src_folder, src_folder))
    

def copy_common_documentation(src, dest_folder, dest_name):
    try:
        print("Copying license agreement and library documentation to %s..." % (dest_folder))
        doc_folder = dest_folder + "doc/"
        
        # Delete all files in doc_folder.
        status = delete_files(doc_folder)
        if status:
            return None
        
        # Copy license agreement.
        path = copy("../../../doc/LicenseAgreement.rtf", doc_folder)

        # Copy library documentation.
        path = copy(src + "latex/refman.pdf", doc_folder + dest_name + ".pdf")
        
        print("Done copying.")
        
        return path
        
    except Exception as e:
        print(e)
        return None


print("Updating distribution folder for the ATECC108 library.")

# Flags to exclude generation of documentation or certain example targets.
generate_pdf_flag = True
generate_avr_flag = True
status = -1

dest_folder = ecc108_dest_folder + avr8_sub_folder + "src/"
        

# Copy source code.
import os
print("Working directory is %s" % (os.getcwd()) 
print("Copying library source modules...")
# Copy library modules.
lib_modules = ["ecc108_example_main.c", "ecc108_examples.c", "ecc108_examples.h", \
               "ecc108_comm_marshaling.c", "ecc108_comm_marshaling.h", \
              "ecc108_comm.c", "ecc108_comm.h", "ecc108_helper.c", "ecc108_helper.h", \
              "ecc108_i2c.c", "ecc108_swi.c", \
              "ecc108_config.h", "ecc108_physical.h", "ecc108_lib_return_codes.h"]
for lib_module in lib_modules:
    path = copy(lib_path + lib_module, dest_folder + lib_module)
# In Python 3.2, copy does not return a value but raises an IOError.
#    if path == None:
#        print("Copy failed.")
#        sys.exit(-1)
# Copy timer utility modules.
src_folder = "../../utilities/"
util_modules = ["timer_utilities.c", "timer_utilities.h"]
for util_module in util_modules:
    path = copy(src_folder + util_module, dest_folder + util_module)
#    if path == None:
#        print("Copy failed.")
#        sys.exit(-1)
# Copy hardware dependent modules.
src_folder = "../../../LibraryExamples/Hardware/AVR_AT/"
hw_modules = ["bitbang_config.h", "bitbang_phys.c", "uart_config.h", "uart_phys.c", \
              "swi_phys.h", "i2c_phys.c", "i2c_phys.h", "avr_compatible.h"]
for hw_module in hw_modules:
    path = copy(src_folder + hw_module, dest_folder + hw_module)
#    if path == None:
#        print("Copy failed.")
#        sys.exit(-1)

print("Done copying.")
print("Done updating distribution folder for AT90USB1287 example.")


if generate_pdf_flag:
    # Generate PDF documentation using Doxygen and MikTex.
    # The source is taken from the distribution folder.
    print("Creating documentation...")
    src_folder = "../DoxygenLibraryDistro/"
    
    if generate_avr_flag:
        # Parse doxyfile for library.
        status = generate_pdf("ECC108distro.doxyfile", src_folder, "")
        if status:
            print("Error executing generate_pdf")
            sys.exit(-1)
        print("Done creating documentation for library.")

        dest_folder = ecc108_dest_folder + avr8_sub_folder
        # Copy common documentation files (license, library) to destination.
        path = copy_common_documentation(src_folder, dest_folder, "ecc108_library")
#        if path == None:
#            sys.exit(-1)
                    
        # Parse doxyfile for example.
  #      src_folder = "../DoxygenExampleAT90USB1287distro/"
  #      status = generate_pdf("ECC108exampleDistro.doxyfile", src_folder, "")
  #      if status:
  #          print("Error executing generate_pdf")
  #          sys.exit(-1)
  #      print("Done creating documentation for AT90USB1287 library example.")

        # Copy documentation file to destination.
        print("Copying documentation for AT90USB1287 library example...")
        copy(src_folder + "/latex/refman.pdf", dest_folder + "doc/ECC108LibraryExampleAT90USB1287.pdf")
        
        print("Done copying.")
