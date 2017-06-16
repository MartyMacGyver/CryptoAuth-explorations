#!/bin/env python
#==========================================================================
# (c) 2004  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aaspi_file.c
#--------------------------------------------------------------------------
# Configure the device as an SPI master and send data.
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
BUFFER_SIZE = 2048
SPI_BITRATE = 1000


#==========================================================================
# FUNCTIONS
#==========================================================================
def blast_bytes (handle, filename):
    # Open the file
    try:
        f=open(filename, 'rb')
    except:
        print "Unable to open file '" + filename + "'"
        return

    trans_num = 0
    while 1:
        # Read from the file
        filedata = f.read(BUFFER_SIZE)
        if (len(filedata) == 0):
            break

        # Write the data to the bus
        data_out = array('B', filedata)
        data_in  = array_u08(len(data_out))
        (count, data_in) = aa_spi_write(handle, data_out, data_in)

        if (count < 0):
            print "error: %s" % aa_status_string(count)
            break
        elif (count != len(data_out)):
            print "error: only a partial number of bytes written"
            print "  (%d) instead of full (%d)" % (count, num_write)

        sys.stdout.write("*** Transaction #%02d\n" % trans_num)
        sys.stdout.write("Data written to device:")
        for i in range(count):
            if ((i&0x0f) == 0):
                sys.stdout.write("\n%04x:  " % i)

            sys.stdout.write("%02x " % (data_out[i] & 0xff))
            if (((i+1)&0x07) == 0):
                sys.stdout.write(" ")

        sys.stdout.write("\n\n")

        sys.stdout.write("Data read from device:")
        for i in range(count):
            if ((i&0x0f) == 0):
                sys.stdout.write("\n%04x:  " % i)

            sys.stdout.write("%02x " % (data_in[i] & 0xff))
            if (((i+1)&0x07) == 0):
                sys.stdout.write(" ")

        sys.stdout.write("\n\n")

        trans_num = trans_num + 1

        # Sleep a tad to make sure slave has time to process this request
        aa_sleep_ms(10)

    f.close()


#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 4):
    print "usage: aaspi_file PORT MODE filename"
    print "  mode 0 : pol = 0, phase = 0"
    print "  mode 1 : pol = 0, phase = 1"
    print "  mode 2 : pol = 1, phase = 0"
    print "  mode 3 : pol = 1, phase = 1"
    print ""
    print "  'filename' should contain data to be sent"
    print "  to the downstream spi device"
    sys.exit()

port     = int(sys.argv[1])
mode     = int(sys.argv[2])
filename = sys.argv[3]

handle = aa_open(port)
if (handle <= 0):
    print "Unable to open Aardvark device on port %d" % port
    print "Error code = %d" % handle
    sys.exit()

# Ensure that the SPI subsystem is enabled
aa_configure(handle,  AA_CONFIG_SPI_I2C)

# Enable the Aardvark adapter's power supply.
# This command is only effective on v2.0 hardware or greater.
# The power pins on the v1.02 hardware are not enabled by default.
aa_target_power(handle, AA_TARGET_POWER_BOTH)

# Setup the clock phase
aa_spi_configure(handle, mode >> 1, mode & 1, AA_SPI_BITORDER_MSB)

# Set the bitrate
bitrate = aa_spi_bitrate(handle, SPI_BITRATE)
print "Bitrate set to %d kHz" % bitrate

blast_bytes(handle, filename)

# Close the device
aa_close(handle)
