#!/usr/bin/env python3

"""
    A sample USB connector program to talk to the AT88CK590 dev kit
    Especially useful when modding firmware


    Copyright (c) 2017 Martin F. Falatic

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

"""

from __future__ import print_function
import usb.core
import usb.util
import sys
import time

def init_device(vendor_id, product_id):
    device = usb.core.find(idVendor=vendor_id, idProduct=product_id)
    if device is None:
        raise ValueError('Device not found')
    usb.util.dispose_resources(device)
    # set the active configuration. With no arguments, the first
    # configuration will be the active one
    device.set_configuration()
    return device

def get_device_config(device):
    config = device.get_active_configuration()
    return config

def get_endpoint_iface(device, config):
    # get an endpoint instance
    intf = config[(0,0)]
    interface_number = config[(0, 0)].bInterfaceNumber
    usb.util.claim_interface(device, interface_number)
    alternate_setting = usb.control.get_interface(device, interface_number)
    interface = usb.util.find_descriptor(
        config,
        bInterfaceNumber=interface_number,
        bAlternateSetting=alternate_setting)
    return interface

def get_endpoint_dir(interface, direction):
    usb_endpoint = usb.util.find_descriptor(interface,
        custom_match=lambda e:
            usb.util.endpoint_direction(e.bEndpointAddress) == direction
    )
    if usb_endpoint is None:  # check we have a real endpoint handle
        raise IOError("Could not find the requested endpoint")
    return usb_endpoint

def send_bytes(endp_out, out_bytes, soften_errors=False):
    print("Send: {}".format(out_bytes))
    try:
        endp_out.write(list(out_bytes))
    except usb.core.USBError as e:
        if soften_errors:
            print("USB Error: {}".format(str(e)))
            return
        else:
            raise IOError("USB Error: {}".format(str(e)))

def recv_bytes(endp_in, soften_errors=False):
    resp = ''
    while True:
        try:
            resp = endp_in.read(64, 50)
            break
        except usb.core.USBError as e:
            if str(e).find("timeout") >= 0:
                pass
            elif soften_errors:
                print("USB Error: {}".format(str(e)))
                return
            else:
                raise IOError("USB Error: {}".format(str(e)))
    # Trim trailing NUL chars, if any
    i = len(resp) + 1
    for c in reversed(resp):
        if c != 0:
            break
        i -= 1
    if i > len(resp):
        print("Recv: {}".format(resp.tobytes()))
    else:
        print("Recv: {}...".format(resp[0:i-1].tobytes()))

def scanner(dev):
    #for i in range(0, 256):
    for i in [2]: # 9 works but breaks things
        try:
            #res = dev.ctrl_transfer(0b00000100, i, 0, 0, 42)
            res = dev.ctrl_transfer(0b00000010, i, 0, 0, 42)
            res = dev.ctrl_transfer(0b10000000, i, 0, 0, 42)
            print(i)
        except usb.core.USBError as e:
            pass
    ##    return dev.ctrl_transfer(
    ##            bmRequestType = bmRequestType,
    ##            bRequest = 0x06,
    ##            wValue = wValue,
    ##            wIndex = wIndex,
    ##            data_or_wLength = desc_size)

def test_and_reset():
    ''' Run a sequence of commands to exercise modded firmware'''
    print()
    print("Command test")
    print()

    dev = init_device(vendor_id=0x03eb, product_id=0x2312)
    #print(dev)
    #print()

    cfg = get_device_config(dev)
    #print(cfg)
    #print()

    iface = get_endpoint_iface(dev, cfg)

    usb_endpoint_in  = get_endpoint_dir(iface, usb.util.ENDPOINT_IN)
    print("Endpoint (IN) : 0x{0:02x}".format(usb_endpoint_in.bEndpointAddress))
    #print(usb_endpoint_in)
    print()
    usb_endpoint_out = get_endpoint_dir(iface, usb.util.ENDPOINT_OUT)
    print("Endpoint (OUT): 0x{0:02x}".format(usb_endpoint_out.bEndpointAddress))
    #print(usb_endpoint_out)
    print()

    send_bytes(usb_endpoint_out, b'()\n')
    recv_bytes(usb_endpoint_in)
    print()

    send_bytes(usb_endpoint_out, b'board:noop()\n')
    recv_bytes(usb_endpoint_in)
    print()

    send_bytes(usb_endpoint_out, b'board:version()\n')
    recv_bytes(usb_endpoint_in)
    print()

    send_bytes(usb_endpoint_out, b'board:firmware(00)\n')
    recv_bytes(usb_endpoint_in)
    print()

    send_bytes(usb_endpoint_out, b'board:firmware(01)\n')
    recv_bytes(usb_endpoint_in)
    print()

    send_bytes(usb_endpoint_out, b'board:firmware(02)\n')
    recv_bytes(usb_endpoint_in)
    print()

    print("Send a NOOP command (modded firmware)")
    send_bytes(usb_endpoint_out, b'board:cmd(00)\n')
    recv_bytes(usb_endpoint_in)
    print()

    print("Send commanded restart (modded firmware)")
    send_bytes(usb_endpoint_out, b'board:cmd(01)\n')
    # Will not return data
    print()

    print("Send commanded DFU mode (modded firmware)")
    send_bytes(usb_endpoint_out, b'board:cmd(02)\n')
    # Will not return data
    print()

    usb.util.dispose_resources(dev)

if __name__ == '__main__':
    test_and_reset()
    time.sleep(5)

    test_and_reset()
    time.sleep(5)

    sys.exit(0)
