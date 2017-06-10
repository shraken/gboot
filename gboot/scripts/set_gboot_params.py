#!/usr/bin/python

'''
    Script to information inside a gboot bootload image.  Allows
    the user to set config, ext_info, MCU, pcbrev, PID, serial, and
    version information inside the bootload hex image.
'''

import argparse
import sys
import os
import struct

GBOOT_CONFIG_OFFSET   = 0x2e    # (0x2e - 0x32) 4 byte
GBOOT_EXT_INFO_OFFSET = 0x1e    # (0x1e - 0x22) 4 byte
GBOOT_MCU_OFFSET      = 0x08    # (0x08 - 0x0a) 2 byte
GBOOT_PCB_REV_OFFSET  = 0x19    # (0x19 - 0x1a) 1 byte
GBOOT_PID_OFFSET      = 0x16    # (0x16 - 0x18) 2 byte
GBOOT_SERIAL_OFFSET   = 0x26    # (0x26 - 0x2a) 4 byte
GBOOT_VER_OFFSET      = 0x18    # (0x18 - 0x19) 1 byte

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Set core parameters"
                                        "inside a gboot bootloadable"   
                                        "hex image")
    parser.add_argument("-f,--file", nargs=1, dest="hex_file",
                        help="gboot hex file to be modified", required=True)
    parser.add_argument("-c,--config", nargs=1, dest="config",
                        help="set product dword", required=False)
    parser.add_argument("-e,--ext_info", nargs=1, dest="ext_info",
                        help="set external info dword", required=False)
    parser.add_argument("-m,--mcu", nargs=1, dest="mcu",
                        help="set MCU type word", required=False)
    parser.add_argument("-r,--pcb_rev", nargs=1, dest="pcb_rev",
                        help="set PCB revision byte", required=False)
    parser.add_argument("-p,--pid", nargs=1, dest="pid",
                        help="set USB PID word", required=False)
    parser.add_argument("-s,--serial", nargs=1, dest="serial",
                        help="set product serial dword", required=False)
    parser.add_argument("-v,--version", nargs=1, dest="version",
                        help="set product version dword", required=False)
    args = parser.parse_args()

    if not os.path.isfile(args.hex_file[0]):
        print '==> error: gboot image could not be opened or does not exist.'
        sys.exit(1)
    else:
        f = open(args.hex_file[0], 'r+b')

    if args.config:
        print '==> info: update config item at offset 0x%x' % (GBOOT_CONFIG_OFFSET)
        config_value = struct.pack('<L', int(args.config[0]))
        f.seek(GBOOT_CONFIG_OFFSET)
        f.write(config_value)

    if args.ext_info:
        print '==> info: update ext info item at offset 0x%x' % (GBOOT_EXT_INFO_OFFSET)
        ext_item_value = struct.pack('<L', int(args.ext_info[0]))
        f.seek(GBOOT_EXT_INFO_OFFSET)
        f.write(ext_item_value)

    if args.mcu:
        print '==> info: update MCU item at offset 0x%x' % (GBOOT_MCU_OFFSET)
        mcu_value = struct.pack('<H', int(args.mcu[0]))
        f.seek(GBOOT_MCU_OFFSET)
        f.write(mcu_value)

    if args.pcb_rev:
        print '==> info: update PCB revision item at offset 0x%x' % (GBOOT_PCB_REV_OFFSET)
        pcb_rev_value = struct.pack('<B', int(args.pcb_rev[0]))
        f.seek(GBOOT_PCB_REV_OFFSET)
        f.write(pcb_rev_value)

    if args.pid:
        print '==> info: update USB PID item at offset 0x%x' % (GBOOT_PID_OFFSET)
        pcb_rev_value = struct.pack('<H', int(args.pid[0]))
        f.seek(GBOOT_PID_OFFSET)
        f.write(pcb_rev_value)

    if args.serial:
        print '==> info: update Serial ID item at offset 0x%x' % (GBOOT_SERIAL_OFFSET)
        serial_value = struct.pack('<L', int(args.serial[0]))
        f.seek(GBOOT_SERIAL_OFFSET)
        f.write(serial_value)

    if args.version:
        print '==> info: update Version item at offset 0x%x' % (GBOOT_VER_OFFSET)
        version_value = struct.pack('<B', int(args.version[0]))
        f.seek(GBOOT_VER_OFFSET)
        f.write(version_value)

    print '==> info: finished updating gboot bootload image'
    f.close()