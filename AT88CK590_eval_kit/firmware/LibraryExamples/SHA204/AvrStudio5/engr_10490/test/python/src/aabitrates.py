#!/bin/env python
#==========================================================================
# (c) 2006  Total Phase, Inc.
#--------------------------------------------------------------------------
# Project : Aardvark Sample Code
# File    : aabitrates.py
#--------------------------------------------------------------------------
# Determine all possible bitrates on an Aardvark adapter.
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
# PRINT FUNCTION
#==========================================================================
def printBitrates (rates):
    sorted_rates = rates[:]
    sorted_rates.sort()

    print "  ",
    count = 0
    for rate in sorted_rates:
        if count >= 9:
            print "\n  ",
            count = 0
        print "%4d " % rate,
        count += 1

    print "\n"


#==========================================================================
# MAIN PROGRAM
#==========================================================================
if (len(sys.argv) < 2):
    print "usage: aabitrates PORT"
    sys.exit()

port = int(sys.argv[1])

# Open the device
handle = aa_open(port)
if (handle <= 0):
    print "Unable to open Aardvark device on port %d" % port
    print "Error code = %d" % handle
    sys.exit()
    
# Ensure that the I2C and SPI subsystems are enabled
aa_configure(handle, AA_CONFIG_SPI_I2C)

# Determine available I2C bitrates
bitrates = { }
for rate in range(1000):
    bitrate = aa_i2c_bitrate(handle, rate)
    bitrates[bitrate] = 1

# Print the available I2C bitrates
print "I2C Bitrates (kHz):"
printBitrates(bitrates.keys())
    
# Determine available SPI bitrates
bitrates = { }
for rate in range(0, 8000, 25):
    bitrate = aa_spi_bitrate(handle, rate)
    bitrates[bitrate] = 1

# Print the available SPI bitrates
print "SPI Bitrates (kHz):"
printBitrates(bitrates.keys())
    
# Close the device and exit
aa_close(handle)
