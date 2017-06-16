#!/bin/env python
#==========================================================================
# (c) 2004  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aaspi_eeprom.py
#--------------------------------------------------------------------------
# Perform simple read and write operations to an SPI EEPROM device.
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
PAGE_SIZE = 32


#==========================================================================
# FUNCTIONS
#==========================================================================
def _writeMemory (handle, addr, length, zero):
    # Write to the SPI EEPROM
    #
    # The AT25080A EEPROM has 32 byte pages.  Data can written
    # in pages, to reduce the number of overall SPI transactions
    # executed through the Aardvark adapter.
    n = 0
    while (n < length):
        # Send write enable command
        data_out = array('B', [ 0x06 ])
        aa_spi_write(handle, data_out, 0)

        # Assemble write command and address
        data_out = array('B', [ 0 for i in range(3+PAGE_SIZE) ])
        data_out[0] = 0x02
        data_out[1] = (addr >> 8) & 0xff
        data_out[2] = (addr >> 0) & 0xff

        # Assemble a page of data
        i = 3
        while 1:
            if not (zero): data_out[i] = n & 0xff

            addr = addr + 1
            n = n +1
            i = i+1

            if not (n < length and (addr & (PAGE_SIZE-1)) ): break

        # Truncate the array to the exact data size
        del data_out[i:]

        # Write the transaction
        aa_spi_write(handle, data_out, 0)
        aa_sleep_ms(10)


def _readMemory (handle, addr, length):
    data_out = array('B', [ 0 for i in range(3+length) ])
    data_in  = array_u08(3+length)

    # Assemble read command and address
    data_out[0] = 0x03
    data_out[1] = (addr >> 8) & 0xff
    data_out[2] = (addr >> 0) & 0xff

    # Write length+3 bytes for data plus command and 2 address bytes
    (count, data_in) = aa_spi_write(handle, data_out, data_in)

    if (count < 0):
        print "error: %s\n" % aa_status_string(count)
        return
    elif (count != length+3):
        print "error: read %d bytes (expected %d)" % (count-3, length)

    # Dump the data to the screen
    sys.stdout.write("\nData read from device:")
    for i in range(len(data_in)-3):
        if ((i&0x0f) == 0):
            sys.stdout.write("\n%04x:  " % (addr+i))

        sys.stdout.write("%02x " % (data_in[i+3] & 0xff))
        if (((i+1)&0x07) == 0):
            sys.stdout.write(" ")

    sys.stdout.write("\n")



#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 7):
    print "usage: aaspi_eeprom PORT BITRATE read  MODE ADDR LENGTH"
    print "usage: aaspi_eeprom PORT BITRATE write MODE ADDR LENGTH"
    print "usage: aaspi_eeprom PORT BITRATE zero  MODE ADDR LENGTH"
    print "  mode 0 : pol = 0, phase = 0"
    print "  mode 3 : pol = 1, phase = 1"
    print "  modes 1 and 2 are not supported"
    sys.exit()

port    = int(sys.argv[1])
bitrate = int(sys.argv[2])
command = sys.argv[3]
mode    = int(sys.argv[4])
addr    = int(sys.argv[5], 0)
length  = int(sys.argv[6])

# Open the device
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
bitrate = aa_spi_bitrate(handle, bitrate)
print "Bitrate set to %d kHz" % bitrate


# Perform the operation
if (command == "write"):
    _writeMemory(handle, addr, length, 0)
    print "Wrote to EEPROM"

elif (command == "read"):
    _readMemory(handle, addr, length)

elif (command == "zero"):
    _writeMemory(handle, addr, length, 1)
    print "Zeroed EEPROM"

else:
    print "unknown command: %s" % command

# Close the device
aa_close(handle)
