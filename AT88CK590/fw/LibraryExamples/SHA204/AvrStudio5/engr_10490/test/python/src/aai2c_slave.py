#!/bin/env python
#==========================================================================
# (c) 2004  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aai2c_slave.py
#--------------------------------------------------------------------------
# Configure the device as an I2C slave and watch incoming data.
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

INTERVAL_TIMEOUT =   500


#==========================================================================
# FUNCTIONS
#==========================================================================
def dump (handle, timeout_ms):
    print "Watching slave I2C data..."

    # Wait for data on bus
    result = aa_async_poll(handle, timeout_ms)
    if (result == AA_ASYNC_NO_DATA):
        print "No data available."
        return

    print ""
    
    trans_num = 0

    # Loop until aa_async_poll times out
    while 1:
        # Read the I2C message.
        # This function has an internal timeout (see datasheet), though
        # since we have already checked for data using aa_async_poll,
        # the timeout should never be exercised.
        if (result == AA_ASYNC_I2C_READ):
            # Get data written by master
            (num_bytes, addr, data_in) = aa_i2c_slave_read(handle, BUFFER_SIZE)

            if (num_bytes < 0):
                print "error: %s" % aa_status_string(num_read)
                return

            # Dump the data to the screen
            sys.stdout.write("*** Transaction #%02d\n" % trans_num)
            sys.stdout.write("Data read from device:")
            for i in range(num_bytes):
                if ((i&0x0f) == 0):
                    sys.stdout.write("\n%04x:  " % i)

                sys.stdout.write("%02x " % (data_in[i] & 0xff))
                if (((i+1)&0x07) == 0):
                    sys.stdout.write(" ")

            sys.stdout.write("\n\n")

        elif (result == AA_ASYNC_I2C_WRITE):
            # Get number of bytes written to master
            num_bytes = aa_i2c_slave_write_stats(handle)

            if (num_bytes < 0):
                print "error: %s" % aa_status_string(num_read)
                return

            # Print status information to the screen
            print "*** Transaction #%02d" % trans_num
            print "Number of bytes written to master: %04d" % num_bytes
            print ""

        else:
            print "error: non-I2C asynchronous message is pending"
            return
        
        trans_num = trans_num +1
        
        # Use aa_async_poll to wait for the next transaction
        result = aa_async_poll(handle, INTERVAL_TIMEOUT)
        if (result == AA_ASYNC_NO_DATA):
            print "No more data available from I2C master."
            break

        
            

#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 4):
    print "usage: aai2c_slave PORT SLAVE_ADDR TIMEOUT_MS"
    print "  SLAVE_ADDR is the slave address for this device"
    print ""
    print "  The timeout value specifies the time to"
    print "  block until the first packet is received."
    print "  If the timeout is -1, the program will"
    print "  block indefinitely."
    sys.exit()

port       = int(sys.argv[1])
addr       = int(sys.argv[2], 0)
timeout_ms = int(sys.argv[3])

# Open the device
handle = aa_open(port)
if (handle <= 0):
    print "Unable to open Aardvark device on port %d" % port
    print "Error code = %d" % handle
    sys.exit()
    
# Ensure that the I2C subsystem is enabled
aa_configure(handle,  AA_CONFIG_SPI_I2C)
    
# Disable the I2C bus pullup resistors (2.2k resistors).
# This command is only effective on v2.0 hardware or greater.
# The pullup resistors on the v1.02 hardware are enabled by default.
aa_i2c_pullup(handle, AA_I2C_PULLUP_NONE)

# Power the EEPROM using the Aardvark adapter's power supply.
# This command is only effective on v2.0 hardware or greater.
# The power pins on the v1.02 hardware are not enabled by default.
aa_target_power(handle, AA_TARGET_POWER_BOTH)

# Set the slave response
slave_resp = array('B', [ 0 for i in range(SLAVE_RESP_SIZE) ])
for i in range(SLAVE_RESP_SIZE):
    slave_resp[i] = ord('A') + i
aa_i2c_slave_set_response(handle, slave_resp)

# Enable the slave
aa_i2c_slave_enable(handle, addr, 0, 0)

# Watch the I2C port
dump(handle, timeout_ms)

# Disable the slave and close the device
aa_i2c_slave_disable(handle)
aa_close(handle)
