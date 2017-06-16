#!/bin/env python
#==========================================================================
#--------------------------------------------------------------------------
# Project : I2C-to-SWI bridge tests
# File    : aabridge_test.py
#--------------------------------------------------------------------------
# Test the I2C-to-SWI bridge.
#--------------------------------------------------------------------------

#==========================================================================
# IMPORTS
#==========================================================================
import sys
from aardvark_py import aa_open, aa_close, aa_i2c_read, aa_i2c_write, \
                        aa_sleep_ms, aa_status_string, array, \
                        aa_configure, aa_i2c_pullup, aa_i2c_bitrate, aa_target_power, \
                        AA_I2C_NO_FLAGS, AA_CONFIG_SPI_I2C, AA_I2C_PULLUP_BOTH, AA_TARGET_POWER_BOTH


#==========================================================================
# CONSTANTS
#==========================================================================
I2C_BITRATE = 400 # kHz
I2C_ADDRESS = 0x64


#==========================================================================
# FUNCTIONS
#==========================================================================


def log(data, count):
    """This function displays the data as hex-ascii."""
    for i in range(count):
        sys.stdout.write("%02X " % data[i])
        # Insert an additional space every eight bytes.
        if (((i + 1) & 0x07) == 0):
            sys.stdout.write(" ")
    sys.stdout.write("\n")


def log_write(data):
    """This function displays write data as hex-ascii."""
    sys.stdout.write("tx: ")
    count = len(data)
    log(data, count)


def log_read(data, count):
    """This function displays read data as hex-ascii."""
    sys.stdout.write("rx: ")
    log(data, count)
    print
    

def write(handle, data_out):
    """This function sends a command to the SHA204 device."""
    log_write(data_out)
    res = aa_i2c_write(handle, I2C_ADDRESS, AA_I2C_NO_FLAGS, data_out)
    if (res < 0): 
        print "error: %s" % (aa_status_string(res))
        return res
    
    if (res == 0):
        print "error: slave device %02X not found" % (I2C_ADDRESS)
    
    return res


def read(handle, rx_length):
    """This function reads a response from the SHA204 device.
       One iteration in the response-ready polling loop takes 1 ms
       plus the time for one loop iteration."""
    poll_interval = 1 # approx. 1 ms
    timeout = 1000 # approx. 1000 ms
    count = 0
    data_in = None
    res = 1
    
    while timeout > 0:
        (count, data_in) = aa_i2c_read(handle, I2C_ADDRESS, AA_I2C_NO_FLAGS, rx_length)
        if count > 0:
            # success
            break
        if count < 0:
            print "error: %s\n" % (aa_status_string(count))
            return count
        if count == 0:
            aa_sleep_ms(poll_interval)
            timeout -= poll_interval
            continue
        if (count != rx_length):
            print "error: read %d bytes (expected %d)\n" % (count, rx_length)
            res = -1
            break

    # Dump the data to the screen
    log_read(data_in, count)
    return res


def get_version(handle):
    """This function reads the firmware version from the I2C-to-SWI Bridge."""
    print "Get firmware version of Bridge"
    res = write(handle, array('B', [0x04, 0x00, 0x03]))
    if res <= 0:
        return res
    return read(handle, 5)


def dev_rev(handle):
    """This function sends a DevRev command to the SHA204 device and receives its response."""
    print "Send DevRev command to SHA204 and receive its response"    
    res = write(handle, array('B', [0x00, 0x07, 0x07, 0x07, 0x30, 0x00, 0x00, 0x00, 0x03, 0x5D]))
    if res <= 0:
        return res
    return read(handle, 9)


def write_config(handle):
    """This functions writes data to the SHA204 configuration zone."""
    print "Send Write command to SHA204 and receive its response"    
    res = write(handle, array('B', [0x00, 0x27, 0x04, \
                              0x27, 0x12, 0x80, 0x08, 0x00, \
                              0x86, 0x40, 0x87, 0x07, 0x0F, 0x00, 0x89, 0xF2, \
                              0x8A, 0x7A, 0x0B, 0x8B, 0x0C, 0x4C, 0xDD, 0x4D, \
                              0xC2, 0x42, 0xAF, 0x8F, 0xFF, 0x00, 0xFF, 0x00, \
                              0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, \
                              0x05, 0x2A]))
    if res <= 0:
        return res
    return read(handle, 6)


def read_config(handle):
    """This functions reads data from the SHA204 configuration zone."""
    print "Send Read command to SHA204 and receive its response"    
    res = write(handle, array('B', [0x00, 0x07, 0x23, 0x07, 0x02, 0x80, 0x08, 0x00, 0x0A, 0x4D]))
    if res <= 0:
        return res
    return read(handle, 37)


def nonce(handle):
    """This function sends a Nonce command to the SHA204 device and receives its response."""
    print "Send Nonce command to SHA204 and receive its response"    
    res = write(handle, array('B', [0x00, 0x1B, 0x23, \
                              0x1B, 0x16, 0x00, 0x00, 0x00, \
                              0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, \
                              0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, \
                              0x98, 0xC6]))
    if res <= 0:
        return res
    return read(handle, 37)

def mac(handle):
    """This function sends a Mac command to the SHA204 device and receives its response."""
    print "Send Mac command to SHA204 and receive its response"    
    res = write(handle, array('B', [0x00, 0x27, 0x23, \
                              0x27, 0x08, 0x00, 0x00, 0x00, \
                              0x3C, 0x37, 0x0E, 0x25, 0x98, 0x43, 0x8E, 0x6C, \
                              0xE9, 0x8A, 0x9B, 0x1E, 0xBD, 0xB3, 0x1E, 0x7D, \
                              0x8C, 0xBD, 0x38, 0xE3, 0x25, 0x06, 0xBB, 0x40, \
                              0x19, 0x68, 0x9C, 0x34, 0x49, 0x8B, 0x1A, 0x97, \
                              0x68, 0x1B]))
    if res <= 0:
        return res
    return read(handle, 37)


def set_swi_timeout(handle, timeout):
    """This function sets the SWI timeout (ms)."""
    timeout %= 2550
    print "Set the SWI timeout to %d ms" % (timeout)    
    res = write(handle, array('B', [0x03, 0x01, 0x00, timeout / 10]))
    if res <= 0:
        return res
    return read(handle, 2)


def set_wakeup_delay(handle, delay):
    """This function sets the SWI Wakeup delay (us)."""
    delay %= 25500
    print "Set the SWI Wakeup delay to %d us" % (delay)    
    res = write(handle, array('B', [0x01, 0x01, 0x00, delay / 100]))
    if res <= 0:
        return res
    return read(handle, 2)
    

#==========================================================================
# MAIN PROGRAM
#==========================================================================
port = 0

# Open the device.
handle = aa_open(port)
if (handle <= 0):
    print "Unable to open Aardvark device on port %d" % port
    print "Error code = %d" % handle
    sys.exit()

# Enable  I2C subsystem.
aa_configure(handle,  AA_CONFIG_SPI_I2C)
    
# Enable the I2C bus pullup resistors (2.2k resistors).
#aa_i2c_pullup(handle, AA_I2C_PULLUP_BOTH)

# Power the board using the Aardvark adapter's power supply.
aa_target_power(handle, AA_TARGET_POWER_BOTH)

# Set the bitrate.
aa_i2c_bitrate(handle, I2C_BITRATE)

# positive tests
if get_version(handle) <= 0:
    exit()

exit()


if dev_rev(handle) <= 0:
    exit()

if write_config(handle) <= 0:
    exit()

if read_config(handle) <= 0:
    exit()

if nonce(handle) <= 0:
    exit()

if mac(handle) <= 0:
    exit()


# Test this with an oscilloscope or Beagle protocol analyzer.
if set_wakeup_delay(handle, 20000) <= 0:
    exit()

if dev_rev(handle) <= 0:
    exit()

if set_wakeup_delay(handle, 1000) <= 0:
    exit()

if dev_rev(handle) <= 0:
    exit()


#negative tests
# Decrease SWI timeout so that Nonce command fails.
set_swi_timeout(handle, 10)   
nonce(handle)

# We forced a premature timeout. Therefore the device did not
# execute the Idle command the firmware has sent.
# Wait for the device to go into Sleep mode.
aa_sleep_ms(1500)

# Reverse SWI timeout.
set_swi_timeout(handle, 500)
nonce(handle)
mac(handle)

# Close the device and exit
aa_close(handle)
