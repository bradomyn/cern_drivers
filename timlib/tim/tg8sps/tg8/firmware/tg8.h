/*************************************************************************/
/* General public data structures needed by the Tg8 access routines.     */
/* These data are shared by the driver and the firmware programs.        */
/* Version 4.0 for TG8 firmware - Vova + Julian Sep 94                   */
/* Vladimir Kovaltsov for SL Timing, February, 1997			 */
/*************************************************************************/

/*************************************************************************/
/* If the module is already defined, dont do it again                    */
/*************************************************************************/

#ifndef TG8
#define TG8

#include <tg8Pub.h>
#include <tg8Hardw.h>
#include <selftest.h>

/**************************************************************************/
/* Mailbox operations codes						  */
/**************************************************************************/

typedef enum {
  Tg8OP_NO_COMMAND = 0,
  Tg8OP_PING_MODULE        = 1,  /* Ping the module */
  Tg8OP_GET_STATUS         = 2,  /* Get the firmware status words */
  Tg8OP_SET_SSC_HEADER     = 3,  /* Set the SSC header code */

  Tg8OP_SET_STATE          = 4,	 /* Modify actions state (Enabled/Disabled) */
  Tg8OP_SET_DELAY          = 5,  /* Modify actions delay value */
  Tg8OP_CLEAR_USER_TABLE   = 6,  /* Remove actions from the user table */
  Tg8OP_SET_USER_TABLE     = 7,  /* Insert actions into the user table */

  Tg8OP_GET_USER_TABLE     = 8,  /* Get a part of the user table */
  Tg8OP_GET_RECORDING_TABLE= 9,  /* Get a part of the Recording table */
  Tg8OP_GET_HISTORY_TABLE  = 10, /* Get a part of the Event History Table  */
  Tg8OP_GET_CLOCK_TABLE    = 11, /* Get a part of the Clock table */

  Tg8OP_SET_PPM_TIMING     = 12, /* Set actions for the PPM timing object */
  Tg8OP_SET_GATE           = 13, /* Write gate information for set of actions */
  Tg8OP_SET_DIM            = 14, /* Set actions dimension values */
  Tg8OP_GET_PPM_LINE       = 15, /* Get the PPM information from set of actions */
  Tg8OP_SIMULATE_PULSE     = 16, /* Simulate s pulse on a counter */
  Tg8OP_SELFTEST_RESULT    = 17  /* Get the selftest result */
} Tg8Operation;

/**************************************************************************/
/* Hardware registers as seen from the DSC site                           */
/**************************************************************************/

typedef enum {
  Tg8HR_INT_VME = 0x7fc,   /* Interrupt the VME master when MPC is writing to this location */
			   /* DSC should read this location to cleare interrupt signal. */
  Tg8HR_INT_MCP = 0x7fe,   /* Interrupt the MC68332 when writing to this location */
  Tg8HR_STATUS  = 0x800,   /* Read hardware status bits or RESET module */
		   /* Reading this location gives the status of the Tg8.
		      Significant bits in word are:
		      0 - 7: set  if the interrupt is made by counters 1 - 8 respectively
		      8: set if interrupt is made through DPRAM (Mailbox process)
		      9: set if timing interrupts are enabled, cleared otherwize
		      14: set if the ext clock 1 is used,
			  0 - if the 10MHz internal clock is used.
		      The rest bits:
		      10: set if Tg8 is in pending state (?)
		      11: set if XR Xilinx is loaded
		      12: set if X1 Xilinx is loaded
		      13: set if X2 Xilinx is loaded
		      15: set if Self-test is not OK (not implemented). */
		   /* Writing to this location: writing 1 - RESET for Tg8 */
  Tg8HR_INTERRUPT= 0x802,   /* Read Interrupt vector or Enable/Disable timing interrupts */
		   /* Reading this location gives the interrupt
		      vector used by Tg8 to interrupt VME. */
		   /* Writing:
		      Tg8ENABLE_INT: Enables interrupts from MTG
		      Tg8DISABLE_INT: Disables them
		      Tg8ENABLE_SYNC_INT: Enables interrupts, sync. with SSC (?)
		      Tg8DISABLE_SYNC_INT: Disables interrupts, sync. with SSC (?)
		   */
  Tg8HR_CLEAR_INT= 0x804,   /* Clear counter interrupts */
		   /* Writing bits 0-7  to this location clears the interrupts
		      generated by the Tg8's counters 1-8 respectively */
  Tg8HR_END = -1   /* End of enum list */
} Tg8HardwareRegister;

/**************************************************************************/
/* Constants used in communication with DPRAM                             */
/**************************************************************************/
typedef enum {
  Tg8ENABLE_INT = 0,	  /* Enable MTG (timing cable) interrupts */
  Tg8DISABLE_INT= 2,	  /* Disable them */
  Tg8ENABLE_SYNC_INT = 1, /* Enable synchonous interrupts */
  Tg8DISABLE_SYNC_INT= 3  /* Disable synchronous interrupts */
} Tg8TimingInterrupt;

/**************************************************************************/
/* Interrupt source bit masks                                             */
/**************************************************************************/

typedef enum {
  Tg8IS_MAILBOX = 0, /* The mailbox protocol handshake */
  Tg8IS_IMM     = 1, /* Immediate interrupts */
  Tg8IS_ERR     = 2  /* There are alarms */
} Tg8InterruptSource;

#define Tg8ISM_MAILBOX (1<<Tg8IS_MAILBOX)
#define Tg8ISM_IMM     (1<<Tg8IS_IMM)
#define Tg8ISM_ERR     (1<<Tg8IS_ERR)

/**************************************************************************/
/* Standard mbox header structure                                         */
/**************************************************************************/

typedef struct {
  Word	Row;	 /* The index of the 1st entry */
  Word  Number;  /* Number of entries to be processed */
} Tg8MbxOpHdr;

/**************************************************************************/
/* Read/Write Actions data block 					  */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr Hdr;	  /* Range of actions */
  Tg8User     Actions[1]; /* Action description(s) [var.length] */
} Tg8MbxRwAction;

/**************************************************************************/
/* Read/Write PPM Timing data block 					  */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr Hdr;	  /* Range of timing units */
  Tg8PpmUser  Actions[1]; /* Action description(s) [var.length] */
} Tg8MbxRwPpmAction;

/**************************************************************************/
/* Read/Write PPM Lines         					  */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr Hdr;	  /* Range of timing units */
  Tg8PpmLine  Lines[1];   /* PPM Action's line description(s) [var.length] */
} Tg8MbxRwPpmLine;

/**************************************************************************/
/* Read/Write action's Gate              				  */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr Hdr;	  /* Range of timing units */
  Tg8Gate     Gates[1];   /* Action's gate description(s) [var.length] */
} Tg8MbxRwGate;

/**************************************************************************/
/* Read/Write action's Dimension              				  */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr Hdr;	  /* Range of timing units */
  Byte        Dims[4];    /* Action's dimension [var.length] */
} Tg8MbxRwDim;

/**************************************************************************/
/* Read Action Recording table					          */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr  Hdr;	  /* Range of actions */
  Tg8Recording Recs[1];   /* Recording data [var.length] */
} Tg8MbxRecording;

/************************************/
/* Read the event history table     */
/************************************/

typedef struct {
  Tg8MbxOpHdr Hdr;	 /* Range of histories */
  Tg8History  Hist[1];   /* Entries */
} Tg8MbxHistory;

/**************************************************************************/
/* Read the Clock table 					          */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr Hdr;	 /* Range of clocks */
  Tg8Clock    Clks[1];   /* Clock data [var.length] */
} Tg8MbxClock;

/**************************************************************************/
/* Clear part of the User table 					  */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr	Hdr;	/* Range of actions */
} Tg8MbxClearAction;

/**************************************************************************/
/* Set Actions state data block						  */
/**************************************************************************/

typedef struct {
  Tg8MbxOpHdr	Hdr;	/* Range of actions */
  Word  	State,  /* Desirable state (Enabled/Disabled), delay */
                Aux;    /* Auxiliary value (clock type etc) */
} Tg8MbxActionState;

/**************************************************************************/
/* Block data transferred between the host and TG8                        */
/**************************************************************************/

  /* The BlockData space size (the address of the next location
     HrIntVme should be less than OR eq. 0x7fc) */

#define Tg8MBX_BUFF_SIZE (0x62A-5*2*Tg8GROUPS)

typedef union {
  struct {
    Word Mask;
    Word SpareMsk;
  } SimPulse;
  Tg8MbxHistory      HistoryBlock;
  Tg8MbxRwAction     RwAction;
  Tg8MbxRwPpmAction  RwPpmAction;
  Tg8MbxRwPpmLine    RwPpmLine;
  Tg8MbxRwGate       RwGate;
  Tg8MbxRwDim        RwDim;
  Tg8MbxRecording    Recording;
  Tg8MbxClock        ClockBlock;
  Tg8MbxClearAction  ClearAction;
  Tg8MbxActionState  ActionState;
  Tg8DateTime        DateTime;
  Tg8StatusBlock     StatusBlock;
  StDpmInfo          SelfTestResult;
  Byte               MbxBuff[Tg8MBX_BUFF_SIZE];
} Tg8BlockData;

/**************************************************************************/
/* DPRAM layout as seen from the DSC side                                 */
/**************************************************************************/

#ifndef Tg8DPRAM_SIZE
#define Tg8DPRAM_SIZE   2048  /* 0x800 , in bytes */
#endif

#if CONVERTOR
#define Tg8Dpm_spare Tg8DPM_BASE
#else
#define Tg8Dpm_spare 0
#endif

typedef struct Tg8Dpm {

/*==============================MEMORY START ADDRESS==================*/
  char DpmSpare_Bytes [Tg8Dpm_spare]; /* Need for asm() usage */

/*==============================EXCEPTION======================*/

  short ExceptionVector,   /* This field is set to 0 by the bootstrap
			      and isn't touched by the firmware program
			      when operating normally. If an unexpected
			      interrupt occurs the default ISR copies the
			      Format/VectorOffset field from Exception
			      frame to this location and stops the MC68332.
			      It may be restarted then only by RESET */
	Spare_ev;          /* Spare word to make the int32 word alignment */

  Uint ExceptionPC;        /* This field is set to 0 by the bootstrap
			      and isn't touched by the firmware program
			      when operating normally. If an unexpected
			      interrupt occurs the default ISR copies the
			      Exception PC field from Exception frame to
			      this location and stops the MC68332. */

/*==============================INTERRUPT TABLE=======================*/

  Tg8InterruptTable It;     /* Interrupt table */

/*==============================AUXILIARY TABLE=======================*/

  Tg8Aux At;                /* Auxiliary table */

/*==============================MAIL BOX=======================*/

  Tg8BlockData BlockData;   /* Block data union */

/*===========================SPECIAL LOCATION INSIDE the DPRAM=====================*/
  short HrIntVme;  /* 0x7fc Interrupt the VME master when MPC is writing to this location. */
                   /* DSC should read this location to cleare interrupt signal. */
  short HrIntMpc;  /* 0x7fe Interrupt the MC68332 when writing to this location 
		      (the present Tg8 driver does not use this facility) */
/*===========================<<<THE END>>>=====================*/
/*NOTE::*/
/* There a few special locations above the top of the DPRAM. */
/* See the hardware register definitions (in this file) for information. */

} Tg8Dpm;

#endif

/* EOF --tg8.h-- */
