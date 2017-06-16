#!/bin/env python
#==========================================================================
# (c) 2004  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aaspi_slave.py
#--------------------------------------------------------------------------
# Configure the device as an SPI slave and watch incoming data.
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
BUFFER_SIZE      = 65535

SLAVE_RESP_SIZE  =    26


#==========================================================================
# FUNCTIONS
#==========================================================================
def dump (handle, timeout_ms):
    print "Watching slave SPI data..."

    # Wait for data on bus
    result = aa_async_poll(handle, timeout_ms)
    if (result != AA_ASYNC_SPI):
        print "No data available."
        return
    
    print ""
    
    trans_num = 0

    # Loop until aa_spi_slave_read times out
    while 1:
        # Read the SPI message.
        # This function has an internal timeout (see datasheet).
        # To use a variable timeout the function aa_async_poll could
        # be used for subsequent messages.
        (num_read, data_in) = aa_spi_slave_read(handle, BUFFER_SIZE)

        if (num_read < 0 and num_read != AA_SPI_SLAVE_TIMEOUT):
            print "error: %s" % aa_status_string(num_read)
            return

        elif (num_read == 0 or num_read == AA_SPI_SLAVE_TIMEOUT):
            print "No more data available from SPI master"
            return

        else:
            # Dump the data to the screen
            sys.stdout.write("*** Transaction #%02d\n" % trans_num)
            sys.stdout.write("Data read from device:")
            for i in range(num_read):
                if ((i&0x0f) == 0):
                    sys.stdout.write("\n%04x:  " % i)

                sys.stdout.write("%02x " % (data_in[i] & 0xff))
                if (((i+1)&0x07) == 0):
                    sys.stdout.write(" ")

            sys.stdout.write("\n\n")

            trans_num = trans_num +1


#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 4):
    print "usage: aaspi_slave PORT MODE TIMEOUT_MS"
    print "  mode 0 : pol = 0, phase = 0"
    print "  mode 1 : pol = 0, phase = 1"
    print "  mode 2 : pol = 1, phase = 0"
    print "  mode 3 : pol = 1, phase = 1"
    print ""
    print "  The timeout value specifies the time to"
    print "  block until the first packet is received."
    print "  If the timeout is -1, the program will"
    print "  block indefinitely."
    sys.exit()

port       = int(sys.argv[1])
mode       = int(sys.argv[2])
timeout_ms = int(sys.argv[3])

handle = aa_open(port)
if (handle <= 0):
    print "Unable to open Aardvark device on port %d" % port
    print "Error code = %d" % handle
    sys.exit()
    
# Ensure that the SPI subsystem is enabled
aa_configure(handle,  AA_CONFIG_SPI_I2C)
    
# Disable the Aardvark adapter's power pins.
# This command is only effective on v2.0 hardware or greater.
# The power pins on the v1.02 hardware are not enabled by default.
aa_target_power(handle, AA_TARGET_POWER_NONE)

# Setup the clock phase
aa_spi_configure(handle, mode >> 1, mode & 1, AA_SPI_BITORDER_MSB)

# Set the slave response
slave_resp = array('B', [ 0 for i in range(SLAVE_RESP_SIZE) ])
for i in range(SLAVE_RESP_SIZE):
    slave_resp[i] = ord('A') + i
aa_spi_slave_set_response(handle, slave_resp)

# Enable the slave
aa_spi_slave_enable(handle)

# Watch the SPI port
dump(handle, timeout_ms)

# Disable the slave and close the device
aa_spi_slave_disable(handle)
aa_close(handle)
