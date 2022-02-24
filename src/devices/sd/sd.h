
#ifndef SD_H
#define SD_H

#include "../base.h"
#include "../../util.h"

#define SD_BASE (PERIPHERAL_BASE + 0x00300000)
#define SD_BLKSIZECNT (SD_BASE + 0x4)
#define SD_ARG1 (SD_BASE + 0x8)
#define SD_CMDTM (SD_BASE + 0xc)
#define SD_RESP0 (SD_BASE + 0x10)
#define SD_RESP1 (SD_BASE + 0x14)
#define SD_RESP2 (SD_BASE + 0x18)
#define SD_RESP3 (SD_BASE + 0x1c)
#define SD_DATA (SD_BASE + 0x20)
#define SD_STATUS (SD_BASE + 0x24)
#define SD_CONTROL0 (SD_BASE + 0x28)
#define SD_CONTROL1 (SD_BASE + 0x2c)
#define SD_INTERRUPT (SD_BASE + 0x30)
#define SD_IRPT_MASK (SD_BASE + 0x34)
#define SD_IRPT_EN (SD_BASE + 0x38)
#define SD_SLOTISR_VER (SD_BASE + 0xfc)

// control 0 flags
#define SD_C0_SPI_MODE_EN 0x00100000
#define SD_C0_HCTL_HS_EN 0x00000004
#define SD_C0_HCTL_DWITDH 0x00000002

// reset sd card
#define SD_C1_SRST_HC (1 << 24)
// internal emmc clock enable(?!)
#define SD_C1_CLK_INTLEN (1 << 0)
// data timeout exp
#define SD_C1_TOUNIT_MAX (0xe << 16)
// enable clk
#define SD_C1_CLK_EN (1 << 2)
// clk stable
#define SD_C1_CLK_STABLE (1 <<1)
// inhibit dat/cmd
#define SD_DAT_INHIBIT (1 << 1)
#define SD_CMD_INHIBIT (1 << 0)

#define SD_SR_APP_CMD 0x00000020

#define SD_HOST_SPEC_NUM 0x00ff0000
#define SD_HOST_SPEC_NUM_SHIFT 16
#define SD_HOST_SPEC_V3 2
#define SD_HOST_SPEC_V2 1
#define SD_HOST_SPEC_V1 0

// commands
#define SD_CMD_NEED_APP        0x80000000
#define SD_CMD_RSPNS_48        0x00020000
#define SD_CMD_ERRORS_MASK     0xfff9c004
#define SD_CMD_RCA_MASK        0xffff0000

#define SD_CMD_GO_IDLE         0x00000000
#define SD_CMD_ALL_SEND_CID    0x02010000
#define SD_CMD_SEND_REL_ADDR   0x03020000
#define SD_CMD_CARD_SELECT     0x07030000
#define SD_CMD_SEND_IF_COND    0x08020000
#define SD_CMD_STOP_TRANS      0x0C030000
#define SD_CMD_READ_SINGLE     0x11220010
#define SD_CMD_READ_MULTI      0x12220032
#define SD_CMD_WRITE_SINGLE    0x18220000
#define SD_CMD_WRITE_MULTI     0x19220022
#define SD_CMD_SET_BLOCKCNT    0x17020000
#define SD_CMD_APP_CMD         0x37000000
#define SD_CMD_SET_BUS_WIDTH   (0x06020000 | SD_CMD_NEED_APP)
#define SD_CMD_SEND_OP_COND    (0x29020000 | SD_CMD_NEED_APP)
#define SD_CMD_SEND_SCR        (0x33220010 | SD_CMD_NEED_APP)

#define SD_INT_DATA_TIMEOUT 0x00100000
#define SD_INT_CMD_TIMEOUT 0x00010000
#define INT_CMD_DONE (1 << 0)
#define SD_INT_ERROR_MASK 0x017E8000

// amcd1 stuff
#define SD_ACMD41_VOLTAGE 0x00ff8000
#define SD_ACMD41_CMD_COMPLETE 0x80000000
#define SD_ACMD41_CMD_CCS 0x40000000
#define SD_ACMD41_ARG_HC 0x51ff8000

// scr flags
#define SD_SCR_SD_BUS_WIDTH_4 0x00000400
#define SD_SCR_SUPP_SET_BLKCNT 0x02000000
#define SD_SCR_SUPP_CCS 0x00000001

// status register flags
#define SD_SR_READ_AVAILABLE 0x00000800
#define SD_SR_WRITE_AVAILABLE  0x00000400
#define SD_SR_DAT_INHIBIT 0x00000002
#define SD_SR_CMD_INHIBIT 0x00000001
#define SD_SR_APP_CMD 0x00000020

// interrupt register flags
#define SD_INT_DATA_TIMEOUT 0x00100000
#define SD_INT_CMD_TIMEOUT 0x00010000
#define SD_INT_READ_RDY 0x00000020
#define SD_INT_WRITE_RDY 0x00000010
#define SD_INT_DATA_DONE 0x00000002
#define SD_INT_CMD_DONE 0x00000001

#define SD_OK 0
#define SD_TIMEOUT -1
#define SD_ERROR -2

int sd_init(void);
int sd_readBlock(uint lba, uchar* buffer, uint num);
int sd_writeBlock(uchar *buffer, uint lba, uint num);

#endif