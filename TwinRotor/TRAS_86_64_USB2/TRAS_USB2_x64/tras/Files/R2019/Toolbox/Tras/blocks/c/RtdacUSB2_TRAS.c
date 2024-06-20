/////////////////////////////////////////////////////////////////////
//
// RT-DAC/USB2 board - I/O procedures
//
//  Copyright (c) by InTeCo/2K, 2011
//  All Rights Reserved
//
//  Version    Date     Description
//    1.0   2011.06.06  TRASinterface
//
//
/////////////////////////////////////////////////////////////////////

#include "string.h"
#include "stdio.h"

#include "rtdacusb2_tras.h"
#include "cyapi.h"
#include "windows.h"


typedef struct {
  CCyUSBDevice    *USBDevice;  
  CCyBulkEndPoint *FPGAOutEpt;
  CCyBulkEndPoint *FPGAInEpt;
  CCyBulkEndPoint *JTAGOutEpt;
  CCyBulkEndPoint *JTAGInEpt;
  CCyControlEndPoint *ControlEpt;
#ifdef ASYNCHRONOUS_TRANSFER
  OVERLAPPED outOvLap, inOvLap;
  UCHAR *inContext, *outContext;
#endif
  USBDevicePropertiesType USBDeviceProperties;
} USBDeviceType;

#ifdef ASYNCHRONOUS_TRANSFER
#define USBDeviceINIT { NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, NULL }
#else
#define USBDeviceINIT { NULL, NULL, NULL, NULL, NULL }
#endif

// Up to 16 device can operate simultaneously
#define MAXUSBDEVICES (16)
static USBDeviceType USBDeviceArr[MAXUSBDEVICES] = { USBDeviceINIT, USBDeviceINIT, USBDeviceINIT, USBDeviceINIT, 
                                                     USBDeviceINIT, USBDeviceINIT, USBDeviceINIT, USBDeviceINIT,
								                     USBDeviceINIT, USBDeviceINIT, USBDeviceINIT, USBDeviceINIT,
								                     USBDeviceINIT, USBDeviceINIT, USBDeviceINIT, USBDeviceINIT };

//static CCyUSBDevice *USBDevice = NULL;

//  8 bits buffers - applied by CyAPI functions
unsigned char UCHARBufferToSend[ BINARY_BUFFER_SIZE ];   // Binary data send to the RT-DAC/USB device   ( 8 bits)
unsigned char UCHARBufferToRead[ BINARY_BUFFER_SIZE ];   // Binary data read from the RT-DAC/USB device ( 8 bits)
// 16 bits buffers - applied by FPGA logic
unsigned int       BufferToSend[ BINARY_BUFFER_SIZE/2 ]; // Binary data send to the RT-DAC/USB device   (16 bits)
unsigned int       BufferToRead[ BINARY_BUFFER_SIZE/2 ]; // Binary data read from the RT-DAC/USB device (16 bits)

//////////////////////////////////////////////////////////////////////////////////
//
// Last error code
//
static int LastError = RTDAC_OK;


//////////////////////////////////////////////////////////////////////////////////
//
// Open the RT-DAC/USB2 device
// Called when only a single board is connected
//
// Return value:
//    0..(MAXUSBDEVICES-1) - device handler (open succeeded)
//    a negative nalue - open device error
//
int USB2Open( void ) {
	return USB2OpenByIdx( 0 );
}

//////////////////////////////////////////////////////////////////////////////////
//
// Open the RT-DAC/USB2 device
// 
// Parameters:
//    BoardIdx - board index.Between 0 and USBDevice->DeviceCount()-1
//
// Return value:
//    0..(MAXUSBDEVICES-1) - device handler (open succeeded)
//    a negative nalue - open device error
//
int USB2OpenByIdx( int BoardIdx ) {
  int i, Idx;
  int Devices, EptCount;

  // Find empty location in the USBDeviceArr array
  for( Idx=0; Idx<MAXUSBDEVICES+1; Idx++ ) {
	  if( Idx == MAXUSBDEVICES ) { // Too many USB devices
	    LastError = RTDAC_TOO_MANY_USB_DEVICES; return LastError;
	  }
	if( (USBDeviceArr[Idx].FPGAOutEpt == NULL) && 
		(USBDeviceArr[Idx].FPGAInEpt  == NULL) &&
		(USBDeviceArr[Idx].JTAGOutEpt == NULL) &&
		(USBDeviceArr[Idx].JTAGInEpt  == NULL) )
	  break;
  } 

  if( USBDeviceArr[Idx].USBDevice == NULL ) 
	USBDeviceArr[Idx].USBDevice = new CCyUSBDevice( /*m_hWnd*/ ); // Create an instance of CCyUSBDevice
  Devices = USBDeviceArr[Idx].USBDevice->DeviceCount();
  if( Devices < 1 ) {
    delete USBDeviceArr[Idx].USBDevice; USBDeviceArr[Idx].USBDevice = (CCyUSBDevice *)NULL;
	LastError = RTDAC_CAN_NOT_FIND_USB_DEVICE; return LastError;
  }

  if( BoardIdx > Devices-1 ) {
    delete USBDeviceArr[Idx].USBDevice; USBDeviceArr[Idx].USBDevice = (CCyUSBDevice *)NULL;
	LastError = RTDAC_TOO_HIGH_BOARD_INDEX; return LastError;
  }
  int ret = USBDeviceArr[Idx].USBDevice->Open( BoardIdx );

  // Find bulk endpoints in the EndPoints[] array
  EptCount = USBDeviceArr[Idx].USBDevice->EndPointCount();
  for ( i=1; i<EptCount; i++ ) {
	switch( (int)(USBDeviceArr[Idx].USBDevice->EndPoints[i]->Address & 0x7F) ) {
	  case 2:
        USBDeviceArr[Idx].FPGAOutEpt = (CCyBulkEndPoint *) USBDeviceArr[Idx].USBDevice->EndPoints[i]; break;
	  case 4:
        USBDeviceArr[Idx].FPGAInEpt =  (CCyBulkEndPoint *) USBDeviceArr[Idx].USBDevice->EndPoints[i]; break;
	  case 6:
        USBDeviceArr[Idx].JTAGOutEpt = (CCyBulkEndPoint *) USBDeviceArr[Idx].USBDevice->EndPoints[i]; break;
	  case 8:
        USBDeviceArr[Idx].JTAGInEpt =  (CCyBulkEndPoint *) USBDeviceArr[Idx].USBDevice->EndPoints[i]; break;
	}
  } 
  if( (USBDeviceArr[Idx].FPGAOutEpt == NULL) || 
	  (USBDeviceArr[Idx].FPGAInEpt  == NULL) ||
	  (USBDeviceArr[Idx].JTAGOutEpt == NULL) ||
	  (USBDeviceArr[Idx].JTAGInEpt  == NULL) ) {
    delete USBDeviceArr[Idx].USBDevice; USBDeviceArr[Idx].USBDevice = (CCyUSBDevice *)NULL;
	LastError = RTDAC_CAN_NOT_ACCESS_ENDPOINTS; return LastError;
  } 
  USBDeviceArr[Idx].ControlEpt = (CCyControlEndPoint *) USBDeviceArr[Idx].USBDevice->ControlEndPt;

  USBDeviceArr[Idx].FPGAOutEpt->Reset();
  USBDeviceArr[Idx].FPGAInEpt ->Reset();
  USBDeviceArr[Idx].JTAGOutEpt->Reset();
  USBDeviceArr[Idx].JTAGInEpt ->Reset();
  USBDeviceArr[Idx].FPGAOutEpt->Abort();
  USBDeviceArr[Idx].FPGAInEpt ->Abort();
  USBDeviceArr[Idx].JTAGOutEpt->Abort();
  USBDeviceArr[Idx].JTAGInEpt ->Abort();

  // 1000ms = 1sec - minimum value
  USBDeviceArr[Idx].FPGAOutEpt->TimeOut = 1000;
  USBDeviceArr[Idx].FPGAInEpt ->TimeOut = 1000;
  USBDeviceArr[Idx].JTAGOutEpt->TimeOut = 1000;
  USBDeviceArr[Idx].JTAGInEpt ->TimeOut = 1000;

  USBDeviceArr[Idx].USBDeviceProperties.ProductID   = USBDeviceArr[Idx].USBDevice->ProductID;
  USBDeviceArr[Idx].USBDeviceProperties.VendorID    = USBDeviceArr[Idx].USBDevice->VendorID;
  USBDeviceArr[Idx].USBDeviceProperties.DevClass    = USBDeviceArr[Idx].USBDevice->DevClass;
  USBDeviceArr[Idx].USBDeviceProperties.DevSubClass = USBDeviceArr[Idx].USBDevice->DevSubClass;
  USBDeviceArr[Idx].USBDeviceProperties.DevProtocol = USBDeviceArr[Idx].USBDevice->DevProtocol;
  USBDeviceArr[Idx].USBDeviceProperties.BcdDevice   = USBDeviceArr[Idx].USBDevice->BcdDevice;

  for( i=0;i<USB_STRING_MAXLEN;i++ ) {
    USBDeviceArr[Idx].USBDeviceProperties.Manufacturer[i] = (char)USBDeviceArr[Idx].USBDevice->Manufacturer[i];
	if( USBDeviceArr[Idx].USBDevice->Manufacturer[i] == 0 ) break;
  }
  for( i=0;i<USB_STRING_MAXLEN;i++ ) {
    USBDeviceArr[Idx].USBDeviceProperties.Product[i] = (char)USBDeviceArr[Idx].USBDevice->Product[i];
	if( USBDeviceArr[Idx].USBDevice->Product[i] == 0 ) break;
  }
  for( i=0;i<USB_STRING_MAXLEN;i++ ) {
    USBDeviceArr[Idx].USBDeviceProperties.SerialNumber[i] = (char)USBDeviceArr[Idx].USBDevice->SerialNumber[i];
	if( USBDeviceArr[Idx].USBDevice->SerialNumber[i] == 0 ) break;
  }
  for( i=0;i<USB_STRING_MAXLEN;i++ ) {
    USBDeviceArr[Idx].USBDeviceProperties.FriendlyName[i] = (char)USBDeviceArr[Idx].USBDevice->FriendlyName[i];
	if( USBDeviceArr[Idx].USBDevice->FriendlyName[i] == 0 ) break;
  }

#ifdef ASYNCHRONOUS_TRANSFER
  USBDeviceArr[Idx].outOvLap.hEvent = CreateEvent(NULL, false, false, "FPGA_OUT");
  USBDeviceArr[Idx].inOvLap.hEvent  = CreateEvent(NULL, false, false, "FPGA_IN");
#endif


#ifdef ASYNCHRONOUS_TRANSFER_____ // ten fragment wysypuje transfer asynchroniczny ???
  LONG inLength = 1024;
  for(i=0;i<2;i++) {
    USBDeviceArr[Idx].inContext  = USBDeviceArr[Idx].FPGAInEpt->BeginDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap);
    USBDeviceArr[Idx].FPGAInEpt->WaitForXfer(&USBDeviceArr[Idx].inOvLap,10);
    USBDeviceArr[Idx].FPGAInEpt->FinishDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap,USBDeviceArr[Idx].inContext);
  }
  for(i=0;i<2;i++) {
    USBDeviceArr[Idx].inContext  = USBDeviceArr[Idx].JTAGInEpt->BeginDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap);
    USBDeviceArr[Idx].JTAGInEpt->WaitForXfer(&USBDeviceArr[Idx].inOvLap,10);
    USBDeviceArr[Idx].JTAGInEpt->FinishDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap,USBDeviceArr[Idx].inContext);
  }
#endif

  return Idx;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Open the RT-DAC/USB2 device pointed by the serial number
// Opens the first device when more then one device with
// the given serial number found
// 
// Parameters:
//    SerialNo - serial number of the board (as integer value)
//
// Return value:
//    0..(MAXUSBDEVICES-1) - device handler (open succeeded)
//    a negative nalue - open device error
//
int USB2OpenBySerialNo( int SerialNo ) {
  int NoOfDevices = USB2NumOfDevices();
  int Idx, iAux;
  USBDevicePropertiesType USBDevProp;

  NoOfDevices = USB2NumOfDevices();
  for(int i=0; i<NoOfDevices+1; i++ ) {
	if( i == NoOfDevices ) {
      delete USBDeviceArr[Idx].USBDevice; USBDeviceArr[Idx].USBDevice = (CCyUSBDevice *)NULL;
	  LastError = RTDAC_CAN_NOT_FIND_SERIAL_NUMBER; return LastError;
	}
    Idx = USB2OpenByIdx( i );
    USB2DeviceProperties( Idx, &USBDevProp );
    sscanf(USBDevProp.SerialNumber, "%d", &iAux );
	if( SerialNo == iAux ) break;
	  USB2Close( Idx );
  }

  return Idx;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Open the RT-DAC/USB2 device pointed by the serial number string
// Opens the first device when more then one device with
// the given serial number found
// 
// Parameters:
//    SerialNo - serial number of the board (as char[])
//
// Return value:
//    0..(MAXUSBDEVICES-1) - device handler (open succeeded)
//    a negative nalue - open device error
//
int USB2OpenBySerialNoStr( char *SerialNo ) {
  int iSerialNo = 0;
  sscanf( SerialNo, "%d", &iSerialNo );
  return USB2OpenBySerialNo( iSerialNo );
}

//////////////////////////////////////////////////////////////////////////////////
//
// Close the RT-DAC/USB2 device
//
// Arguments:
//    Idx - device handler (number from 0 to MAXUSBDEVICES-1)
//
// Return value:
//    0 - close succeeded
//    a negative nalue - close operation error
//
int USB2Close( int Idx ) {
  int i;

  if( USBDeviceArr[Idx].USBDevice == NULL ) {
	LastError = RTDAC_INVALIED_DEVICE_POINTER; return LastError;
  }

  if( (USBDeviceArr[Idx].FPGAOutEpt == NULL) || 
	  (USBDeviceArr[Idx].FPGAInEpt  == NULL) ||
	  (USBDeviceArr[Idx].JTAGOutEpt == NULL) ||
	  (USBDeviceArr[Idx].JTAGInEpt  == NULL) ) {
	LastError = RTDAC_CAN_NOT_ACCESS_NULL_ENDPOINT; return LastError;
  }

  USBDeviceArr[Idx].ControlEpt = NULL; 
  USBDeviceArr[Idx].FPGAOutEpt = NULL; 
  USBDeviceArr[Idx].FPGAInEpt  = NULL;
  USBDeviceArr[Idx].JTAGOutEpt = NULL;
  USBDeviceArr[Idx].JTAGInEpt  = NULL;

#ifdef ASYNCHRONOUS_TRANSFER
  CloseHandle(USBDeviceArr[Idx].outOvLap.hEvent);
  CloseHandle(USBDeviceArr[Idx].inOvLap.hEvent);
#endif

  for( i=0; i<MAXUSBDEVICES; i++ ) {
	if( (USBDeviceArr[i].FPGAOutEpt != NULL) || 
		(USBDeviceArr[i].FPGAInEpt  != NULL) ||
		(USBDeviceArr[i].JTAGOutEpt != NULL) ||
		(USBDeviceArr[i].JTAGInEpt  != NULL) ) {
	      LastError = RTDAC_ENDPOINTS_NOT_CLOSED; return LastError;
	}
  } 

  USBDeviceArr[Idx].USBDeviceProperties.ProductID = USBDeviceArr[Idx].USBDeviceProperties.VendorID = 0;
  USBDeviceArr[Idx].USBDeviceProperties.DevClass = USBDeviceArr[Idx].USBDeviceProperties.DevSubClass = 0;
  USBDeviceArr[Idx].USBDeviceProperties.DevProtocol = 0;
  USBDeviceArr[Idx].USBDeviceProperties.BcdDevice = 0;
  USBDeviceArr[Idx].USBDeviceProperties.Manufacturer[0] = '\0';
  USBDeviceArr[Idx].USBDeviceProperties.Product[0] = '\0';
  USBDeviceArr[Idx].USBDeviceProperties.SerialNumber[0] = '\0';
  USBDeviceArr[Idx].USBDeviceProperties.FriendlyName[0] = '\0';
  USBDeviceArr[Idx].USBDeviceProperties.CompilationDate[0] = '\0';

  USBDeviceArr[Idx].USBDevice->Close();
  USBDeviceArr[Idx].USBDevice  = NULL;

  delete USBDeviceArr[Idx].USBDevice; USBDeviceArr[Idx].USBDevice = (CCyUSBDevice *)NULL;
  LastError = RTDAC_OK; return LastError;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Return the number of connected RT-DAC/USB2 devices
//
int USB2NumOfDevices( void ) {
    CCyUSBDevice *USBDevice = NULL;
	int DevCount;
    if( USBDevice == NULL ) USBDevice = new CCyUSBDevice( /*m_hWnd*/ ); // Create an instance of CCyUSBDevice
    DevCount = (int)USBDevice->DeviceCount( );
    delete USBDevice;
	return DevCount;
}


//////////////////////////////////////////////////////////////////////////////////
//
// Return the properties of the connected RT-DAC/USB2 devices
//
int USB2DeviceProperties( int Idx, USBDevicePropertiesType *pUSBDeviceProperties )  {
  unsigned char buf[ 256 ];
  unsigned int CompilationDateAddr, ReleaseDateAddr;
  LONG bytesToRead;
  int i;

  pUSBDeviceProperties->ProductID = pUSBDeviceProperties->VendorID = 0;
  pUSBDeviceProperties->DevClass = pUSBDeviceProperties->DevSubClass = 0;
  pUSBDeviceProperties->DevProtocol = 0;
  pUSBDeviceProperties->BcdDevice = 0;
  pUSBDeviceProperties->Manufacturer[0] = '\0';
  pUSBDeviceProperties->Product[0] = '\0';
  pUSBDeviceProperties->SerialNumber[0] = '\0';
  pUSBDeviceProperties->FriendlyName[0] = '\0';
  pUSBDeviceProperties->CompilationDate[0] = '\0';

  if( USBDeviceArr[Idx].USBDevice == NULL ) {
	LastError = RTDAC_INVALIED_DEVICE_POINTER; return LastError;
  }

  if( (USBDeviceArr[Idx].FPGAOutEpt == NULL) || 
	  (USBDeviceArr[Idx].FPGAInEpt  == NULL) ||
	  (USBDeviceArr[Idx].JTAGOutEpt == NULL) ||
	  (USBDeviceArr[Idx].JTAGInEpt  == NULL) ) {
	LastError = RTDAC_CAN_NOT_ACCESS_NULL_ENDPOINT; return LastError;
  }

  pUSBDeviceProperties->ProductID   = USBDeviceArr[Idx].USBDeviceProperties.ProductID;
  pUSBDeviceProperties->VendorID    = USBDeviceArr[Idx].USBDeviceProperties.VendorID;
  pUSBDeviceProperties->DevClass    = USBDeviceArr[Idx].USBDeviceProperties.DevClass;
  pUSBDeviceProperties->DevSubClass = USBDeviceArr[Idx].USBDeviceProperties.DevSubClass;
  pUSBDeviceProperties->DevProtocol = USBDeviceArr[Idx].USBDeviceProperties.DevProtocol;
  pUSBDeviceProperties->BcdDevice   = USBDeviceArr[Idx].USBDeviceProperties.BcdDevice;
  strncpy( pUSBDeviceProperties->Manufacturer, USBDeviceArr[Idx].USBDeviceProperties.Manufacturer, USB_STRING_MAXLEN );
  strncpy( pUSBDeviceProperties->Product,      USBDeviceArr[Idx].USBDeviceProperties.Product, USB_STRING_MAXLEN );
  strncpy( pUSBDeviceProperties->SerialNumber, USBDeviceArr[Idx].USBDeviceProperties.SerialNumber, USB_STRING_MAXLEN );
  strncpy( pUSBDeviceProperties->FriendlyName, USBDeviceArr[Idx].USBDeviceProperties.FriendlyName, USB_STRING_MAXLEN );

  USBDeviceArr[Idx].ControlEpt->ReqCode = 0xD9;   // Read string positions
  USBDeviceArr[Idx].ControlEpt->Value =   0x0000; // Address - not used
  USBDeviceArr[Idx].ControlEpt->Index =   0x0000; // Not used
  bytesToRead = 6;
  USBDeviceArr[Idx].ControlEpt->Read( buf, bytesToRead );
  CompilationDateAddr  = 256*(unsigned int)buf[3] + buf[2];
  ReleaseDateAddr      = 256*(unsigned int)buf[5] + buf[4];
  
  USBDeviceArr[Idx].ControlEpt->ReqCode = 0xDA;   // Read 32 byte data buffer 
  USBDeviceArr[Idx].ControlEpt->Value =   CompilationDateAddr; // Address - compilation date
  USBDeviceArr[Idx].ControlEpt->Index =   0x0000; // Not used
  bytesToRead = 32;
  USBDeviceArr[Idx].ControlEpt->Read( buf, bytesToRead );
  for(i=0;i<11;i++) pUSBDeviceProperties->CompilationDate[i] = buf[2*i];
  pUSBDeviceProperties->CompilationDate[10] = '\0';

  USBDeviceArr[Idx].ControlEpt->ReqCode = 0xDA;   // Read 32 byte data buffer 
  USBDeviceArr[Idx].ControlEpt->Value =   ReleaseDateAddr; // Address - release date
  USBDeviceArr[Idx].ControlEpt->Index =   0x0000; // Not used
  bytesToRead = 32;
  USBDeviceArr[Idx].ControlEpt->Read( buf, bytesToRead );
  for(i=0;i<11;i++) pUSBDeviceProperties->ReleaseDate[i] = buf[2*i];
  pUSBDeviceProperties->ReleaseDate[10] = '\0';

  LastError = RTDAC_OK; return LastError;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Set the state of the middle LED
//
int USB2MiddleLED( int Idx, int State )  {
  LONG bytesToWrite;
  unsigned char buf[256];
  if( State == 0 )
    USBDeviceArr[Idx].ControlEpt->ReqCode = 0xDE;   // First state
  else
    USBDeviceArr[Idx].ControlEpt->ReqCode = 0xDF;   // Second state
  USBDeviceArr[Idx].ControlEpt->Value =   0x0000;   // Not used
  USBDeviceArr[Idx].ControlEpt->Index =   0x0000;   // Not used
  bytesToWrite = 0;
  USBDeviceArr[Idx].ControlEpt->Write( buf, bytesToWrite );
  LastError = RTDAC_OK; return LastError;
}

//////////////////////////////////////////////////////////////////////////////////
//
// 8/16 buffer convertion functions
//
static void Convert8BitBufferTo16BitBuffer( void ) {
  int i;
  for(i=0; i<BINARY_BUFFER_SIZE; i+=2) 
    BufferToRead[i/2] = ((((unsigned char)UCHARBufferToRead[i+1]) & 0x00FF) << 8) | 
		                 (((unsigned char)UCHARBufferToRead[i])   & 0x00FF);
}

static void Convert16BitBufferTo8BitBuffer( void ) {
  int i;
  for(i=0; i<BINARY_BUFFER_SIZE; i+=2) {
	UCHARBufferToSend[i]   = (BufferToSend[i/2] >> 0) & 0xFF;
	UCHARBufferToSend[i+1] = (BufferToSend[i/2] >> 8) & 0xFF;
  }
}


//////////////////////////////////////////////////////////////////////////////////
//
// Logic dependent functions
//
static void CreateBinaryBufferToSend_TRAS( RTDACUSB2BufferType *RTDACUSBBufferToSend ) {
  int i;

  //BufferToSend[ 12 ] = (RTDACUSBBufferToSend->CN1Direction >>  0) & 0x7FFF;  
  //BufferToSend[ 13 ] = (RTDACUSBBufferToSend->CN1Direction >> 15) & 0x07FF;  

  BufferToSend[ 14 ] = (RTDACUSBBufferToSend->CN1Output >>  0) & 0x7FFF;  
  BufferToSend[ 15 ] = (RTDACUSBBufferToSend->CN1Output >> 15) & 0x07FF;  

  for(i=0;i<2;i++) {
	BufferToSend[20 + 3*i] =  RTDACUSBBufferToSend->PWM[i].Prescaler & 0x7FFF;
    BufferToSend[21 + 3*i] = ((RTDACUSBBufferToSend->PWM[i].Mode & 1) << 1) | 
		                     ((RTDACUSBBufferToSend->PWM[i].Prescaler >> 15) & 1);
    BufferToSend[22 + 3*i] =  RTDACUSBBufferToSend->PWM[i].Width & 0x0FFF;
  }

  BufferToSend[31] = RTDACUSBBufferToSend->PWMFlag & 0x0003;
  
  BufferToSend[50] =  0;
  for(i=0;i<2;i++) {
	BufferToSend[50] += (1 << i) * (RTDACUSBBufferToSend->Encoder[i].Reset & 0x0001);
  }

  BufferToSend[70] = BufferToSend[71] =  0;
  for(i=0;i<1;i++) {
	BufferToSend[70] += (1 << i) * (RTDACUSBBufferToSend->TmrCnt[i].Reset & 0x0001);
	BufferToSend[71] += (1 << i) * (RTDACUSBBufferToSend->TmrCnt[i].Mode & 0x0001);
  }

  BufferToSend[200] = BufferToSend[201] = BufferToSend[202] = BufferToSend[203] = 0;
  BufferToSend[200] += (RTDACUSBBufferToSend->AD[ 0].Gain & 0x0007) <<  0;
  BufferToSend[200] += (RTDACUSBBufferToSend->AD[ 1].Gain & 0x0007) <<  4;
  BufferToSend[200] += (RTDACUSBBufferToSend->AD[ 2].Gain & 0x0007) <<  8;
  BufferToSend[200] += (RTDACUSBBufferToSend->AD[ 3].Gain & 0x0007) << 12;
  BufferToSend[201] += (RTDACUSBBufferToSend->AD[ 4].Gain & 0x0007) <<  0;
  BufferToSend[201] += (RTDACUSBBufferToSend->AD[ 5].Gain & 0x0007) <<  4;
  BufferToSend[201] += (RTDACUSBBufferToSend->AD[ 6].Gain & 0x0007) <<  8;
  BufferToSend[201] += (RTDACUSBBufferToSend->AD[ 7].Gain & 0x0007) << 12;
  BufferToSend[202] += (RTDACUSBBufferToSend->AD[ 8].Gain & 0x0007) <<  0;
  BufferToSend[202] += (RTDACUSBBufferToSend->AD[ 9].Gain & 0x0007) <<  4;
  BufferToSend[202] += (RTDACUSBBufferToSend->AD[10].Gain & 0x0007) <<  8;
  BufferToSend[202] += (RTDACUSBBufferToSend->AD[11].Gain & 0x0007) << 12;
  BufferToSend[203] += (RTDACUSBBufferToSend->AD[12].Gain & 0x0007) <<  0;
  BufferToSend[203] += (RTDACUSBBufferToSend->AD[13].Gain & 0x0007) <<  4;
  BufferToSend[203] += (RTDACUSBBufferToSend->AD[14].Gain & 0x0007) <<  8;
  BufferToSend[203] += (RTDACUSBBufferToSend->AD[15].Gain & 0x0007) << 12;

  for (i=0;i<4;i++)  BufferToSend[220 + i] = RTDACUSBBufferToSend->DA[i] & 0x3FFF;
  
  Convert16BitBufferTo8BitBuffer( );
}

static void UnpackBinaryBuffer_TRAS( RTDACUSB2BufferType *RTDACUSBBufferToRead ) {
  int i;

  Convert8BitBufferTo16BitBuffer();

  RTDACUSBBufferToRead->LogicVersion = BufferToRead[0];  
  RTDACUSBBufferToRead->ApplicationName[6] = '\0';
  RTDACUSBBufferToRead->ApplicationName[0] = (char)( (BufferToRead[3] >> 8) & 0xFF );
  RTDACUSBBufferToRead->ApplicationName[1] = (char)( (BufferToRead[3] >> 0) & 0xFF );
  RTDACUSBBufferToRead->ApplicationName[2] = (char)( (BufferToRead[2] >> 8) & 0xFF );
  RTDACUSBBufferToRead->ApplicationName[3] = (char)( (BufferToRead[2] >> 0) & 0xFF );
  RTDACUSBBufferToRead->ApplicationName[4] = (char)( (BufferToRead[1] >> 8) & 0xFF );
  RTDACUSBBufferToRead->ApplicationName[5] = (char)( (BufferToRead[1] >> 0) & 0xFF );
  RTDACUSBBufferToRead->LogicDate =   (unsigned long)(BufferToRead[4]) |
								    (((unsigned long)(BufferToRead[5])) << 16);

  RTDACUSBBufferToRead->NoOfChannels[ 0] = (((unsigned int)(BufferToRead[6])) >>  0) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 1] = (((unsigned int)(BufferToRead[6])) >>  4) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 2] = (((unsigned int)(BufferToRead[6])) >>  8) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 3] = (((unsigned int)(BufferToRead[6])) >> 12) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 4] = (((unsigned int)(BufferToRead[7])) >>  0) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 5] = (((unsigned int)(BufferToRead[7])) >>  4) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 6] = (((unsigned int)(BufferToRead[7])) >>  8) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 7] = (((unsigned int)(BufferToRead[7])) >> 12) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 8] = (((unsigned int)(BufferToRead[8])) >>  0) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[ 9] = (((unsigned int)(BufferToRead[8])) >>  4) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[10] = (((unsigned int)(BufferToRead[8])) >>  8) & 0x000F;
  RTDACUSBBufferToRead->NoOfChannels[11] = (((unsigned int)(BufferToRead[8])) >> 12) & 0x000F;

  //RTDACUSBBufferToRead->CN1PinMode   =   (unsigned long)(BufferToRead[10]) |
  //								       (((unsigned long)(BufferToRead[11] & 0x03FF)) << 16);
  RTDACUSBBufferToRead->CN1Direction =   (unsigned long)(BufferToRead[12]) |
								       (((unsigned long)(BufferToRead[13] & 0x03FF)) << 16);
  RTDACUSBBufferToRead->CN1Output    =   (unsigned long)(BufferToRead[14]) |
								       (((unsigned long)(BufferToRead[15] & 0x03FF)) << 16);
  RTDACUSBBufferToRead->CN1Input     =   (unsigned long)(BufferToRead[16]) |
								       (((unsigned long)(BufferToRead[17] & 0x03FF)) << 16);

  for(i=0;i<2;i++) {
	RTDACUSBBufferToRead->PWM[i].Prescaler = (unsigned int)  BufferToRead[20 + 3*i];
    RTDACUSBBufferToRead->PWM[i].Mode      = (unsigned int)( BufferToRead[21 + 3*i] & 0x0001);
    RTDACUSBBufferToRead->PWM[i].Width     = (unsigned int)( BufferToRead[22 + 3*i] & 0x0FFF);
  }

  RTDACUSBBufferToRead->PWMFlag = (unsigned int)( BufferToRead[31] & 0x0003);
  
  for(i=0;i<2;i++) {
	RTDACUSBBufferToRead->Encoder[i].Reset     = (unsigned int)  ((BufferToRead[50] >> i) & 0x0001);
    RTDACUSBBufferToRead->Encoder[i].Counter   = (unsigned long) ( BufferToRead[53 + 2*i]) |
								                 (unsigned long)(( BufferToRead[54 + 2*i]) << 16);
  }

  for(i=0;i<1;i++) {
	RTDACUSBBufferToRead->TmrCnt[i].Reset   = (unsigned int)  ((BufferToRead[70] >> i) & 0x0001);
    RTDACUSBBufferToRead->TmrCnt[i].Mode    = (unsigned int)  ((BufferToRead[71] >> i) & 0x0001);
    RTDACUSBBufferToRead->TmrCnt[i].Counter = (unsigned long) ( BufferToRead[72 + 2*i]) |
								              (unsigned long)(( BufferToRead[73 + 2*i]) << 16);
  }

  RTDACUSBBufferToRead->AD[ 0].Gain = (BufferToRead[200] >>  0) & 0x0007;
  RTDACUSBBufferToRead->AD[ 1].Gain = (BufferToRead[200] >>  4) & 0x0007;
  RTDACUSBBufferToRead->AD[ 2].Gain = (BufferToRead[200] >>  8) & 0x0007;
  RTDACUSBBufferToRead->AD[ 3].Gain = (BufferToRead[200] >> 12) & 0x0007;
  RTDACUSBBufferToRead->AD[ 4].Gain = (BufferToRead[201] >>  0) & 0x0007;
  RTDACUSBBufferToRead->AD[ 5].Gain = (BufferToRead[201] >>  4) & 0x0007;
  RTDACUSBBufferToRead->AD[ 6].Gain = (BufferToRead[201] >>  8) & 0x0007;
  RTDACUSBBufferToRead->AD[ 7].Gain = (BufferToRead[201] >> 12) & 0x0007;
  RTDACUSBBufferToRead->AD[ 8].Gain = (BufferToRead[202] >>  0) & 0x0007;
  RTDACUSBBufferToRead->AD[ 9].Gain = (BufferToRead[202] >>  4) & 0x0007;
  RTDACUSBBufferToRead->AD[10].Gain = (BufferToRead[202] >>  8) & 0x0007;
  RTDACUSBBufferToRead->AD[11].Gain = (BufferToRead[202] >> 12) & 0x0007;
  RTDACUSBBufferToRead->AD[12].Gain = (BufferToRead[203] >>  0) & 0x0007;
  RTDACUSBBufferToRead->AD[13].Gain = (BufferToRead[203] >>  4) & 0x0007;
  RTDACUSBBufferToRead->AD[14].Gain = (BufferToRead[203] >>  8) & 0x0007;
  RTDACUSBBufferToRead->AD[15].Gain = (BufferToRead[203] >> 12) & 0x0007;

  for (i=0;i<16;i++) {
    RTDACUSBBufferToRead->AD[i].Result = BufferToRead[204 + i] & 0x0FFF;
    RTDACUSBBufferToRead->AD[i].Busy   = (BufferToRead[204 + i] >> 12) & 0x0001;
  }
//ADStart = (int)((UInt32)pUIntBuffer[215] & 0x0001);
//ADReady = (int)(((UInt32)pUIntBuffer[215] >> 1) & 0x0001);
           
  for (i=0;i<4;i++)  RTDACUSBBufferToRead->DA[i] = BufferToRead[220 + i] & 0x3FFF;

}

int CommandSendBinaryBuffer_TRAS( int Idx ) {
  LONG length = 504; //258;
  UCHARBufferToSend[0] = (unsigned char)((COMMAND_SEND >> 0) & 0xFF);
  UCHARBufferToSend[1] = (unsigned char)((COMMAND_SEND >> 8) & 0xFF);

#ifdef ASYNCHRONOUS_TRANSFER
  USBDeviceArr[Idx].outContext = USBDeviceArr[Idx].FPGAOutEpt->BeginDataXfer(UCHARBufferToSend, length, &USBDeviceArr[Idx].outOvLap);
  USBDeviceArr[Idx].FPGAOutEpt->WaitForXfer(&USBDeviceArr[Idx].outOvLap,100);
  USBDeviceArr[Idx].FPGAOutEpt->FinishDataXfer(UCHARBufferToSend, length, &USBDeviceArr[Idx].outOvLap,USBDeviceArr[Idx].outContext);
#else
  if( !USBDeviceArr[Idx].FPGAOutEpt->XferData(UCHARBufferToSend, length) ) {
    LastError = RTDAC_INVALIED_SYNCHRONOUS_OUT_TRANSFER; return LastError;
  }
#endif

  LastError = RTDAC_OK; return LastError;
  // !!!!!
}

int CommandSend_TRAS( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToSend ) {
  LONG length = 504; //258;
  CreateBinaryBufferToSend_TRAS( RTDACUSBBufferToSend );
  UCHARBufferToSend[0] = (unsigned char)((COMMAND_SEND >> 0) & 0xFF);
  UCHARBufferToSend[1] = (unsigned char)((COMMAND_SEND >> 8) & 0xFF);

#ifdef ASYNCHRONOUS_TRANSFER
  USBDeviceArr[Idx].outContext = USBDeviceArr[Idx].FPGAOutEpt->BeginDataXfer(UCHARBufferToSend, length, &USBDeviceArr[Idx].outOvLap);
  USBDeviceArr[Idx].FPGAOutEpt->WaitForXfer(&USBDeviceArr[Idx].outOvLap,100);
  USBDeviceArr[Idx].FPGAOutEpt->FinishDataXfer(UCHARBufferToSend, length, &USBDeviceArr[Idx].outOvLap,USBDeviceArr[Idx].outContext);
#else
  if( !USBDeviceArr[Idx].FPGAOutEpt->XferData(UCHARBufferToSend, length) ) {
    LastError = RTDAC_INVALIED_SYNCHRONOUS_OUT_TRANSFER; return LastError;
  }
#endif

  LastError = RTDAC_OK; return LastError;
  // !!!!!
}

int CommandRead_TRAS( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToRead ) {
  LONG length = 4;
  LONG inLength = 504; //256;

  unsigned char AuxBuffer[ 5 ];
  AuxBuffer[0] = (COMMAND_READ >> 0) & 0xFF;
  AuxBuffer[1] = (COMMAND_READ >> 8) & 0xFF;
  AuxBuffer[2] = AuxBuffer[3] = 0;  // Send min 4 bytes !
#ifdef ASYNCHRONOUS_TRANSFER
  // Just to be cute, request the return data before initiating the loopback
  USBDeviceArr[Idx].inContext  = USBDeviceArr[Idx].FPGAInEpt->BeginDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap);
  USBDeviceArr[Idx].outContext = USBDeviceArr[Idx].FPGAOutEpt->BeginDataXfer(AuxBuffer, length, &USBDeviceArr[Idx].outOvLap);

  USBDeviceArr[Idx].FPGAOutEpt->WaitForXfer(&USBDeviceArr[Idx].outOvLap,100);
  USBDeviceArr[Idx].FPGAInEpt->WaitForXfer(&USBDeviceArr[Idx].inOvLap,100);

  USBDeviceArr[Idx].FPGAOutEpt->FinishDataXfer(AuxBuffer, length, &USBDeviceArr[Idx].outOvLap,USBDeviceArr[Idx].outContext);
  USBDeviceArr[Idx].FPGAInEpt->FinishDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap,USBDeviceArr[Idx].inContext);
#else
//Sleep(1); // Tu OK
  if( !USBDeviceArr[Idx].FPGAOutEpt->XferData(AuxBuffer, length) ) {
    LastError = RTDAC_INVALIED_SYNCHRONOUS_OUT_TRANSFER; return LastError;
  }
//Sleep(1); // A tu DD
  if( !USBDeviceArr[Idx].FPGAInEpt->XferData(UCHARBufferToRead, inLength) ) {
    LastError = RTDAC_INVALIED_SYNCHRONOUS_IN_TRANSFER; return LastError;
  }
#endif

  UnpackBinaryBuffer_TRAS( RTDACUSBBufferToRead );

  LastError = RTDAC_OK; return LastError;
  // !!!!!
}


//////////////////////////////////////////////////////////////////////////////////
//
// JTAG endpoint transfer functions
//
int JTAGSend( int Idx, unsigned char value ) {
  LONG length = 1;
  UCHARBufferToSend[0] = value;
  UCHARBufferToSend[1] = 0;

#ifdef ASYNCHRONOUS_TRANSFER
  USBDeviceArr[Idx].outContext = USBDeviceArr[Idx].JTAGOutEpt->BeginDataXfer(UCHARBufferToSend, length, &USBDeviceArr[Idx].outOvLap);
  USBDeviceArr[Idx].JTAGOutEpt->WaitForXfer(&USBDeviceArr[Idx].outOvLap,100);
  USBDeviceArr[Idx].JTAGOutEpt->FinishDataXfer(UCHARBufferToSend, length, &USBDeviceArr[Idx].outOvLap,USBDeviceArr[Idx].outContext);
#else
  if( !USBDeviceArr[Idx].JTAGOutEpt->XferData(UCHARBufferToSend, length) ) {
    LastError = RTDAC_INVALIED_SYNCHR_JTAG_OUT_TRANSFER; return LastError;
  }
#endif

  LastError = RTDAC_OK; return LastError;
  // !!!!!
}

int JTAGRead( int Idx ) {
  LONG inLength = 1;

#ifdef ASYNCHRONOUS_TRANSFER
  // Just to be cute, request the return data before initiating the loopback
  USBDeviceArr[Idx].inContext  = USBDeviceArr[Idx].JTAGInEpt->BeginDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap);
  USBDeviceArr[Idx].JTAGInEpt->WaitForXfer(&USBDeviceArr[Idx].inOvLap,100);
  USBDeviceArr[Idx].JTAGInEpt->FinishDataXfer(UCHARBufferToRead, inLength, &USBDeviceArr[Idx].inOvLap,USBDeviceArr[Idx].inContext);
#else
  if( !USBDeviceArr[Idx].JTAGInEpt->XferData(UCHARBufferToRead, inLength) ) {
    LastError = RTDAC_INVALIED_SYNCHR_JTAG_IN_TRANSFER; return LastError;
  }
#endif

  return (int) (UCHARBufferToRead[0]);  // !!!!!!
}

int JTAGSendFrame( int Idx, UCHAR Buffer[], LONG length ) {
#ifdef ASYNCHRONOUS_TRANSFER
  USBDeviceArr[Idx].outContext = USBDeviceArr[Idx].JTAGOutEpt->BeginDataXfer(Buffer, length, &USBDeviceArr[Idx].outOvLap);
  USBDeviceArr[Idx].JTAGOutEpt->WaitForXfer(&USBDeviceArr[Idx].outOvLap,100);
  USBDeviceArr[Idx].JTAGOutEpt->FinishDataXfer(UCHARBufferToSend, length, &USBDeviceArr[Idx].outOvLap,USBDeviceArr[Idx].outContext);
#else
  if( !USBDeviceArr[Idx].JTAGOutEpt->XferData(Buffer, length) ) {
    LastError = RTDAC_INVALIED_SYNCHR_JTAG_OUT_TRANSFER; return LastError;
  }
#endif

  LastError = RTDAC_OK; return LastError;
  // !!!!!
}

int JTAGReadFrame( int Idx, UCHAR Buffer[], LONG inLength ) {
#ifdef ASYNCHRONOUS_TRANSFER
  // Just to be cute, request the return data before initiating the loopback
  USBDeviceArr[Idx].inContext  = USBDeviceArr[Idx].JTAGInEpt->BeginDataXfer(Buffer, inLength, &USBDeviceArr[Idx].inOvLap);
  USBDeviceArr[Idx].JTAGInEpt->WaitForXfer(&USBDeviceArr[Idx].inOvLap,100);
  USBDeviceArr[Idx].JTAGInEpt->FinishDataXfer(Buffer, inLength, &USBDeviceArr[Idx].inOvLap,USBDeviceArr[Idx].inContext);
#else
  if( !USBDeviceArr[Idx].JTAGInEpt->XferData(Buffer, inLength) ) {
    LastError = RTDAC_INVALIED_SYNCHR_JTAG_IN_TRANSFER; return LastError;
  }
#endif

  LastError = RTDAC_OK; return LastError;
  // !!!!!
}


///////////////////////////////////////////////////////////
//
// Error  functions
//
int USB2LastError( void ) {
  return LastError;
}

char *USB2LastErrorMsg( void ) {
  static char ErrorMsg[ 256 ];
  switch( LastError ) {
	case RTDAC_OK:                                strcpy( ErrorMsg, "RTDAC_OK" );                                break;
	case RTDAC_TOO_MANY_USB_DEVICES:              strcpy( ErrorMsg, "RTDAC_TOO_MANY_USB_DEVICES" );              break;
	case RTDAC_CAN_NOT_FIND_USB_DEVICE:           strcpy( ErrorMsg, "RTDAC_CAN_NOT_FIND_USB_DEVICE" );           break;
	case RTDAC_TOO_HIGH_BOARD_INDEX:              strcpy( ErrorMsg, "RTDAC_TOO_HIGH_BOARD_INDEX" );              break;
	case RTDAC_CAN_NOT_ACCESS_ENDPOINTS:          strcpy( ErrorMsg, "RTDAC_CAN_NOT_ACCESS_ENDPOINTS" );          break;
	case RTDAC_CAN_NOT_ACCESS_NULL_ENDPOINT:      strcpy( ErrorMsg, "RTDAC_CAN_NOT_CLOSE_NULL_ENDPOINT" );       break;
	case RTDAC_ENDPOINTS_NOT_CLOSED:              strcpy( ErrorMsg, "RTDAC_ENDPOINTS_NOT_CLOSED" );              break;
	case RTDAC_INVALIED_DEVICE_POINTER:           strcpy( ErrorMsg, "RTDAC_INVALIED_DEVICE_POINTER" );           break;
	case RTDAC_INVALIED_SYNCHRONOUS_OUT_TRANSFER: strcpy( ErrorMsg, "RTDAC_INVALIED_SYNCHRONOUS_OUT_TRANSFER" ); break;
	case RTDAC_INVALIED_SYNCHRONOUS_IN_TRANSFER:  strcpy( ErrorMsg, "RTDAC_INVALIED_SYNCHRONOUS_IN_TRANSFER" );  break;
	case RTDAC_INVALIED_SYNCHR_JTAG_OUT_TRANSFER: strcpy( ErrorMsg, "RTDAC_INVALIED_SYNCHR_JTAG_OUT_TRANSFER" ); break;
	case RTDAC_INVALIED_SYNCHR_JTAG_IN_TRANSFER:  strcpy( ErrorMsg, "RTDAC_INVALIED_SYNCHR_JTAG_IN_TRANSFER" );  break;
	case RTDAC_CAN_NOT_FIND_SERIAL_NUMBER:        strcpy( ErrorMsg, "RTDAC_CAN_NOT_FIND_SERIAL_NUMBER" );        break;

	default: strcpy( ErrorMsg, "RTDAC_UNKNOWN_ERROR" ); 
  }
  return ErrorMsg;
}

