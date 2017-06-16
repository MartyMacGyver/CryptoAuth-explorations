#!/bin/env python
#==========================================================================
# (c) 2004-2005  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aamonitor_filtered.py
#--------------------------------------------------------------------------
# Perform I2C monitoring functions with the Aardvark I2C/SPI adapter with
# the ability to filter the data based on slave address.
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
import sys, time

from aardvark_py import *


#==========================================================================
# CONSTANTS
#==========================================================================
BUFFER_SIZE = 32767

TIMEFORMAT  = "%Y-%m-%d %H:%M:%S"

#==========================================================================
# FUNCTIONS
#==========================================================================
def dump (handle, filter_addr, timeout):
    # Wait for data on the bus
    print "Waiting %d ms for first transaction..." % timeout
    print "  Filtering on 0x%03x" % filter_addr
    result = aa_async_poll(handle, timeout)
    if (result == AA_ASYNC_NO_DATA):
        print "  no data pending."
        return

    print "  data received"
    
    last_data0 = 0
    last_data1 = 0

    # Loop until aa_async_poll times out
    while 1:
        # Read the next monitor transaction.
        # This function has an internal timeout (see datasheet), though
        # since we have already checked for data using aa_async_poll,
        # the timeout should never be exercised.
        (status, data) = aa_i2c_monitor_read(handle, BUFFER_SIZE)
        
        if (status < 0):
            print "error: %s" % aa_status_string(status)
            return
        
        # The display flag indicates if the filtered address has been matched
        # and the data should be displayed.
        display        = 0
        
        # The display_buffer is used to hold the start condition because it
        # is sent before the address is known, so the output needs to be 
        # cached to display later.
        display_buffer = ""
        
        for i in range(len(data)):
            if (data[i] == AA_I2C_MONITOR_CMD_START):
                # Generate a timestamp.  This time stamp does not accurately
                # reflect the actual time that the transaction occurred, but 
                # is generated to give the user a relative time for the 
                # transaction.
                fmtstamp = time.strftime(TIMEFORMAT, time.localtime(time.time()))
                
                # Cache the start condition
                display_buffer = "\n%s : [S] " % fmtstamp

            elif (data[i] == AA_I2C_MONITOR_CMD_STOP):
                if display:
                    sys.stdout.write("[P]\n")
                # After a stop condition, reset the display flag for 
                # next message
                display = 0

            else:
                nack = (data[i] & AA_I2C_MONITOR_NACK)
                if nack:  nack_str = "*"
                else:     nack_str = ""
                
                # 7-bit addresses
                if (last_data0 == AA_I2C_MONITOR_CMD_START and
                    ((data[i] & 0xf8) != 0xf0 or nack)):
                        
                    # Test to see if 7-bit address matches
                    if ((data[i] & 0xff) >> 1 == filter_addr):
                        # If the address matches, the set display to 1
                        display = 1
                        # Write out the start condition
                        sys.stdout.write(display_buffer)
                        # And reset the buffer
                        display_buffer = ""
                        
                        # Now process regularly
                        if (data[i] & 0x01): dir_str = "r"
                        else:                dir_str = "w"
                        sys.stdout.write("<%02x:%s>%s " %
                            ((data[i] & 0xff) >> 1,
                            dir_str,
                            nack_str
                            ))

                # 10-bit addresses
                # See Philips specification for more details.
                elif (last_data1 == AA_I2C_MONITOR_CMD_START and
                     (last_data0 & 0xf8) == 0xf0):

                    # Test to see if 10-bit address matches
                    if (((last_data0 << 7) & 0x300) | (data[i] & 0xff)) == filter_addr:
                        
                        # If the address matches, the set display to 1
                        display = 1
                        # Write out the start condition
                        sys.stdout.write(display_buffer)
                        # Reset the buffer
                        display_buffer = ""
                        
                        if (last_data0 & 0x01): dir_str = "r"
                        else:                   dir_str = "w"
                        sys.stdout.write("<%03x:%s>%s " %
                            (((last_data0 << 7) & 0x300) | (data[i] & 0xff),
                            dir_str,
                            nack_str
                            ))

                # Normal data
                elif (last_data0 != AA_I2C_MONITOR_CMD_START):
                    if display:
                        sys.stdout.write("%02x%s " % (data[i] & 0xff, nack_str))

            last_data1 = last_data0
            last_data0 = data[i]
            sys.stdout.flush()

        # print "\nWaiting %d ms for subsequent transaction..." % INTERVAL_TIMEOUT

        # Use aa_async_poll to wait for the next transaction
        result = aa_async_poll(handle, timeout)
        if (result == AA_ASYNC_NO_DATA):
            print "  No more data pending."
            break
            

#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 4):
    print "usage: aamonitor PORT ADDR TIMEOUT"
    print "  where:"
    print "    PORT    is the Aardvark adapter port number"
    print "    ADDR    is the slave address as an integer"
    print "    TIMEOUT is the timeout interval in ms"
    sys.exit()

port        = int(sys.argv[1])
filter_addr = int(sys.argv[2], 0)
timeout     = int(sys.argv[3])

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

# Disable the Aardvark adapter's power pins.
# This command is only effective on v2.0 hardware or greater.
# The power pins on the v1.02 hardware are not enabled by default.
aa_target_power(handle, AA_TARGET_POWER_NONE)

# Enable the monitor
result = aa_i2c_monitor_enable(handle)
if (result < 0):
    print "error: %s\n" % aa_status_string(result)
    sys.exit()
    
print "Enabled I2C monitor."

# Watch the I2C port
dump(handle, filter_addr, timeout)

# Disable the slave and close the device
aa_i2c_monitor_disable(handle)
aa_close(handle)
