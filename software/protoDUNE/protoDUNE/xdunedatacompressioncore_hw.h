// ==============================================================
// File generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2016.3
// Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
// 
// ==============================================================

// BUS_A
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x10 : Data signal of cfg
//        bit 31~0 - cfg[31:0] (Read/Write)
// 0x14 : Data signal of cfg
//        bit 31~0 - cfg[63:32] (Read/Write)
// 0x18 : Data signal of cfg
//        bit 31~0 - cfg[95:64] (Read/Write)
// 0x1c : Data signal of cfg
//        bit 31~0 - cfg[127:96] (Read/Write)
// 0x20 : Data signal of cfg
//        bit 31~0 - cfg[159:128] (Read/Write)
// 0x24 : Data signal of cfg
//        bit 31~0 - cfg[191:160] (Read/Write)
// 0x28 : reserved
// 0x2c : Data signal of status_common_pattern
//        bit 31~0 - status_common_pattern[31:0] (Read)
// 0x30 : Control signal of status_common_pattern
//        bit 0  - status_common_pattern_ap_vld (Read/COR)
//        others - reserved
// 0x34 : Data signal of status_cfg
//        bit 31~0 - status_cfg[31:0] (Read)
// 0x38 : Data signal of status_cfg
//        bit 31~0 - status_cfg[63:32] (Read)
// 0x3c : Control signal of status_cfg
//        bit 0  - status_cfg_ap_vld (Read/COR)
//        others - reserved
// 0x40 : Data signal of status_read
//        bit 31~0 - status_read[31:0] (Read)
// 0x44 : Data signal of status_read
//        bit 31~0 - status_read[63:32] (Read)
// 0x48 : Data signal of status_read
//        bit 31~0 - status_read[95:64] (Read)
// 0x4c : Data signal of status_read
//        bit 31~0 - status_read[127:96] (Read)
// 0x50 : Data signal of status_read
//        bit 31~0 - status_read[159:128] (Read)
// 0x54 : Data signal of status_read
//        bit 31~0 - status_read[191:160] (Read)
// 0x58 : Data signal of status_read
//        bit 31~0 - status_read[223:192] (Read)
// 0x5c : Data signal of status_read
//        bit 31~0 - status_read[255:224] (Read)
// 0x60 : Data signal of status_read
//        bit 31~0 - status_read[287:256] (Read)
// 0x64 : Data signal of status_read
//        bit 31~0 - status_read[319:288] (Read)
// 0x68 : Data signal of status_read
//        bit 31~0 - status_read[351:320] (Read)
// 0x6c : Data signal of status_read
//        bit 31~0 - status_read[383:352] (Read)
// 0x70 : Data signal of status_read
//        bit 31~0 - status_read[415:384] (Read)
// 0x74 : Control signal of status_read
//        bit 0  - status_read_ap_vld (Read/COR)
//        others - reserved
// 0x78 : Data signal of status_write
//        bit 31~0 - status_write[31:0] (Read)
// 0x7c : Data signal of status_write
//        bit 31~0 - status_write[63:32] (Read)
// 0x80 : Data signal of status_write
//        bit 31~0 - status_write[95:64] (Read)
// 0x84 : Control signal of status_write
//        bit 0  - status_write_ap_vld (Read/COR)
//        others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_AP_CTRL                    0x00
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_GIE                        0x04
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_IER                        0x08
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_ISR                        0x0c
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_CFG_DATA                   0x10
#define XDUNEDATACOMPRESSIONCORE_BUS_A_BITS_CFG_DATA                   192
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_COMMON_PATTERN_DATA 0x2c
#define XDUNEDATACOMPRESSIONCORE_BUS_A_BITS_STATUS_COMMON_PATTERN_DATA 32
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_COMMON_PATTERN_CTRL 0x30
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_CFG_DATA            0x34
#define XDUNEDATACOMPRESSIONCORE_BUS_A_BITS_STATUS_CFG_DATA            64
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_CFG_CTRL            0x3c
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_READ_DATA           0x40
#define XDUNEDATACOMPRESSIONCORE_BUS_A_BITS_STATUS_READ_DATA           416
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_READ_DATA_          0x68
#define XDUNEDATACOMPRESSIONCORE_BUS_A_BITS_STATUS_READ_DATA           416
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_READ_CTRL           0x74
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_WRITE_DATA          0x78
#define XDUNEDATACOMPRESSIONCORE_BUS_A_BITS_STATUS_WRITE_DATA          96
#define XDUNEDATACOMPRESSIONCORE_BUS_A_ADDR_STATUS_WRITE_CTRL          0x84

