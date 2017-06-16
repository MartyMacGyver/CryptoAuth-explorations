#!/bin/env python
#==========================================================================
# (c) 2004  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aai2c_eeprom.py
#--------------------------------------------------------------------------
# Perform simple read and write operations to an I2C EEPROM device.
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
PAGE_SIZE   = 8
BUS_TIMEOUT = 150  # ms


#==========================================================================
# FUNCTIONS
#==========================================================================
def _writeMemory (handle, device, addr, length, zero):
    # Write to the I2C EEPROM
    #
    # The AT24C02 EEPROM has 8 byte pages.  Data can written
    # in pages, to reduce the number of overall I2C transactions
    # executed through the Aardvark adapter.
    n = 0
    while (n < length):
        data_out = array('B', [ 0 for i in range(1+PAGE_SIZE) ])

        # Fill the packet with data
        data_out[0] = addr & 0xff
        
        # Assemble a page of data
        i = 1
        while 1:
            if not (zero): data_out[i] = n & 0xff

            addr = addr + 1
            n = n +1
            i = i+1

            if not (n < length and (addr & (PAGE_SIZE-1)) ): break
        
        # Truncate the array to the exact data size
        del data_out[i:]

        # Write the address and data
        aa_i2c_write(handle, device, AA_I2C_NO_FLAGS, data_out)
        aa_sleep_ms(10)


def _readMemory (handle, device, addr, length):
    # Write the address
    aa_i2c_write(handle, device, AA_I2C_NO_STOP, array('B', [addr & 0xff]))

    (count, data_in) = aa_i2c_read(handle, device, AA_I2C_NO_FLAGS, length)
    if (count < 0):
        print "error: %s" % aa_status_string(count)
        return
    elif (count == 0):
        print "error: no bytes read"
        print "  are you sure you have the right slave address?"
        return
    elif (count != length):
        print "error: read %d bytes (expected %d)" % (count, length)

    # Dump the data to the screen
    sys.stdout.write("\nData read from device:")
    for i in range(count):
        if ((i&0x0f) == 0):
            sys.stdout.write("\n%04x:  " % (addr+i))

        sys.stdout.write("%02x " % (data_in[i] & 0xff))
        if (((i+1)&0x07) == 0):
            sys.stdout.write(" ")

    sys.stdout.write("\n")



#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 7):
    print "usage: aai2c_eeprom PORT BITRATE read  SLAVE_ADDR OFFSET LENGTH"
    print "usage: aai2c_eeprom PORT BITRATE write SLAVE_ADDR OFFSET LENGTH"
    print "usage: aai2c_eeprom PORT BITRATE zero  SLAVE_ADDR OFFSET LENGTH"
    sys.exit()

port    = int(sys.argv[1])
bitrate = int(sys.argv[2])
command = sys.argv[3]
device  = int(sys.argv[4], 0)
addr    = int(sys.argv[5], 0)
length  = int(sys.argv[6])

handle = aa_open(port)
if (handle <= 0):
    print "Unable to open Aardvark device on port %d" % port
    print "Error code = %d" % handle
    sys.exit()
    
# Ensure that the I2C subsystem is enabled
aa_configure(handle,  AA_CONFIG_SPI_I2C)
    
# Enable the I2C bus pullup resistors (2.2k resistors).
# This command is only effective on v2.0 hardware or greater.
# The pullup resistors on the v1.02 hardware are enabled by default.
aa_i2c_pullup(handle, AA_I2C_PULLUP_BOTH)

# Power the EEPROM using the Aardvark adapter's power supply.
# This command is only effective on v2.0 hardware or greater.
# The power pins on the v1.02 hardware are not enabled by default.
aa_target_power(handle, AA_TARGET_POWER_BOTH)

# Set the bitrate
bitrate = aa_i2c_bitrate(handle, bitrate)
print "Bitrate set to %d kHz" % bitrate

# Set the bus lock timeout
bus_timeout = aa_i2c_bus_timeout(handle, BUS_TIMEOUT)
print "Bus lock timeout set to %d ms" % bus_timeout

# Perform the operation
if (command == "write"):
    _writeMemory(handle, device, addr, length, 0)
    print "Wrote to EEPROM"

elif (command == "read"):
    _readMemory(handle, device, addr, length)

elif (command == "zero"):
    _writeMemory(handle, device, addr, length, 1)
    print "Zeroed EEPROM"

else:
    print "unknown command: %s" % command
    
# Close the device
aa_close(handle)
