#!/bin/env python
#==========================================================================
# (c) 2004-2005  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Simple SPI EEPROM programmer.
# File    : aaspi_program.py
#--------------------------------------------------------------------------
# Program an SPI EEPROM device using an Intel Hex format file.
#--------------------------------------------------------------------------
# Redistribution and use of this file in source and binary forms, with
# or without modification, are permitted.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#==========================================================================

#==========================================================================
# IMPORTS
#==========================================================================
import sys

from aardvark_py import *


#==========================================================================
# CONSTANTS
#==========================================================================

# A mapping of devices to their (total memory size, page size)
#
SPI_BITRATE = 4000

DEVICES = {
   "AT25256" : (32768, 64),
   "AT25080" : (1024, 32)
}


#==========================================================================
# FUNCTIONS
#==========================================================================
def _writeMemory (handle, device, data):
    # Determine the max size and page size of eeprom
    (max_size, page_size) = DEVICES[device]

    n = 0
    while (n < len(data)):
        # Write the write enable instruction
        data_out = array('B', [ 0x06 ])
        (count, data_in) = aa_spi_write(handle, data_out, 0)
        if (count < 0):
            print "error: %s\n" % aa_status_string(count)
            return
        elif (count != 1):
            print "error: read %d bytes (expected %d)" % (count, 1)

        # Assemble the write command and address
        data_out = array('B', [ 0 for i in range(3 + page_size) ] )
        data_out[0] = 0x02
        data_out[1] = (n >> 8) & 0xff
        data_out[2] = (n >> 0) & 0xff

        # Assemble a page of data
        data_out[3:] = data[n:n+page_size]
        
        n += len(data_out) - 3

        # Write the transaction
        (count, data_in) = aa_spi_write(handle, data_out, 0)
        if (count < 0):
            print "error: %s\n" % aa_status_string(count)
            return
        elif (count != len(data_out)):
            print "error: read %d bytes (expected %d)" % (count-3, len(data_out))
            
        aa_sleep_ms(10)
        


def _readMemory (handle, addr, length):
    data_out = array('B', [ 0 for i in range(3+length) ])
    data_in  = array_u08(3+length)

    # Assemble the read command and address
    data_out[0] = 0x03
    data_out[1] = (addr >> 8) & 0xff
    data_out[2] = (addr >> 0) & 0xff

    # Write the transaction
    (count, data_in) = aa_spi_write(handle, data_out, data_in)

    if (count < 0):
        print "error: %s\n" % aa_status_string(count)
        return
    elif (count != length+3):
        print "error: read %d bytes (expected %d)" % (count-3, length)

    return data_in[3:]


#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 4):
    print "usage: aaspi_program DEVICE PORT MODE FILENAME"
    print "  DEVICE    is the EEPROM device type"
    print "             - AT25256"
    print "             - AT25080"
    print "  PORT      is the port of the Aardvark adapter"
    print "  MODE      is the SPI Mode"
    print "             - mode 0 : pol = 0, phase = 0"
    print "             - mode 3 : pol = 1, phase = 1"
    print "             modes 1 and 2 are not supported"
    print "  FILENAME  is the Intel Hex Record file that"
    print "            contains the data to be sent to the"
    print "            SPI EEPROM"
    sys.exit()

device = sys.argv[1]
port   = int(sys.argv[2])
mode   = int(sys.argv[3])
file   = sys.argv[4]

# Test for valid mode
if (mode != 0) and (mode != 1):
    print "Mode %d is not supported" % mode
    sys.exit()

# Test for valid device
if (not DEVICES.has_key(device)):
    print "%s is not a supported device" % device
    sys.exit()

(max_size, page_size) = DEVICES[device]
    
# Try to open file
try:
    f=open(file, 'r')
except:
    print "Unable to open file '" + file + "'"
    sys.exit()

# Open the Aardvark adapter
handle = aa_open(port)
if (handle <= 0):
    print "Unable to open Aardvark device on port %d" % port
    print "Error code = %d" % handle
    sys.exit()

    
# Ensure that the SPI subsystem is enabled
aa_configure(handle,  AA_CONFIG_SPI_I2C)
    
# Power the EEPROM using the Aardvark adapter's power supply.
# This command is only effective on v2.0 hardware or greater.
# The power pins on the v1.02 hardware are not enabled by default.
aa_target_power(handle, AA_TARGET_POWER_BOTH)

# Setup the clock phase
aa_spi_configure(handle, mode >> 1, mode & 1, AA_SPI_BITORDER_MSB)

# Set the bitrate
bitrate = aa_spi_bitrate(handle, SPI_BITRATE)

# Create a 64k array of 0xFF
data_in = array('B', [ 0xff for i in range(65535) ])

# Parse file
#
# Read each line of the hex file and verify
print "Reading File: %s..." % file

line_it = 1;
while 1:
    line = f.readline()

    # If empty, then end of file
    if (len(line) == 0):
        break

    # Strip newline, linefeed and whitespace
    line = line.strip()
    
    # If empty, after strip, then it is simply a newline.
    if (len(line) == 0):
        continue

    # Strip colon
    line = line[1:]
    
    # Parse line length
    line_length = int(line[0:2], 16)
    
    # Verify line length
    if (len(line) != line_length*2 + 2 + 4 + 2 + 2):
        print "Error in line %d: Length mismatch" % line_it
        sys.exit()
        
    # Verify line checksum
    line_check = 0
    for x in range(len(line)/2):
        line_check += int(line[x*2:x*2+2], 16)
    if (line_check&0xff != 0):
        print "Error in line %d: Line Checksum Error" % line_it
        sys.exit()
        
    line_addr   = int(line[2:6], 16)
    line_type   = int(line[6:8], 16)
    
    # Verify type
    if (line_type > 1):
        print "Error in line %d: Unsupported hex-record type" % line_it
        sys.exit()
    
    line_data   = line[8:-2]

    # Populate the data_in array
    if (line_type == 0):
        for x in range(line_length):
            data_in[line_addr + x] = int(line_data[x*2:x*2+2], 16)
        
    # Increment iterator
    line_it += 1

# Truncate the data_in to the maximum size for the EEPROM
data_in = data_in[:DEVICES[device][0]]

# Generate Checksum of data
checksum = 0;
for x in range(len(data_in)):
    checksum += data_in[x]
    
print "Checksum: 0x%x" % checksum

print "Writing EEPROM..."
_writeMemory(handle, device, data_in)

print "Reading EEPROM... pass 1"
test1 = _readMemory(handle, 0, max_size)

if data_in == test1:
    print "...PASSED"
else:
    print "...FAILED"

print "Reading EEPROM... pass 2"
test2 = _readMemory(handle, 0, max_size)

if data_in == test2:
    print "...PASSED"
else:
    print "...FAILED"

# Close the device
aa_close(handle)
