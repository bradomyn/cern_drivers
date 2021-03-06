/*********************************************************************/
/* The Tg8 hardware layout. Declarations are prived for the firmware */
/* J.Lewis 7th Oct 94                                                */
/* V.Kovaltsov Oct 96                                                */
/* Vladimir Kovaltsov for ROCS Project, February, 1997		     */
/*********************************************************************/

#ifndef Tg8HARDWARE
#define Tg8HARDWARE

#include <mc68332.h>

#define Tg8DISABLE_INTERRUPTS 0x2700    /* Status Register (SR) code */
#define Tg8ENABLE_INTERRUPTS  0x2400    /* Enable int. levels 5-7    */

#define Tg8ENABLE_WATCHDOG    0xa0      /* Enable watchdog, timeout=490 mks */
#define Tg8ENABLE_BUSMONITOR  0x04      /* Enable BUS Monitor, timeout=64 CLK */

/*********************************************************************/
/* Base addresses and interrupt vectors                              */
/*********************************************************************/

#define Tg8RAM_BASE 0x00000000 /* 64 Kb; 0-0x4000 ? is used by DEBUGGER  */
			       /* 0x4000 - downloaded program address  */
			       /* 0x5000:0x10000 - program's data area  */
#define Tg8DPM_BASE 0x00010000 /* 2Kb size */
#define Tg8CAM_BASE 0x00011000 /* 256 * 3 words size */
#define Tg8XLX_BASE 0x00012000 /* 2 Kb size */

#define Tg8DSC_VECTOR McpVECTOR_AUTO_4 /* Not used? */
#define Tg8DPM_VECTOR McpVECTOR_AUTO_5 /* Not used? */
#define Tg8XLX_VECTOR McpVECTOR_AUTO_6 /* Incoming frame interrupt */
#define Tg8ABT_VECTOR McpVECTOR_AUTO_7 /* Abort button interrupt */

#define Tg8VME_INTERRUPT (Tg8DPM_BASE+0x7FC) /* Tg8 --> DSC */
#define Tg8DSC_INTERRUPT (Tg8DPM_BASE+0x7FE) /* DSC --> Tg8 (Not used?) */

#define Tg8TPU_BASE_VECTOR 64
#define Tg8TPU_INT_LEVEL   7

/*********************************************************************/
/* The layout of the DPRAM is application dependent and is described */
/* by an application include file else where.                        */
/*********************************************************************/

/* Tg8Dpm is described else where, its application specific. */
/* See the file 'tg8.h' for details.			     */

/*********************************************************************/
/* Main Tg8 RAM definition                                           */
/*********************************************************************/

#define Tg8RAM_SIZE 0x800 /* 2 Kb */
typedef unsigned char Tg8Ram[Tg8RAM_SIZE];

/*********************************************************************/
/* Layout the XILINX register space                                  */
/*********************************************************************/

#define Tg8COUNTERS 8 /* Number of counters (e.g. output channels) */

#if CONVERTOR
#define Tg8Xlx_spare Tg8XLX_BASE
#else
#define Tg8Xlx_spare 0
#endif

typedef struct {
   char  XlxSpare_Bytes[Tg8Xlx_spare]; /* Need for asm() usage */
   short XlxDelay [Tg8COUNTERS];       /* Xilinx counter value (e.g. delay) */
   short XlxConfig[Tg8COUNTERS];       /* Xilinx counter configuration */
   short XWsscRframe1;         /* Xilinx Write SSC or Read frame word 1 */
   short XRframe2;             /* Xilinx Read frame word 2 */
   short WClrRerr;             /* Xilinx Write clear receiver error, Read it */

 } Tg8Xlx;

/***********************************/
/* Fields of counter configuration */
/***********************************/

typedef enum {
  ConfOUTPUT      = 1,         /* Produce an output pulse */
  ConfINTERRUPT   = 4,         /* Produce a VME interrupt */
  ConfCABLE_CLOCK = 8,         /* Use cable clock (PS version only) */
  ConfEXT_CLOCK_1 = 0x10,      /* Use the Ext 1 or Internal Clock */
  ConfEXT_CLOCK_2 = 0x18,      /* Use the External clock #2 */
  ConfEXT_START   = 0x20,      /* Use the external start pulse */
  ConfNORMAL      = 0x40,      /* Normal counting */
  ConfCHAINE      = 0x80,      /* Counting in the chain, waits for the previuos cnt */
  ConfBURST       = 0xC0       /* Burst mode, not used */

} Tg8CounterConfig;

/***********************************/
/* XILINX Receiver hardware status */
/***********************************/

typedef enum {
   XrDATA_OVERFLOW =  0x01,
   XrPARITY_ERROR  =  0x02,
   XrEND_SEQUENCE  =  0x04,
   XrMID_BIT       =  0x08,
   XrSTART_SEQUENCE=  0x10,
   XrDISABLED      =  0x20,
   XrMS_MISSING    =  0x40,
   XrMS_WATCH_DOG  =  0x80

 } Tg8XrHardwareStatus;

/*******************************/
/* Tg8's Hardware status bits  */
/*******************************/

typedef enum {
  Tg8HS_COUNTER_1_INTERRUPT=    0x0001, /* Interrupt is from cnt 1 */
  Tg8HS_COUNTER_2_INTERRUPT=    0x0002,
  Tg8HS_COUNTER_3_INTERRUPT=    0x0004,
  Tg8HS_COUNTER_4_INTERRUPT=    0x0008,
  Tg8HS_COUNTER_5_INTERRUPT=    0x0010,
  Tg8HS_COUNTER_6_INTERRUPT=    0x0020,
  Tg8HS_COUNTER_7_INTERRUPT=    0x0040,
  Tg8HS_COUNTER_8_INTERRUPT=    0x0080, /* Interrupt is from cnt 8 */
  Tg8HS_DPRAM_INTERRUPT    =    0x0100, /* Interrupt is from dpram */
  Tg8HS_RECEIVER_ENABLED =      0x0200, /* Enable frame interrupts for MPC */
  Tg8HS_PENDING_DOWNLOAD =      0x0400, /* Pending state */
  Tg8HS_XILINX_XR_LOADED =      0x0800, /* XILINX Receiver loaded */
  Tg8HS_XILINX_X1_LOADED =      0x1000, /* X1 part loaded */
  Tg8HS_XILINX_X2_LOADED =      0x2000, /* X2 part loaded */
  Tg8HS_EXTERNAL_CLOCK_JUMPER = 0x4000, /* External/Internal clock state */
  Tg8HS_SELF_TEST_ERROR  =      0x8000  /* Selftest result */

} Tg8HardwareStatus;

#define Tg8INTERRUPT_MASK       0x1FF	/* DPRAM int.+ Counters int. */
#define Tg8HW_OK_STATUS (Tg8HS_XILINX_XR_LOADED|Tg8HS_XILINX_X1_LOADED| Tg8HS_XILINX_X2_LOADED)    /* XR X1 X2 Loaded OK */

/********************************/
/* TPU Clear Interrupt bitmasks */
/********************************/

typedef enum {
   ClrINT_CHANNEL_0 = 0xfffe, /* TPU 0 is ms clock */
   ClrINT_CHANNEL_1 = 0xfffd, /* TPU 1 is counter #1 interrupt */
   ClrINT_CHANNEL_2 = 0xfffb,
   ClrINT_CHANNEL_3 = 0xfff7,
   ClrINT_CHANNEL_4 = 0xffef,
   ClrINT_CHANNEL_5 = 0xffdf,
   ClrINT_CHANNEL_6 = 0xffbf,
   ClrINT_CHANNEL_7 = 0xff7f,
   ClrINT_CHANNEL_8 = 0xfeff  /* TPU 8 is counter #8 interrupt */
 } Tg8TpuClearInterrupt;

/*********************************************************************/
/* CAM commands. CAM consists from 256 of 48 bits words.             */
/*********************************************************************/

#define Tg8CAM_SIZE 256 /* in 48-bit words */
#define Tg8ACTIONS_NUMBER Tg8CAM_SIZE /* MAX. NUMBER OF ACTIONS */

typedef enum {
    CamDATA_MODE	= 0x0002, /* Set up the data mode (mask for PortF1) */
    CamCOMMAND_MODE	= 0xFFFD, /* Set up the command mode (andw PortF1)  */
    CamWRITE		= 0xE000, /* Enable the CAM row writting */
    CamCLEAR_SKIP_BITS	= 0x9800, /* Clear all skip bits      */
    CamSET_SKIP_BIT	= 0x9400, /* Command to set skip bit  */
    CamSTATUS_MULTIPLE	= 14,     /* Are there mupltiple keys (bit #14) */
    CamMATCH		= 0x8000, /* Match bit of CAM status  */
    CamMUL		= 0x4000, /* Multiple matches bit     */
    CamBYTE_MASK        = 0x00FF  /* Mask on byte value (entry index) */

  } Tg8CamControl;

/*********************************************************************/
/* Define PORT F bits for controlling the self test OK status        */
/*********************************************************************/

typedef enum {
    OkLedON    = 4, /* Self Test Ok ON */
    OkLedBLINK = 8  /* Trigger monostable for OK light */

  } Tg8OkLed;

/**********************************************************************/
/* Define the chip selects and options.                               */
/**********************************************************************/

/* CSBOOT is defined by the hardware and selects the EPROM on reboot. */

/* CS0 writes the left byte to the RAM, its set up in the bootstrap.  */
/* CS1 writes the right byte to the RAM, its set up in the bootstrap. */
/* CS2 reads both bytes from the RAM, its set up in the bootstrap.    */

/* CS3 Selects the Xilinx memory map for X1 X2 and XR.                */

/* CS4 Is the Abort button interrupt autovector.                      */
/* CS5 Is the XILINX frame interrupt autovector.                      */

/* CS6 is Write the CAM with an option control bit.                   */
/* CS7 is Read the CAM                                                */

/* CS8 selects the DPRAM                                              */

/* CSBOOT is defined by the hardware and selects the EPROM on reboot. */
/* It must be burnt into ROM.					      */
/*      Tg8CSBOOT 0x60468b0 */
#define Tg8CSBOOT 0x6000000 | McpCS_BA_BLKSZ_64K    | McpCS_OP_BYTE_BOTH | McpCS_OP_RW_RO      | McpCS_OP_DSACK_WAIT_2 | McpCS_OP_SPACE_BOTH

/*-------------------------------------------------------------------*/
/* CS0 writes the left byte to the RAM, its set up in the bootstrap. */
/*      Tg8CS0 0x3503e */
#define Tg8CS0 McpCS_BA_BLKSZ_16K | McpCS_OP_BYTE_UPPER | McpCS_OP_RW_WO     | McpCS_OP_SPACE_BOTH | McpCS_OP_IPL_7

/* CS1 writes the right byte to the RAM, its set up in the bootstrap. */
/*      Tg8CS1 0x3303e */
#define Tg8CS1 McpCS_BA_BLKSZ_16K | McpCS_OP_BYTE_LOWER | McpCS_OP_RW_WO     | McpCS_OP_SPACE_BOTH | McpCS_OP_IPL_7

/* CS2 reads both bytes from the RAM, its set up in the bootstrap. */
/*      Tg8CS2 0x3683e */
#define Tg8CS2 McpCS_BA_BLKSZ_16K | McpCS_OP_BYTE_BOTH  | McpCS_OP_RW_RO     | McpCS_OP_SPACE_BOTH | McpCS_OP_IPL_7
/*-------------------------------------------------------------------*/

/* CS3 Selects the Xilinx memory map for X1 X2 and XR. */
/*      Tg8CS3 0x01207c30 */
#define Tg8CS3 (Tg8XLX_BASE<<McpCS_BA_ADDRESS_SHIFT)  | McpCS_BA_BLKSZ_2K | McpCS_OP_BYTE_BOTH  | McpCS_OP_RW_RW   | McpCS_OP_STRB_DATA| McpCS_OP_SPACE_BOTH | McpCS_OP_AVEC_EXTERNAL

/*-------------------------------------------------------------------*/
/* CS4 Is the Abort button interrupt autovector. */
/*      Tg8CS4 0xfff8680f */
#define Tg8CS4 McpCS_BA_ADDRESS_MASK | McpCS_BA_BLKSZ_2K  | McpCS_OP_STRB_ADDRESS | McpCS_OP_SPACE_CPU | McpCS_OP_IPL_7 | McpCS_OP_AVEC_ENABLED

/* CS5 Is the XILINX frame interrupt autovector. */
/*      Tg8CS5 0xfff8680d */
#define Tg8CS5 McpCS_BA_ADDRESS_MASK | McpCS_BA_BLKSZ_2K  | McpCS_OP_BYTE_BOTH    | McpCS_OP_RW_RO     | McpCS_OP_STRB_ADDRESS | McpCS_OP_SPACE_CPU | McpCS_OP_IPL_6 | McpCS_OP_AVEC_ENABLED
/*-------------------------------------------------------------------*/

/* CS6 is Write the CAM with an option control bit. */
/*      Tg8CS6 0x01107430 */
#define Tg8CS6 (Tg8CAM_BASE<<McpCS_BA_ADDRESS_SHIFT)  | McpCS_BA_BLKSZ_2K  | McpCS_OP_BYTE_BOTH  | McpCS_OP_RW_WO   | McpCS_OP_STRB_DATA | McpCS_OP_SPACE_BOTH | McpCS_OP_AVEC_EXTERNAL

/* CS7 is Read the CAM    */
/*      Tg8CS7 0x01106830 */
#define Tg8CS7 (Tg8CAM_BASE<<McpCS_BA_ADDRESS_SHIFT) | McpCS_BA_BLKSZ_2K   | McpCS_OP_BYTE_BOTH | McpCS_OP_RW_RO | McpCS_OP_STRB_ADDRESS | McpCS_OP_SPACE_BOTH| McpCS_OP_AVEC_EXTERNAL
/*-------------------------------------------------------------------*/

/* CS8 selects the DPRAM  */
/*      Tg8CS8 0x01007bf0 */
#define Tg8CS8 (Tg8DPM_BASE<<McpCS_BA_ADDRESS_SHIFT) | McpCS_BA_BLKSZ_2K   | McpCS_OP_BYTE_BOTH | McpCS_OP_RW_RW | McpCS_OP_STRB_ADDRESS | McpCS_OP_DSACK_EXTERNAL | McpCS_OP_SPACE_BOTH | McpCS_OP_AVEC_EXTERNAL
/*-------------------------------------------------------------------*/

/* CS9 is not used */
#define Tg8CS9 0        /* Not used by Tg8 */

/* CS10 is not used */
#define Tg8CS10 0       /* Not used by Tg8 */

#endif

/* eof tg8Hardware.h */


