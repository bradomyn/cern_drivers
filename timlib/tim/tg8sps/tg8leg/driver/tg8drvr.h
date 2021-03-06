/***************************************************************************/
/* Include file to provide the Tg8 device driver interface.                */
/* These definitions are shared by the driver and the applications.        */
/* Vladimir Kovaltsov for the SL Version, February, 1997		   */
/***************************************************************************/

/* Dont define the module more than once */

#ifndef TG8DRVR
#define TG8DRVR

#include <tg8Pub.h>     /* Public declarations */
#include <tg8User.h>    /* Public declarations to manipulate with the control word */
#include <tg8Hardw.h>
#include <selftest.h>

#define Tg8DrvrMODULES  4   /* Max. number of the Tg8 mod. per DSC */
#define Tg8DrvrDEVICES  8   /* Max. number of devices (e.g. clients) per module */
#define Tg8DrvrCARDS   16   /* Max. number of the Tg8 cards per a DSC */

/*****************************/
/* The module address.       */
/*****************************/

typedef struct {
  void    * VMEAddress;		 /* Base address */
  Uint      InterruptVector;	 /* Interrupt vector number */
  Uint      InterruptLevel;	 /* Interrupt level (2 usually) */
  Uint      SwitchSettings;	 /* Switch settings */
  Uint      SscHeader;           /* SSC Header code (it sets up the acc. machine) */
} Tg8ModuleAddress;

/**************************************************************************/
/* DPRAM section                                                          */
/**************************************************************************/

typedef struct {
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
  Tg8InterruptTable It;    /* Interrupt table */
  Tg8Aux            At;    /* Auxiliary table */
  Word              Hw;    /* Hardware status -- used only by the driver */
} Tg8Dpram;

/*******************************/
/* The DPRAM test result block */
/*******************************/

typedef struct {
  Word Status;         /* Result status */
  Word Addr;           /* Faulty address */
  Word Val,Back;       /* Value was written, read Back */
} Tg8TestDpram;

/*******************************/
/* Tg8 Auto Test result block  */
/*******************************/

typedef struct {
  Word Test;          /* Test selector */
  Word Res;           /* Result code: 0=OK,0x8000=BUS Error */
  Word V1,V2;         /* Extra data */
} Tg8AutoTest;

typedef enum {
  Tg8CARD_RESET=1,    /* Do the reset */
  Tg8CARD_CONF,       /* Check the card configuration */
  Tg8CARD_STAT,       /* Check the status register */
  Tg8CARD_ACK,        /* Check the card's boot program acknowledge */
  Tg8CARD_MBX,        /* Check the mailbox protocol support */
  Tg8CARD_XR          /* Check the XILINX Receiver */
} Tg8CardTest;

/**************************************************************************/
/* The most important information about of the Super Cycle                */
/**************************************************************************/

typedef struct {
  Uint ScNumber;        /* The Super Cycle number */
  Uint ScLength;        /* The last completed S-Cycle length */
  Uint ScTime;          /* The current value of the S-Cycle time */
  Uint ScCounter;       /* Number of SC received since the last Reset */
} Tg8SuperCycleInfo;

/***************************************************************/
/* Incoming timing event's details for 'read' events operation */
/***************************************************************/

typedef struct {
   Uint         Id;     /* Timing object identifier */
   Tg8Event     Event;  /* Declared timing event (wildcards allowed) */
   Tg8Interrupt Inter;  /* Interrupt information */
   Word	        Alarms; /* The firmware alarms bit mask ORed with the driver alarms bit mask */
   Byte         FwStat; /* Firmware status */
   Byte         DevMask;/* Target devices bitmask */
} Tg8DrvrEvent;

/***************************************************************/
/* Device close mode                                           */
/***************************************************************/

typedef enum {
  Tg8ONCL_LEAVE,        /* Leave the card in working state == DEFAULT Value */
  Tg8ONCL_DISABLE,      /* Disable the card asynchronously */
  Tg8ONCL_CLEAR,        /* Disable the card asynchronously AND clear the user table */
  Tg8ONCL_DISABLE_SYNC  /* Disable the card synchronously */
} Tg8DrvrOnCloseMode;

typedef struct {
  Word Mode;            /* What to do by close device down (according to the Tg8DrvrOnCloseMode) */
  Word SimPulseMask;    /* Set of pulses to be simulated */
} Tg8DrvrOnClose;

/***************************************************************/
/* Union of any possible data to be transmitted via the ioctl  */
/***************************************************************/

typedef union {
  struct {
    Uint Row,Cnt;   /* The 1st entry number and number of entries */
  } Range;          /* Range of smth */

  int  TimeOut;     /* Set the timeout value */
  int  SscHeader;   /* Set the SSC Header code */
  int  QueueLength; /* Get the queue length */
  int  Trace;       /* Read the trace location */
  int  ObjectId;    /* Remove the timing object by its ID */
  int  SimPulseMask;/* Simulate pulses */

  char Ver[16];     /* Get the driver OR the firmware version */

  Tg8InterruptTable IntTable;    /* Read the interrupt table */
  Tg8Aux            AuxTable;    /* Read the auxiliary table */
  Tg8DateTime       DateTime;    /* Read the current date and time */
  Tg8Dpram          Dpram;       /* Read the DPRAM contents */

  Tg8TestDpram      TestDpram;   /* Do the DPRAM test */
  Tg8AutoTest       AutoTest;    /* Do the card autotest */

  Tg8DrvrEvent      Event;       /* Wait for the given event */
  Tg8Object         Object;      /* Write the timing object */
  Tg8SuperCycleInfo ScInfo;      /* Get the most important information
				    about of the Super Cycle */
  Tg8DrvrOnClose    CloseMode;   /* Close down the device gracefully */
  Tg8FirmwareObject FirmwareObject; /* Install the new firmware object */

  struct {
    int Index;                   /* Module index */
    Tg8ModuleAddress Address;    /* Its address */
  } InsModule;   /* Install the new Tg8 module */

  struct {
    Uint             DevMasks[Tg8DrvrMODULES];  /* Bitmap of opened devices for each module */
    Tg8ModuleAddress Addresses[Tg8DrvrMODULES]; /* Module's hardware description */
  } GetConfig;   /* Get the driver's current configuration */

  struct {
    Tg8StatusBlock Sb;          /* Status block */
    Tg8DrvrOnClose Cm;          /* Device Close mode */
    StDpmInfo      Res;         /* The SelfTest execution result */
    int      ErrCnt;            /* Total number of errors erased on module */
    int      ErrCode;           /* The last error code */
    char     ErrMsg[128];       /* The last error message text */
  } RawStatus;   /* Read raw status blobk */

  struct {
    Tg8Event Event;     /* Event code with wildcards */
    int      Matches;   /* Number of matches (output) */
  } Filter;      /* Connect to events (use an event as a filter) */

  struct {
    int Pid;            /* Owner's pid */
    int Signal;         /* Signal to be delivered to process pid (0=not used) */
  } Signal;      /* Set/Get signal & pid values */

  struct {
    int  Machine;        /* Acc. machine number */
    Word Data[Tg8GROUPS];/* Telegram buffer */
  } Telegram;    /* Get telegram */

  struct {
    Uint Row,Cnt;        /* The 1st action and number of actions */
    int  State;          /* Desirable state (enabled/disabled) */
  } ActState;    /* Set the action's state */

  struct {
    int Status;          /* Software status bitmask */
    int Alarms;          /* Alarms bitmask */
  } Status;      /* Read the module's status */

  struct {
    Uint Row,Cnt;        /* The 1st action and number of actions */
    Tg8User Table[Tg8ACTIONS]; /* Actions */
  } UserTable;   /* Read/write the user table */

  struct {
    Uint Row,Cnt;        /* The 1st action and number of actions */
    Tg8Recording Table[Tg8ACTIONS]; /* Recording info */
  } RecTable;    /* Read the recording table */

  struct {
    Uint Cnt;            /* Number of the last entries */
    Tg8History Table[Tg8HISTORIES]; /* Histories */
  } HistTable;   /* Read the events history table */

  struct {
    Uint Cnt;            /* Number of the last entries */
    Tg8Clock Table[Tg8CLOCKS]; /* Clock errors info */
  } ClockTable;  /* Read the clock table */

  struct {
    int Length;         /* Number of objects in the array */
    int Id[Tg8ACTIONS]; /* Array of object IDs */
  } ObjectsList; /* Get all known object IDs */

  struct {
    Word      Act;      /* The 1st action number */
    Word      Dim;      /* Dimension */
    Tg8Object Object;   /* Timing object */
  } ObjectDescr; /* Get the timing object description */

  struct {
    int  Mask;          /* Mask on connected lines */
    int  Id;            /* Object identifier*/
  } ObjectConnect; /* Connect to the set of lines of the timing object */

  struct {
    int  Id;            /* Object identifier */
    Word Line;          /* Line number */
    Word Sel;           /* Parameter selector (state/delay) */
    Word Value,         /* Desirable value */
         Aux;           /* Second value (clock type) */
  } ObjectParam; /* Change some object's parameter */

  struct {
    Uint Row,Cnt;        /* The 1st action and number of actions */
    Tg8PpmUser Table[Tg8ACTIONS]; /* Timing units */
  } PpmUserTable;   /* Write the PPM user table */

  struct {
    Uint Row,Cnt;        /* The 1st action and number of actions */
    Tg8PpmLine Table[Tg8ACTIONS]; /* Actions */
  } PpmLineTable;   /* Read the PPM lines */

} Tg8IoBlock;

/***************************************************************************/
/* IO Control functions                                                    */
/***************************************************************************/

typedef enum {

  /** Module level functions **/

	Tg8DrvrDEBUG_ON,		/* Log debug info on console */
	Tg8DrvrDEBUG_OFF,		/* Stop log */

	Tg8DrvrGET_DRI_VERSION,		/* Get the driver version date */
	Tg8DrvrGET_FIRMWARE_VERSION,	/* Get firmware version */

	Tg8DrvrSET_TIME_OUT,		/* Set Read/Write timeout */
	Tg8DrvrSET_SSC_HEADER,		/* Set the SSC Header code */

	Tg8DrvrENABLE_SYNC,     /* Enable outputs and interrupts synchronously with the SSC frame */
	Tg8DrvrDISABLE_SYNC,    /* Disable outputs and interrupts synchronously with the SSC frame */
	Tg8DrvrENABLE_MODULE,   /* Enable outputs and interrupts */
	Tg8DrvrDISABLE_MODULE,  /* Disable outputs and interrupts */
	Tg8DrvrRESET_MODULE,    /* Hardware reset and disable */

	Tg8DrvrINSTALL_MODULE,     /* Install the new module */
	Tg8DrvrGET_CONFIGURATION,  /* Get the current configuration */

	Tg8DrvrSIMULATE_PULSE,  /* Simulate a pulse */

	Tg8DrvrUSER_TABLE,      /* Get a part of the user table */
	Tg8DrvrRECORDING_TABLE, /* Get a part of the Recording table */
	Tg8DrvrINTERRUPT_TABLE, /* Get a part of the Interrupt table */
	Tg8DrvrHISTORY_TABLE,   /* Get a part of the Event History Table  */
	Tg8DrvrCLOCK_TABLE,     /* Get a part of the Clock table */
	Tg8DrvrAUX_TABLE,       /* Get the Auxiliary table */
	Tg8DrvrSC_INFO,         /* Get the most important information about of the Super Cycle */

	Tg8DrvrTELEGRAM,        /* Read a telegram buffer for given machine */

	Tg8DrvrTRACE_FIRMWARE,  /* Get trace data */
	Tg8DrvrDATE_TIME,       /* Get Date and time */
	Tg8DrvrGET_RAW_STATUS,  /* Read current status directly from the DPRAM */
	Tg8DrvrGET_STATUS,      /* Get the module status (use the Mbx protocol) */
	Tg8DrvrGET_DPRAM,	/* Read the whole DPRAM content */

	Tg8DrvrRELOAD_FIRMWARE, /* Reload firmware */
	Tg8DrvrRELOAD_ACTIONS,  /* Reload actions table */

  /** Connection (application) level functions **/

	Tg8DrvrON_CLOSE,        /* Close device mode */
	Tg8DrvrSET_SIGNAL,	/* Set Pid & Signal codes */
	Tg8DrvrGET_SIGNAL,	/* Get Pid & Signal codes */

	Tg8DrvrGET_QUEUE_LENGTH,/* Get event queue length */
	Tg8DrvrPURGE_QUEUE,	/* Discard all events on the Queue */

  /** Action (user table) level functions **/

	Tg8DrvrSET_ACTION_STATE,  /* Set the action's state (Enabled/Disabled) */
	Tg8DrvrCLEAR_ACTION,      /* Clear the action in the driver and firmware */

	Tg8DrvrWAIT_EVENT,        /* Synchronous wait for the given event */
	Tg8DrvrFILTER_EVENT,      /* Filter events in the user table */

	Tg8DrvrGET_PPM_LINE,      /* Get PPM info from action's line */

  /** Object level functions **/

	Tg8DrvrSET_OBJECT,      /* Insert the timing object */
	Tg8DrvrREMOVE_OBJECT,   /* Remove the timing object */
	Tg8DrvrGET_OBJECT,      /* Get the timing object's details (selected by id) */
	Tg8DrvrCONNECT,         /* Connect to the timing object */
	Tg8DrvrOBJECT_PARAM,    /* Change object's parameters */
	Tg8DrvrOBJECTS_LIST,    /* Get the list of all known object IDs */

  /* Test level functions */
	Tg8DrvrTEST_DPRAM,      /* Do the dpram test */
	Tg8DrvrCARD_TEST,       /* Do the card tests */

  /* The last entry defines the number of IOCTL functions */
	Tg8DrvrCONTROL_FUNCTIONS
} Tg8DrvrControlFunction;

#define Tg8DrvrNORMAL_SOFT_STATUS (Tg8DrvrMODULE_RUNNING|\
				   Tg8DrvrMODULE_FIRMWARE_LOADED|\
				   Tg8DrvrMODULE_ENABLED)

#endif

/* eof Tg8Drvr.h */
