/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef DEBUG_CM_H
#define DEBUG_CM_H

#include "target_struct.h"


// Debug Port Register Addresses
#define DP_IDCODE      0x00        // IDCODE Register (Read only)
#define DP_ABORT       0x00        // Abort Register (Write only)
#define DP_CTRL_STAT   0x04        // Control & Status
#define DP_WCR         0x04        // Wire Control Register (SW Only)
#define DP_DLCR        0x04        // Data Link Control Register (Renamed WCR)
#define DP_SELECT      0x08        // Select Register (JTAG R/W & SW W)
#define DP_RESEND      0x08        // Resend (SW Read Only)
#define DP_RDBUFF      0x0C        // Read Buffer (Read Only)

// Abort Register definitions
#define DAPABORT       0x00000001  // DAP Abort
#define STKCMPCLR      0x00000002  // Clear STICKYCMP Flag (SW Only)
#define STKERRCLR      0x00000004  // Clear STICKYERR Flag (SW Only)
#define WDERRCLR       0x00000008  // Clear WDATAERR Flag (SW Only)
#define ORUNERRCLR     0x00000010  // Clear STICKYORUN Flag (SW Only)

// Debug Control and Status definitions
#define ORUNDETECT     0x00000001  // Overrun Detect
#define STICKYORUN     0x00000002  // Sticky Overrun
#define TRNMODE        0x0000000C  // Transfer Mode Mask
#define TRNNORMAL      0x00000000  // Transfer Mode: Normal
#define TRNVERIFY      0x00000004  // Transfer Mode: Pushed Verify
#define TRNCOMPARE     0x00000008  // Transfer Mode: Pushed Compare
#define STICKYCMP      0x00000010  // Sticky Compare
#define STICKYERR      0x00000020  // Sticky Error
#define READOK         0x00000040  // Read OK (SW Only)
#define WDATAERR       0x00000080  // Write Data Error (SW Only)
#define MASKLANE       0x00000F00  // Mask Lane Mask
#define MASKLANE0      0x00000100  // Mask Lane 0
#define MASKLANE1      0x00000200  // Mask Lane 1
#define MASKLANE2      0x00000400  // Mask Lane 2
#define MASKLANE3      0x00000800  // Mask Lane 3
#define TRNCNT         0x001FF000  // Transaction Counter Mask
#define CDBGRSTREQ     0x04000000  // Debug Reset Request
#define CDBGRSTACK     0x08000000  // Debug Reset Acknowledge
#define CDBGPWRUPREQ   0x10000000  // Debug Power-up Request
#define CDBGPWRUPACK   0x20000000  // Debug Power-up Acknowledge
#define CSYSPWRUPREQ   0x40000000  // System Power-up Request
#define CSYSPWRUPACK   0x80000000  // System Power-up Acknowledge

// Debug Select Register definitions
#define CTRLSEL        0x00000001  // CTRLSEL (SW Only)
#define APBANKSEL      0x000000F0  // APBANKSEL Mask
#define APSEL          0xFF000000  // APSEL Mask

// Access Port Register Addresses
#define AP_CSW         0x00        // Control and Status Word
#define AP_TAR         0x04        // Transfer Address
#define AP_DRW         0x0C        // Data Read/Write
#define AP_BD0         0x10        // Banked Data 0
#define AP_BD1         0x14        // Banked Data 1
#define AP_BD2         0x18        // Banked Data 2
#define AP_BD3         0x1C        // Banked Data 3
#define AP_ROM         0xF8        // Debug ROM Address
#define AP_IDR         0xFC        // Identification Register

// AP Control and Status Word definitions
#define CSW_SIZE       0x00000007  // Access Size: Selection Mask
#define CSW_SIZE8      0x00000000  // Access Size: 8-bit
#define CSW_SIZE16     0x00000001  // Access Size: 16-bit
#define CSW_SIZE32     0x00000002  // Access Size: 32-bit
#define CSW_ADDRINC    0x00000030  // Auto Address Increment Mask
#define CSW_NADDRINC   0x00000000  // No Address Increment
#define CSW_SADDRINC   0x00000010  // Single Address Increment
#define CSW_PADDRINC   0x00000020  // Packed Address Increment
#define CSW_DBGSTAT    0x00000040  // Debug Status
#define CSW_TINPROG    0x00000080  // Transfer in progress
#define CSW_HPROT      0x02000000  // User/Privilege Control
#define CSW_MSTRTYPE   0x20000000  // Master Type Mask
#define CSW_MSTRCORE   0x00000000  // Master Type: Core
#define CSW_MSTRDBG    0x20000000  // Master Type: Debug
#define CSW_RESERVED   0x01000000  // Reserved Value

// Core Debug Register Address Offsets
#define DBG_OFS        0x0DF0      // Debug Register Offset inside NVIC
#define DBG_HCSR_OFS   0x00        // Debug Halting Control & Status Register
#define DBG_CRSR_OFS   0x04        // Debug Core Register Selector Register
#define DBG_CRDR_OFS   0x08        // Debug Core Register Data Register
#define DBG_EMCR_OFS   0x0C        // Debug Exception & Monitor Control Register

// Core Debug Register Addresses
#define DBG_HCSR       (DBG_Addr + DBG_HCSR_OFS)
#define DBG_CRSR       (DBG_Addr + DBG_CRSR_OFS)
#define DBG_CRDR       (DBG_Addr + DBG_CRDR_OFS)
#define DBG_EMCR       (DBG_Addr + DBG_EMCR_OFS)

// Debug Halting Control and Status Register definitions
#define C_DEBUGEN      0x00000001  // Debug Enable
#define C_HALT         0x00000002  // Halt
#define C_STEP         0x00000004  // Step
#define C_MASKINTS     0x00000008  // Mask Interrupts
#define C_SNAPSTALL    0x00000020  // Snap Stall
#define S_REGRDY       0x00010000  // Register R/W Ready Flag
#define S_HALT         0x00020000  // Halt Flag
#define S_SLEEP        0x00040000  // Sleep Flag
#define S_LOCKUP       0x00080000  // Lockup Flag
#define S_RETIRE_ST    0x01000000  // Sticky Retire Flag
#define S_RESET_ST     0x02000000  // Sticky Reset Flag
#define DBGKEY         0xA05F0000  // Debug Key

// Debug Exception and Monitor Control Register definitions
#define VC_CORERESET   0x00000001  // Reset Vector Catch
#define VC_MMERR       0x00000010  // Debug Trap on MMU Fault
#define VC_NOCPERR     0x00000020  // Debug Trap on No Coprocessor Fault
#define VC_CHKERR      0x00000040  // Debug Trap on Checking Error Fault
#define VC_STATERR     0x00000080  // Debug Trap on State Error Fault
#define VC_BUSERR      0x00000100  // Debug Trap on Bus Error Fault
#define VC_INTERR      0x00000200  // Debug Trap on Interrupt Error Fault
#define VC_HARDERR     0x00000400  // Debug Trap on Hard Fault
#define MON_EN         0x00010000  // Monitor Enable
#define MON_PEND       0x00020000  // Monitor Pend
#define MON_STEP       0x00040000  // Monitor Step
#define MON_REQ        0x00080000  // Monitor Request
#define TRCENA         0x01000000  // Trace Enable (DWT, ITM, ETM, TPIU)

// NVIC: Interrupt Controller Type Register
#define NVIC_ICT       (NVIC_Addr + 0x0004)
#define INTLINESNUM    0x0000001F  // Interrupt Line Numbers

// NVIC: CPUID Base Register
#define NVIC_CPUID     (NVIC_Addr + 0x0D00)
#define CPUID_PARTNO   0x0000FFF0  // Part Number Mask
#define CPUID_REVISION 0x0000000F  // Revision Mask
#define CPUID_VARIANT  0x00F00000  // Variant Mask

// NVIC: Application Interrupt/Reset Control Register
#define NVIC_AIRCR     (NVIC_Addr + 0x0D0C)
#define VECTRESET      0x00000001  // Reset Cortex-M (except Debug)
#define VECTCLRACTIVE  0x00000002  // Clear Active Vector Bit
#define SYSRESETREQ    0x00000004  // Reset System (except Debug)
#define VECTKEY        0x05FA0000  // Write Key

// NVIC: Debug Fault Status Register
#define NVIC_DFSR      (NVIC_Addr + 0x0D30)
#define HALTED         0x00000001  // Halt Flag
#define BKPT           0x00000002  // BKPT Flag
#define DWTTRAP        0x00000004  // DWT Match
#define VCATCH         0x00000008  // Vector Catch Flag
#define EXTERNAL       0x00000010  // External Debug Request

//Yangliang Add

#define Reg_CoreDebug_DHCSR_address      0xE000EDF0
#define Reg_CoreDebug_DCRSR_address      0xE000EDF4
#define Reg_CoreDebug_DCRDR_address      0xE000EDF8
#define Reg_CoreDebug_DEMCR_address      0xE000EDFC

// CPU related commands 
#define CMD_READ_MEM			0x30	// parameters: 8-bit Memory Type, 32-bit address, 8-bit Size (8/16/32), 32-bit length (number to read)
#define CMD_WRITE_MEM			0x31 	// parameters: 8-bit Memory Type, 32-bit address, 8-bit Size (8/16/32), 32-bit length (number to write), data ...
#define CMD_READ_NEXT			0x32	// no parameters, reads the next block of data from the CMD_READ_MEM command
#define CMD_WRITE_NEXT			0x33 	// no parameters, writes the next block of data from the CMD_WRITE_MEM command
#define CMD_FILL				0x34 	// parameters: 8-bit Memory Type, 32-bit address, 8-bit Size (8/16/32), 32-bit length (number to write), data ...

#define CMD_CORE_WRITE			0xA0
#define CMD_CORE_RESET			0xA1
#define CMD_CORE_READ			0xA2

// System related commands
#define CMD_GET_STATUS			0x10	// returns a block of status information for the BDM and it's current state
#define CMD_BDM_INIT			0x11	// initialize BDM

#define CMD_SET_TARGET			0x1A	// 8bit parameter; 0 = ColdFire(default), 1 = JTAG
#define CMD_RESET_TARGET		0x1B	// 8bit parameter; 0 = reset to BDM mode, 1 = reset to NORMAL mode 

#define CMD_HALT				0x20	// stop the CPU and bring it into BDM mode 
#define CMD_GO					0x21	// start code execution from current PC address 
#define CMD_STEP				0x22	// perform single step 
#define CMD_RESYNCHRONIZE		0x23	// resynchronize communication with the target (in case of noise, etc.) 
#define CMD_ASSERT_TA			0x24	// parameter: 8-bit number of 10us ticks - duration of the TA assertion 
#define CMD_SET_SPEED			0x25	// set target clock speed
#define CMD_GET_SPEED			0x26	// get target clock speed
#define CMD_SPECIAL_FEATURE             0x27    // Architecture Specific Command - See appropriate firmware file.



#define HWBCR		(0x20000000)      
#define HWCSR		(0x20000004)      
#define HWCMR		(0x20000008)     
#define HWBFR	    (0x2000000C)  	  


#define HWESR		(0x20000010)      
#define HWERR		(0x20000014)   
#define HWUST       (0x20000018)
#define HWSID		(0x2000001C)  
#define HWFCG1		(0x20000020) 	
#define HWFCG2		(0x20000024) 
#define HWSEC       (0x20000028)
#define HWCHK		(0x2000002C) 
#define HWBUFADDR   (0x20000030) 



#define HWBUF_DEFAULT       (0x1FFFFF00)	   


#define CMD_RUN		0xA5AA6512
#define CMD_DONW	0xAA557108

#define CMD_IDLE    0xA4A53151
#define CMD_ERASE   0xA5A51234
#define CMD_PROGRA  0xAA557854

#define CPU_RUN     0x1267A5A7
#define CPU_STOP    0x73211983
#define CPU_WAIT    0x2231AA78

#define ERR_NONE    0x00
#define ERR_FLASH   0x01
#define ERR_CMD     0x02
#define ERR_ADDR    0x03

#define FLASH_PC    0x2000006D
#define FLASH_SP    0x20000FF8

#define FILE_SIZE   0x1F800
#define KINETIS_FLASHPRG	0x1FC00



#define RETRY_TIME  3
#define READ_TIME   32768

#define LOCK_MCU    0x12344321

#define READBACK_ADDR	0x20001C00



typedef enum osbdm_error_tag {
    osbdm_error_ok,
    osbdm_error_fail,
    osbdm_error_invalid_parameter,
    osbdm_error_internal_failure,
    osbdm_error_undefined
} osbdm_error;


uint8_t t_go(void);
uint8_t t_halt(void);
void command_exec(void);

//add end

#endif
