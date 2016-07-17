
/*
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430G2xx1 Demo - Flash In-System Programming, Copy SegC to SegD
//
//  Description: This program first erases flash seg C, then it increments all
//  values in seg C, then it erases seg D, then copies seg C to seg D.
//  Assumed MCLK 771kHz - 1428kHz.
//  //* Set Breakpoint on NOP in the Mainloop to avoid Stressing Flash *//
//
//               MSP430G2xx1
//            -----------------
//        /|\|              XIN|-
//         | |                 |
//         --|RST          XOUT|-
//           |                 |
//
//  D. Dang
//  Texas Instruments Inc.
//  October 2010
//  Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************

#include "flash.h"

// Function prototypes


void flash_init(void)
{

  FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
/*

  while(1)                                  // Repeat forever
  {
    write_SegC(value++);                    // Write segment C, increment value
    copy_C2D();                             // Copy segment C to D
    __no_operation();                       // SET BREAKPOINT HERE
  }*/
}
void clean_SegC()
{
	UINT8 *Flash_ptr = (UINT8 *) 0x1040;			  // Initialize Flash pointer
	FCTL1 = FWKEY + ERASE;					  // Set Erase bit
	FCTL3 = FWKEY;							  // Clear Lock bit
	*Flash_ptr = 0; 						  // Dummy write to erase Flash segment

}
void write_SegC (UINT8 * WriteData)
{
  UINT8 *Flash_ptr;                          // Flash pointer
  unsigned int i;

  Flash_ptr = (UINT8 *) 0x1040;              // Initialize Flash pointer
  clean_SegC();

  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  for (i=0; i<64; i++)
  {
    *Flash_ptr++ = *WriteData++;                   // Write value to flash
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
void read_SegC (UINT8* ReadData)
{
    UINT8 *Flash_ptrC;
	Flash_ptrC = (UINT8 *) 0x1040;             // Initialize Flash segment C pointer
	for (UINT8 i=0; i<64; i++)
	{
    	*ReadData++ = *Flash_ptrC++;          // copy value segment C to segment D
  	}
}

