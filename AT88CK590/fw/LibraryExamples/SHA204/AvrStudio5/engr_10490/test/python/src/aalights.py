#!/bin/env python
#==========================================================================
# (c) 2004  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aalights.py
#--------------------------------------------------------------------------
# Flash the lights on the Aardvark I2C/SPI Activity Board.
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
I2C_BITRATE = 100 # kHz


#==========================================================================
# FUNCTIONS
#==========================================================================
def flash_lights (handle):
    data_out = array('B', [0, 0])

    # Configure I/O expander lines as outputs
    data_out[0] = 0x03
    data_out[1] = 0x00
    res = aa_i2c_write(handle, 0x38, AA_I2C_NO_FLAGS, data_out)
    if (res < 0): return res
    
    if (res == 0):
        print "error: slave device 0x38 not found"
        return 0
    
    # Turn lights on in sequence
    i = 0xff
    while (i != 0):
        i = ((i<<1) & 0xff)
        data_out[0] = 0x01
        data_out[1] = i
        res = aa_i2c_write(handle, 0x38, AA_I2C_NO_FLAGS, data_out)
        if (res < 0): return res
        aa_sleep_ms(70)
        
    # Leave lights on for 100 ms
    aa_sleep_ms(100)
    
    # Turn lights off in sequence
    i = 0x00
    while (i != 0xff):
        i = ((i<<1) | 0x01)
        data_out[0] = 0x01
        data_out[1] = i
        res = aa_i2c_write(handle, 0x38, AA_I2C_NO_FLAGS, data_out)
        if (res < 0): return res
        aa_sleep_ms(70)

    aa_sleep_ms(100)
        
    # Configure I/O expander lines as inputs
    data_out[0] = 0x03
    data_out[1] = 0xff
    res = aa_i2c_write(handle, 0x38, AA_I2C_NO_FLAGS, data_out)
    if (res < 0): return res

    return 0


#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 2):
    print "usage: aalights PORT"
    sys.exit()

port = int(sys.argv[1])

# Open the device
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

# Power the board using the Aardvark adapter's power supply.
# This command is only effective on v2.0 hardware or greater.
# The power pins on the v1.02 hardware are not enabled by default.
aa_target_power(handle, AA_TARGET_POWER_BOTH)

# Set the bitrate
bitrate = aa_i2c_bitrate(handle, I2C_BITRATE)
print "Bitrate set to %d kHz" % bitrate

res = flash_lights(handle)
if (res < 0):
    print "error: %s" % aa_status_string(res)
    
# Close the device and exit
aa_close(handle)
