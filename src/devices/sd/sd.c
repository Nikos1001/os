
#include "../gpio/gpio.h"
#include "../uart/uart.h"
#include "sd.h"
#include "../../lock.h"

ulong sd_scr[2], sd_ocr, sd_rca, sd_err, sd_hv;

struct Lock sd_lock;

int waitForCnt, timeout;
#define WAIT_FOR(cond) \
    waitForCnt = 10000; \
    while(waitForCnt > 0 && !(cond)) { \
        waitForCnt--; \
        delay(10000); \
    } \
    timeout = waitForCnt <= 0;

int sd_status(uint mask) {
    WAIT_FOR(!(get32(SD_STATUS) & mask) || (get32(SD_INTERRUPT) & SD_INT_ERROR_MASK))
    delay(100000);
    return (timeout || (get32(SD_INTERRUPT) & SD_INT_ERROR_MASK)) ? SD_ERROR : SD_OK;
}

int sd_int(uint mask) {
    uint r, m = mask | SD_INT_ERROR_MASK;
    WAIT_FOR(get32(SD_INTERRUPT) & m)
    r = get32(SD_INTERRUPT);
    if(timeout || (r & SD_INT_CMD_TIMEOUT) || (r & SD_INT_DATA_TIMEOUT)) {
        put32(SD_INTERRUPT, r);
        return SD_TIMEOUT;
    } else if(r & SD_INT_ERROR_MASK) {
        put32(SD_INTERRUPT, r);
        return SD_ERROR;
    }
    put32(SD_INTERRUPT, mask);
    return 0;
}

int sd_cmd(uint code, uint arg) {
    int r = 0;
    sd_err = SD_OK;
    if(code & SD_CMD_NEED_APP) {
        r = sd_cmd(SD_CMD_APP_CMD | (sd_rca ? SD_CMD_RSPNS_48 : 0), sd_rca);
        if(sd_rca && !r) {
            uart_puts("sd - failed to send sd app command.\n");
            sd_err = SD_ERROR;
            return 0;
        }
        code &= ~SD_CMD_NEED_APP;
    }
    if(sd_status(SD_SR_CMD_INHIBIT)) {
        uart_puts("sd - emmc busy\n");
        sd_err = SD_TIMEOUT;
        return 0;
    }
    put32(SD_INTERRUPT, get32(SD_INTERRUPT));
    put32(SD_ARG1, arg);
    put32(SD_CMDTM, code);
    if(code == SD_CMD_SEND_OP_COND)
        delay(100000000);
    else if(code == SD_CMD_SEND_IF_COND || code == SD_CMD_APP_CMD)
        delay(10000000);

    r = sd_int(SD_INT_CMD_DONE);
    if(r) {
        uart_puts("sd - failed to send emmc command\n");
        sd_err = r;
        return 0;
    }
    r = get32(SD_RESP0);
    if(code == SD_CMD_GO_IDLE || code == SD_CMD_APP_CMD)
        return 0;
    else if(code == (SD_CMD_APP_CMD | SD_CMD_RSPNS_48))
        return r & SD_SR_APP_CMD;
    else if(code == SD_CMD_SEND_OP_COND)
        return r;
    else if(code == SD_CMD_SEND_IF_COND)
        return r == arg ? SD_OK : SD_ERROR; 
    else if(code == SD_CMD_ALL_SEND_CID) {
        r |= get32(SD_RESP3);
        r |= get32(SD_RESP2);
        r |= get32(SD_RESP1);
        return r;
    } else if(code == SD_CMD_SEND_REL_ADDR) {
        sd_err = (((r & 0x1fff)) | ((r & 0x2000) << 6) | ((r & 0x4000) << 8) | ((r & 0x8000) << 8)) & SD_CMD_ERRORS_MASK;
        return r & SD_CMD_RCA_MASK;
    }
    return r & SD_CMD_ERRORS_MASK;
}

int sd_readSector(int core, uint lba, uchar* buffer, uint num) {
    waitLock(&sd_lock, core);

    if(num < 1)
        num = 1;
    if(sd_status(SD_SR_DAT_INHIBIT)) {
        sd_err = SD_TIMEOUT;
        return 0;
    }
    uint* buf = (uint*)buffer;
    if(sd_scr[0] & SD_SCR_SUPP_CCS) {
        if(num > 1 && (sd_scr[0] & SD_SCR_SUPP_SET_BLKCNT)) {
            sd_cmd(SD_CMD_SET_BLOCKCNT, num);
            if(sd_err)
                return 0;
        }

        // num blocks, 512 bytes each
        put32(SD_BLKSIZECNT, (num << 16) | 512);
        sd_cmd(num == 1 ? SD_CMD_READ_SINGLE : SD_CMD_READ_MULTI, lba);
        if(sd_err)
            return 0;
    } else {
        put32(SD_BLKSIZECNT, (1 << 16) | 512);
    }
    int c = 0;
    while(c < num) {
        if(!(sd_scr[0] & SD_SCR_SUPP_CCS)) {
            sd_cmd(SD_CMD_READ_SINGLE, (lba + c) * 512);
            if(sd_err)
                return 0;
        }
        long notReady = sd_int(SD_INT_READ_RDY);
        if(notReady) {
            uart_puts("sd - timed out reading\n");
            sd_err = notReady;
            return 0;
        }
        for(int d = 0; d < 128; d++)
            buf[d] = get32(SD_DATA);
        c++;
        buf += 128;
    }
    if(num > 1 && !(sd_scr[0] & SD_SCR_SUPP_SET_BLKCNT) && (sd_scr[0] & SD_SCR_SUPP_CCS))
        sd_cmd(SD_CMD_STOP_TRANS, 0);
    
    exitLock(&sd_lock, core);
    return sd_err != SD_OK || c != num ? 0 : num * 512;
}

int sd_writeSector(int core, uchar *buffer, uint lba, uint num) {
    waitLock(&sd_lock, core);

    int r;
    int c = 0;
    int d;
    if(num < 1)
        num=1;
    if(sd_status(SD_SR_DAT_INHIBIT | SD_SR_WRITE_AVAILABLE)) {
        sd_err = SD_TIMEOUT;
        return 0;
    }
    uint* buf = (uint*)buffer;
    if(sd_scr[0] & SD_SCR_SUPP_CCS) {
        if(num > 1 && (sd_scr[0] & SD_SCR_SUPP_SET_BLKCNT)) {
            sd_cmd(SD_CMD_SET_BLOCKCNT, num);
            if(sd_err)
                return 0;
        }
        put32(SD_BLKSIZECNT, (num << 16) | 512);
        sd_cmd(num == 1 ? SD_CMD_WRITE_SINGLE : SD_CMD_WRITE_MULTI, lba);
        if(sd_err) return 0;
    } else {
        put32(SD_BLKSIZECNT, (1 << 16) | 512);
    }
    while(c < num) {
        if(!(sd_scr[0] & SD_SCR_SUPP_CCS)) {
            sd_cmd(SD_CMD_WRITE_SINGLE, (lba + c) * 512);
            if(sd_err)
                return 0;
        }
        r = sd_int(SD_INT_WRITE_RDY);
        if(r) {
            uart_puts("sd - write timeout.\n");
            sd_err = r;
            return 0;
        }
        for(d = 0; d < 128; d++) 
            put32(SD_DATA, buf[d]);
        c++;
        buf += 128;
    }
    r = sd_int(SD_INT_DATA_DONE);
    if(r) {
        uart_puts("sd - data done timeout.\n");
        sd_err = r;
        return 0;
    }
    if(num > 1 && !(sd_scr[0] & SD_SCR_SUPP_SET_BLKCNT) && (sd_scr[0] & SD_SCR_SUPP_CCS))
        sd_cmd(SD_CMD_STOP_TRANS, 0);

    exitLock(&sd_lock, core);
    return sd_err != SD_OK || c != num ? 0 : num * 512;
}

int sd_clk(uint f) {
    uint d;
    uint c = 41666666 / f;
    uint x;
    uint s = 32;
    uint h = 0;
    
    WAIT_FOR((get32(SD_STATUS) & (SD_SR_CMD_INHIBIT | SD_SR_DAT_INHIBIT)) == 0)
    if(timeout) {
        uart_puts("sd - timeout waiting for inhibit flag.\n");
        return SD_ERROR;
    }

    put32(SD_CONTROL1, get32(SD_CONTROL1) & ~SD_C1_CLK_EN);
    delay(1000000);
    x = c - 1;
    if(!x) {
        s=0;
    } else {
        if(!(x & 0xffff0000u)) {
            x <<= 16;
            s -= 16;
        }
        if(!(x & 0xff000000u)) {
            x <<= 8;
            s -= 8;
        }
        if(!(x & 0xf0000000u)) {
            x <<= 4;
            s -= 4;
        }
        if(!(x & 0xc0000000u)) {
            x <<= 2;
            s -= 2;
        }
        if(!(x & 0x80000000u)) {
            x <<= 1;
            s -= 1;
        }
        if(s > 0)
            s--;
        if(s > 7)
            s = 7;
    }
    if(sd_hv > SD_HOST_SPEC_V2)
        d = c;
    else
        d = (1 << s);
    if(d <= 2) {
        d=2;
        s=0;
    }

    if(sd_hv > SD_HOST_SPEC_V2)
        h = (d & 0x300) >> 2;
    d = (((d & 0x0ff) << 8) | h);
    put32(SD_CONTROL1, (get32(SD_CONTROL1) & 0xffff003f) | d);
    delay(1000000);
    put32(SD_CONTROL1, get32(SD_CONTROL1) | SD_C1_CLK_EN);
    delay(1000000);
    WAIT_FOR(get32(SD_CONTROL1) & SD_C1_CLK_STABLE)
    delay(1000000);
    if(timeout) {
        uart_puts("sd - failed to get stable clock.\n");
        return SD_ERROR;
    }
    return SD_OK;
}

int sd_init(void) {
    initLock(&sd_lock);

    long ccs = 0;

    gpio_selectFunction(47, 0);
    gpio_pushFunctions(2, (1 << 47));
    gpio_setHighDetectEnable(47, 1); // setup cd pins

    gpio_selectFunction(48, 7);
    gpio_selectFunction(49, 7);
    gpio_pushFunctions(2, (1 << 48) | (1 << 49)); // setup clk & cmd pins

    gpio_selectFunction(50, 7);
    gpio_selectFunction(51, 7);
    gpio_selectFunction(52, 7);
    gpio_selectFunction(53, 7);
    gpio_pushFunctions(2, (1 << 50) | (1 << 51) | (1 << 52) | (1 << 53)); // setup data pins

    sd_hv = (get32(SD_SLOTISR_VER) & SD_HOST_SPEC_NUM) >> SD_HOST_SPEC_NUM_SHIFT;

    // reset sd card
    put32(SD_CONTROL0, 0);
    put32(SD_CONTROL1, get32(SD_CONTROL1) | SD_C1_SRST_HC);
    WAIT_FOR((get32(SD_CONTROL1) & SD_C1_SRST_HC) == 0)
    if(timeout) {
        uart_puts("sd - failed to reset.\n");
        return SD_ERROR;
    }

    put32(SD_CONTROL1, get32(SD_CONTROL1) | SD_C1_CLK_INTLEN | SD_C1_TOUNIT_MAX);
    delay(1000000);

    // setup sd clock
    long clockResp = sd_clk(400000);
    if(clockResp)
        return clockResp;
    
    // enable all interrupts
    put32(SD_IRPT_EN, 0xffffffff);
    put32(SD_IRPT_MASK, 0xffffffff);

    sd_scr[0] = sd_scr[1] = sd_rca = sd_err = 0;

    // make sd idle
    sd_cmd(SD_CMD_GO_IDLE, 0);
    if(sd_err)
        return sd_err;

    sd_cmd(SD_CMD_SEND_IF_COND, 0x000001AA);
    if(sd_err)
        return sd_err;
    
    int completeCnt = 6;
    long completeResp = 0;
    while(!(completeResp & SD_ACMD41_CMD_COMPLETE) && completeCnt >= 0) {
        completeCnt--;
        delay(400);
        completeResp = sd_cmd(SD_CMD_SEND_OP_COND, SD_ACMD41_ARG_HC);
        if(sd_err != SD_TIMEOUT && sd_err != SD_OK) {
            uart_puts("sd - emmc acmd41 returned error.\n");
            return sd_err;
        }
    }
    if(!(completeResp & SD_ACMD41_CMD_COMPLETE) || completeCnt <= 0)
        return SD_TIMEOUT;
    if(!(completeResp & SD_ACMD41_VOLTAGE))
        return SD_ERROR;
    if(completeResp & SD_ACMD41_CMD_CCS)
        ccs = SD_SCR_SUPP_CCS;

    sd_cmd(SD_CMD_ALL_SEND_CID, 0);

    sd_rca = sd_cmd(SD_CMD_SEND_REL_ADDR, 0);
    if(sd_err)
        return sd_err;

    // set new clock freq
    clockResp = sd_clk(25000000);
    if(clockResp)
        return clockResp;
    
    sd_cmd(SD_CMD_CARD_SELECT, sd_rca);
    if(sd_err)
        return sd_err;

    if(sd_status(SD_SR_DAT_INHIBIT))
        return SD_TIMEOUT;
    
    // get 1 block, 8 bytes
    put32(SD_BLKSIZECNT, (1 << 16) | 8);
    sd_cmd(SD_CMD_SEND_SCR, 0);
    if(sd_err)
        return sd_err;
    if(sd_int(SD_INT_READ_RDY))
        return SD_TIMEOUT;

    long readIdx = 0;
    int readCnt = 100000;
    while(readIdx < 2 && readCnt > 0) {
        readCnt--;
        if(get32(SD_STATUS) & SD_SR_READ_AVAILABLE) {
            sd_scr[readIdx] = get32(SD_DATA);
            readIdx++;
        } else {
            delay(100000);
        }
    }
    if(readIdx != 2)
        return SD_TIMEOUT;
    if(sd_scr[0] & SD_SCR_SD_BUS_WIDTH_4) {
        sd_cmd(SD_CMD_SET_BUS_WIDTH, sd_rca | 2);
        if(sd_err)
            return sd_err;
        put32(SD_CONTROL0, get32(SD_CONTROL0) | SD_C0_HCTL_DWITDH);
    }

    // add software flag
    sd_scr[0] &= ~SD_SCR_SUPP_CCS;
    sd_scr[0] |= ccs;
    return SD_OK;
}