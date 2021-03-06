/*************************************************************************/
/* Special prived data structures used by the Tg8's firmware program.    */
/* Vladimir Kovaltsov for SL Timing, February, 1997			 */

/* Fri 10th June Julian Lewis: Modifications as follows...            */

/*    Removed statically initialized memory so that program can be    */
/*    downloaded accross the VME bus.                                 */

/*    Added static memory initialization to the SoftInit routine for  */
/*    download capability                                             */

/*    Implemented SPS telegram support                                */

/*    Removed all logic to do with the old date/time formats for the  */
/*    legacy events for tg3, and PS tg8                               */

/*    Implemented full UTC time support based on the Tgv events       */

/*    The handling of the SUPER-CYCLE number in the old 0x20 header   */
/*    now returns the 32 bit UTC time instead of the 24 bit payload   */

/*    Some general tidy up of the code, in particular so that the O3  */
/*    optimization can be turned on hardware addresses are declared   */
/*    as volatile. This makes the code much faster.                   */

/*************************************************************************/

#include "tg8Priv.h"
#include "tg8User.h"
#include "macros.h"

/* Trapi ISR routines */

extern void ImmCompletion();  /* Trap to end processing of the IMM-queue */
extern void AtCompletion();   /* Trap to end processing of the AT-queue  */
extern void AtStartProcess(); /* Trap to start processing of incomming event */
extern void InsertToCam();    /* Trap to insert in CAM */
extern void ClearCam();       /* Clear CAM hardware trap */
extern void SetIntSourceMask(); /* Refresh the interrupts source mask */

extern void Abort_Isr();		/* Abort button ISR */
extern void Dsc_Isr();           /* The ISR for interrupts provoked by the host */
extern void Xr_Isr();		/* ISR for incomming timing frame */

extern void Spurious_Isr();	/* Spurious interrupts from DSC to 68332 */
extern void BusError_Isr();      /* Bus Error exception handler */
extern void AddressError_Isr();  /* Address Error exception handler */
extern void PrivViolation_Isr(); /* Privileges Violetion Error exceptions handler */
extern void Default_Isr();       /* Default ISR for the rest exceptions */

extern void Tpu0_Isr();		/* TPU channel 0 ISR routine */
extern void Tpu1_Isr();		/* TPU channels 1-8 ISR routines */
extern void Tpu2_Isr();
extern void Tpu3_Isr();
extern void Tpu4_Isr();
extern void Tpu5_Isr();
extern void Tpu6_Isr();
extern void Tpu7_Isr();
extern void Tpu8_Isr();

void AtProcess();	/* Process any incomming events for actions with counting */
void ImmProcess();	/* Process any incomming events for immediate actions only */
void StartActions();    /* Post-process of started actions */

static void SoftInit();	  /* Do the whole software initialization */
static void Init();	  /* Initialization process */
static void MbxProcess(); /* Main MailBox routine = Idle loop */
static int  GetXConfiguration(int cw);

static void memset(volatile short *m,short v,unsigned int bc);
static void memcpy16(short *d,short *s,int bc);

/*************************************************************************/
/* Global Variables */

volatile McpSim * sim; /* System Integration Module structure */
volatile McpTpu * tpu; /* Time processor unit */
volatile Tg8Xlx * xlx; /* XILINX 1 & 2 Counters */
volatile short  * cam; /* Content addressed memory */
volatile Tg8Dpm * dpm; /* Pointer to DPRAM structure */

/*************************************************/
/* Locations used by XrIsr to keep the registers */
/*************************************************/

short mbx_ccr,imm_ccr;	   /* Conditional codes register save area */
long  mbx_pc ,imm_pc;      /* Current PC value at an interruption  */
long  mbx_context[16],
      imm_context[16],
      xr_context [16];
long  context_c[3]; /* Registers save area in the "outputs history" ISRs */
long  regic[5];     /* Registers save area for trap handlers */

/*************************************************/
/* Event reception                               */
/*************************************************/

Tg8Event timing_frame;	/* Here the timing frame is read */
struct {
   Byte Err;     /* The receiver error code */
   Byte Hour;    /* Hour value lies in the same word */
} rcv_error;     
           
/***********************************************/
/* Data needed to insert a record into the CAM */
/***********************************************/

static struct {
   Word	              Addr;      /* Location address */
   Tg8LongOrTwoShorts Event;	 /* Event code to be inserted (32 bits) */
   Tg8ShortOrTwoBytes Last_word; /* Extra 16 bits (0 always for the SL) */
} ins, ins_sav;

/*******************/
/* Local variables */
/*******************/

static struct {
  Tg8ShortOrTwoBytes mm;        /* Need to change byte in the DPRAM */
  short * tpu_parameters[16];	/* Functions params for 16 channels of the TPU */
} var;

static Tg8Recording sim_rec;   /* Recording data for the pulse simulation */
static Tg8Interrupt sim_int;

static unsigned long utc;       /* UTC second sent in B5,B6 events */
static unsigned long utcl;      /* UTC second latched */
unsigned char dm[12];           /* Days in the month */

/*****************/
/* Events in use */
/*****************/

Uint wild_c[8]; /* Wildcard possible combinations */
Byte time_event_index[16]; /* Table for decoding of event header */

struct {
  Byte Simple_event[256],    /* Usage flags for all simple events only */
       SimpleH_wc[256],      /* Wildcard combinations for event headers */
       Simple_wc[256],       /* Wildcard combinations for event codes (with output)  */
       Simple_wc_imm[256];   /* Wildcard combinations for event codes (just immediate) */
  Byte Time_event[256],      /* Usage flags for ms,time,date events only */
       TimeH_wc[16],         /* Wildcard combinations for event headers */
       Time_wc[256],         /* Wildcard combinations for event codes (with output)  */
       Time_wc_imm[256];     /* Wildcard combinations for event codes (just immediate) */
} in_use;

/*************************************************************************/

Word              camBusy;  /* Busy flag for the CAM */
Word              doImm;    /* Generate VME bus interrupts if >0 */

EProg             eprog;    /* Event processing program state */
InterActTable     act;      /* Internal Form of the User Table */
InterClockTable   clk;      /* Clock table */
StartedAct        match;    /* The list of started actions */
Queue             atQueue,  /* The action process queue */
                  immQueue; /* The immediate action process queue */
InterHistoryTable hist;	    /* History table */
InterTelegram     tel;      /* Telegram buffers */

/* Test execution Information section */
StDpmInfo         info;

/*EOF tg8P.h */
