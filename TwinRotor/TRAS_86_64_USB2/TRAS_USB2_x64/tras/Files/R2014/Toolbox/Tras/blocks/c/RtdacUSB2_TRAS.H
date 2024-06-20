/////////////////////////////////////////////////////////////////////
//
// RT-DAC/USB2 board - I/O procedures
// RTDACUSB2.H
//
//  Functions for the CYAPI.LIB library
//
//  Copyright (c) by InTeCo/2K, 2011
//  All Rights Reserved
//
// 2011-06-06 - Customised for TRAS model
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
// MATLAB_SFUNCTION - defined to build S-functions
//
#ifndef MATLAB_SFUNCTION
#ifndef EXT_MODE
#include "cyapi.h"
#endif
#endif

#define COMMAND_SEND            (0x8F0F)
#define COMMAND_READ            (0x8FFF)

// If defined ASYNCHRONOUS_TRANSFER - USB transfers data in asynchronous mode
// If undefined - synchronous data transfer selected
// #define ASYNCHRONOUS_TRANSFER

#define NO_OF_PWM         ( 2)  /* Max number of PWMs */
#define NO_OF_ENCODER     ( 2)  /* Max number of encoders (with and without index signal) */
#define NO_OF_TMRCNT      ( 1)  /* Max number of timer/counters */
#define NO_OF_GENERATOR   ( 0)  /* Max number of generators */
#define NO_OF_CHRONO      ( 0)  /* Max number of chronometers */
#define NO_OF_FREQM       ( 0)  /* Max number of frequency meters */

#define NO_OF_AD          (16)
#define NO_OF_DA          ( 4)

#define BINARY_BUFFER_SIZE   (1024)

typedef struct {
  unsigned int Mode;
  unsigned int Prescaler;
  unsigned int Width;
} PWMType;

typedef struct {
  unsigned int Reset;
  unsigned int IdxActive;
  unsigned int IdxInvert;
  long int     Counter;
} EncoderType;

typedef struct {
  unsigned int Reset;
  unsigned int Mode;
  unsigned int Counter;
} TmrCntType;

typedef struct {
  unsigned int St1Len;
  unsigned int St2Len;
  unsigned int NoOfPeriods;
  int          Enable;
  int          SwHwGateStartFlag;
  int          SwGate;
  int          SwStart;
  int          InvertGate;
  int          InvertStart; 
  int          InfiniteGeneration;
  int          St1Level;
  int          InitLevel;
  int          TerminateLevel;
} GeneratorType;

typedef struct {
  unsigned int EnableBlock;
  unsigned int TriggerMode;
  unsigned int EnableGate;
  unsigned int InvertStartStop;
  unsigned int InvertStop;
  unsigned int InvertGate;
  unsigned int NextMeasurement;
  unsigned int ArmMeasurement;
  unsigned int Status;
  unsigned int BlockState_Armed;
  unsigned int BlockState_Pending;
  unsigned int BlockState_Ready;
  unsigned int ClkDivider;
  unsigned int Counter;
  unsigned int Result;
} ChronoType;

typedef struct {
  unsigned int EnableBlock;
  unsigned int Ready;
  unsigned int Timer;
  unsigned int Result;
  unsigned int Counter;
  unsigned int Mode;
  unsigned int InfiniteFlag;
  unsigned int SwStart;
  unsigned int StartInv;
  unsigned int SwGate;
  unsigned int SwHwGateStartFlag;
  unsigned int GateInv;
  unsigned int GateMode;
  unsigned int InputInv;
} FreqMType;

typedef struct {
  unsigned int Gain;
  unsigned int Result;
  unsigned int Busy;
} ADType;

typedef struct {
  unsigned int  LogicVersion;
  char          ApplicationName[7];
  unsigned long LogicDate;
  unsigned int  NoOfChannels[12]; // [ xx xx xx Chrono FreqM GeneratorImpr TmrCnt Counter Timer EncoderI Encoder PWM ]
  unsigned int  PWMStatus;
  unsigned int  PWMFlag;
//  unsigned int  CN1PinMode;
  unsigned int  CN1Direction;
  unsigned int  CN1Output;
  unsigned int  CN1Input;
  PWMType       PWM[ NO_OF_PWM ];
  EncoderType   Encoder[ NO_OF_ENCODER ];
  TmrCntType    TmrCnt[ NO_OF_TMRCNT ];
//  GeneratorType Generator[ NO_OF_GENERATOR ];
//  ChronoType    Chrono[ NO_OF_CHRONO ];
//  FreqMType     FreqM[ NO_OF_FREQM ];
  ADType        AD[ NO_OF_AD ];
  unsigned int  DA[ NO_OF_DA ];
} RTDACUSB2BufferType;


#ifndef MATLAB_SFUNCTION
#ifndef EXT_MODE
typedef struct {
  USHORT VendorID, ProductID;
  char   Manufacturer[USB_STRING_MAXLEN+1];
  char   Product[USB_STRING_MAXLEN+1];
  char   SerialNumber[USB_STRING_MAXLEN+1];       
  char   FriendlyName[USB_STRING_MAXLEN+1];
  char   CompilationDate[USB_STRING_MAXLEN+1];
  char   ReleaseDate[USB_STRING_MAXLEN+1];
  UCHAR  DevClass;
  UCHAR  DevSubClass;
  UCHAR  DevProtocol;
  USHORT BcdDevice;
} USBDevicePropertiesType;

int USB2DeviceProperties( int Idx, USBDevicePropertiesType *pUSBDeviceProperties );

#endif
#endif

// Open and close USB device
int USB2Open( void );
int USB2OpenByIdx( int BoardIdx );
int USB2OpenBySerialNo( int SerialNo );
int USB2OpenBySerialNoStr( char *SerialNo );
int USB2Close( int Idx );

int USB2NumOfDevices( void );

int USB2MiddleLED( int Idx, int State );

int  CommandSendBinaryBuffer_TRAS( int Idx );
int  CommandSend_TRAS( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToSend );
int  CommandRead_TRAS( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToRead );


#define JTAG_TDO_REQUEST_MASK  (0x10)
#define JTAG_TDI_REQUEST_MASK  (0x20)
#define JTAG_TCK_REQUEST_MASK  (0x40)
#define JTAG_TMS_REQUEST_MASK  (0x80)


int JTAGSendFrame( int Idx, UCHAR Buffer[], LONG length );
int JTAGReadFrame( int Idx, UCHAR Buffer[], LONG inLength );


int JTAGSend( int Idx, unsigned char value );
int JTAGRead( int Idx );


///////////////////////////////////////////////////////////////////////////
//
// Error codes
//
#define RTDAC_OK                                (    0 )
#define RTDAC_TOO_MANY_USB_DEVICES              (   -1 )
#define RTDAC_CAN_NOT_FIND_USB_DEVICE           (   -2 )
#define RTDAC_TOO_HIGH_BOARD_INDEX              (   -3 )
#define RTDAC_CAN_NOT_ACCESS_ENDPOINTS          (   -4 )
#define RTDAC_CAN_NOT_ACCESS_NULL_ENDPOINT      (   -5 )
#define RTDAC_ENDPOINTS_NOT_CLOSED              (   -6 )
#define RTDAC_INVALIED_DEVICE_POINTER           (   -7 )
#define RTDAC_INVALIED_SYNCHRONOUS_OUT_TRANSFER (   -8 )
#define RTDAC_INVALIED_SYNCHRONOUS_IN_TRANSFER  (   -9 )
#define RTDAC_INVALIED_SYNCHR_JTAG_OUT_TRANSFER (  -10 )
#define RTDAC_INVALIED_SYNCHR_JTAG_IN_TRANSFER  (  -11 )
#define RTDAC_CAN_NOT_FIND_SERIAL_NUMBER        (  -12 )

int   USB2LastError( void );
char *USB2LastErrorMsg( void );

