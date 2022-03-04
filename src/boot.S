
#include "sysregs.h"

.section ".text.boot"

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)

.globl _start
_start:
    adr x0, main
    mov x1, 0xd8
    str x0, [x1] // set adr of core 0
    mov x1, 0xe0
    str x0, [x1] // set adr of core 1
    mov x1, 0xe8
    str x0, [x1] // set adr of core 2
    mov x1, 0xf0
    str x0, [x1] // set adr of core 3
    sev // wake up cores
main:
    mov x1, #0x33ff
    msr cptr_el2, x1
    msr hstr_el2, xzr
    mov x1, #0x300000
    msr cpacr_el1, x1 // enable fpu

    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0 // disable MMU

    ldr x0, =HCR_VAL
    msr hcr_el2, x0 // disable hypervisor, 64 bit mode for EL1

    ldr x0, =SPSR_VALUE
    msr spsr_el2, x0 // set saved program status register to EL1, because we want to *be* in EL1

    adr x0, EL1Entry
    msr elr_el2, x0 // set exception vector to EL1 entry

    eret // enter EL1
EL1Entry:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    mov x2, #PAGE_SIZE
    mul x1, x0, x2
    add x1, x1, #LOW_MEMORY
    mov sp, x1 // set stack pointer to low memory
    bl kernelMain // call kernel main