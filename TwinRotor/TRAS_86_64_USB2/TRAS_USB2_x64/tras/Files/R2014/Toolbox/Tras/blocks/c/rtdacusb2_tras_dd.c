/*
 * RTDACUSB_TRAS_DD.C
 *
 *   S-Function for the RT-DAC/USB2 device - TRAS interface
 *   All I/O functions as a single device block
 *
 *  Copyright (c) 2011 by 2K/InTeCo
 *  All Rights Reserved
 */

 ////////////////////////////////////
 //
 // Inputs:               / size / comment
 //      AzimuthCtrl      /    1
 //      PitchCtrl        /    1 
 //      EncoderReset     /    2 
 //      PWMPrescaler     /    1 / For both PWM outputs
 //      AzimuthCtrlTerm  /    1
 //      PitchCtrlTerm    /    1 
 //
 // Outputs:              / size / comment
 //      Status           /    1 / error code
 //      AzimuthAngle     /    1 
 //      PitchAngle       /    1
 //      AzimuthRPM       /    1
 //      PitchRPM         /    1
 //      TmrCounter       /    1
 //      ThermStatus      /    2

#define S_FUNCTION_NAME rtdacusb2_tras_dd
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#include "windows.h"

#include "RtdacUSB2_TRAS.H"

// Configuration of the RT-DAC/USB device
static RTDACUSB2BufferType RTDACUSBBuffer; 


/* Input Arguments */
#define BOARD_ID_ARG     (ssGetSFcnParam(S,0))
#define SAMPLE_TIME_ARG  (ssGetSFcnParam(S,1))
#define NUMBER_OF_ARGS   (2)
#define NSAMPLE_TIMES    (1)

#define BOARD_ID         ((real_T) mxGetPr(BOARD_ID_ARG)[0])
#define SAMPLE_TIME      ((real_T) mxGetPr(SAMPLE_TIME_ARG)[0])

static HINSTANCE DLLHLib = 0;
#ifdef __WATCOMC__
typedef int __declspec(dllimport) __cdecl (*lpRTDACUSB2OpenType)( void );
typedef int __declspec(dllimport) __cdecl (*lpRTDACUSB2CloseType)( int Idx );
typedef int __declspec(dllimport) __cdecl (*lpRTDACUSB2NumOfDevicesType)( void );
typedef int __declspec(dllimport) __cdecl (*lpRTDACCommandSend_TRASType)( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToSend );
typedef int __declspec(dllimport) __cdecl (*lpRTDACCommandRead_TRASType)( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToSend );
#else
typedef int (*lpRTDACUSB2OpenType)( void );
typedef int (*lpRTDACUSB2CloseType)( int Idx );
typedef int (*lpRTDACUSB2NumOfDevicesType)( void );
typedef int (*lpRTDACCommandSend_TRASType)( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToSend );
typedef int (*lpRTDACCommandRead_TRASType)( int Idx, RTDACUSB2BufferType *RTDACUSBBufferToSend );
#endif  /* __WATCOMC__ */



static lpRTDACUSB2OpenType         lpRTDACUSB2Open;
static lpRTDACUSB2CloseType        lpRTDACUSB2Close;
static lpRTDACUSB2NumOfDevicesType lpRTDACUSB2NumOfDevices;
static lpRTDACCommandSend_TRASType lpRTDACCommandSend_TRAS;
static lpRTDACCommandRead_TRASType lpRTDACCommandRead_TRAS;

static int Status = 0;
static int BoardIdx = -1;


static void mdlInitializeSizes(SimStruct *S) {
    ssSetNumSFcnParams(S, NUMBER_OF_ARGS);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch will be reported by Simulink */
    }

    if (!ssSetNumInputPorts(S, 6)) return;
    ssSetInputPortWidth(S,  0, 1);               // AzimuthCtrl
    ssSetInputPortDirectFeedThrough(S,  0, 0);
    ssSetInputPortWidth(S,  1, 1);               // PitchCtrl
    ssSetInputPortDirectFeedThrough(S,  1, 0);
    ssSetInputPortWidth(S,  2, 2);               // EncoderReset
    ssSetInputPortDirectFeedThrough(S,  2, 0);
    ssSetInputPortWidth(S,  3, 1);               // PWMPrescaler
    ssSetInputPortDirectFeedThrough(S,  3, 0);
    ssSetInputPortWidth(S,  4, 1);               // AzimuthCtrlTerm
    ssSetInputPortDirectFeedThrough(S,  4, 0);
    ssSetInputPortWidth(S,  5, 1);               // PitchCtrlTerm
    ssSetInputPortDirectFeedThrough(S,  5, 0);

    if (!ssSetNumOutputPorts(S, 7)) return;
    ssSetOutputPortWidth(S,  0,  1 );            // Status
    ssSetOutputPortWidth(S,  1,  1 );            // AzimuthAngle
    ssSetOutputPortWidth(S,  2,  1 );            // PitchAngle
    ssSetOutputPortWidth(S,  3,  1 );            // AzimuthRPM
    ssSetOutputPortWidth(S,  4,  1 );            // PitchRPM
    ssSetOutputPortWidth(S,  5,  1 );            // TmrCounter
    ssSetOutputPortWidth(S,  6,  2 );            // ThermStatus

    ssSetNumSampleTimes(S, NSAMPLE_TIMES );

    /* Take care when specifying exception free code */
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
    
}
 
 
/* Function to initialize sample times */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0);
}
 
#define MDL_START
static void mdlStart(SimStruct *S) {
#ifndef MATLAB_MEX_FILE  
  int NumOfDevices = 0;
  S = S;
  DLLHLib = LoadLibrary( "TRAS_RTDACUSB2_x64.dll" );
  if( DLLHLib == NULL ) {
    DLLHLib = 0;
    Status = -1;
    return;
  }   
  lpRTDACUSB2Open = (lpRTDACUSB2OpenType) GetProcAddress( DLLHLib, "RTDACUSB2Open" );
  if( lpRTDACUSB2Open == NULL ) {
    Status = -2;
    return;
  }
  lpRTDACUSB2Close = (lpRTDACUSB2CloseType) GetProcAddress( DLLHLib, "RTDACUSB2Close" );
  if( lpRTDACUSB2Close == NULL ) {
    Status = -3;
    return;
  }
  lpRTDACUSB2NumOfDevices = (lpRTDACUSB2NumOfDevicesType) GetProcAddress( DLLHLib, "RTDACUSB2NumOfDevices" );
  if( lpRTDACUSB2NumOfDevices == NULL ) {
    Status = -4;
    return;
  }
  lpRTDACCommandSend_TRAS = (lpRTDACCommandSend_TRASType) GetProcAddress( DLLHLib, "RTDACCommandSend_TRAS" );
  if( lpRTDACCommandSend_TRAS == NULL ) {
    Status = -5;
    return;
  }
  lpRTDACCommandRead_TRAS = (lpRTDACCommandRead_TRASType) GetProcAddress( DLLHLib, "RTDACCommandRead_TRAS" );
  if( lpRTDACCommandRead_TRAS == NULL ) {
    Status = -6;
    return;
  }
  BoardIdx = (*lpRTDACUSB2Open)();
  if( BoardIdx < 0 ) {
    Status = -7;
    return;
  }
  NumOfDevices = (*lpRTDACUSB2NumOfDevices)();
  if( NumOfDevices < 1 ) {
    Status = -8;
    return;
  }
  if( (*lpRTDACCommandRead_TRAS)( BoardIdx, &RTDACUSBBuffer ) < 0 ) {
    Status = -9;
    return;
  }
#endif  
}

/* Function to compute outputs */
static void mdlOutputs(SimStruct *S, int_T tid)
{
#ifndef MATLAB_MEX_FILE
    int i;
    double dAux;
    int RetVal;
    InputRealPtrsType uPtrsAzimuthCtrl          = ssGetInputPortRealSignalPtrs(S,  0);
    InputRealPtrsType uPitchCtrl                = ssGetInputPortRealSignalPtrs(S,  1);
    InputRealPtrsType uPtrsEncoderReset         = ssGetInputPortRealSignalPtrs(S,  2);
    InputRealPtrsType uPtrsPWMPrescaler         = ssGetInputPortRealSignalPtrs(S,  3);
    
    real_T *yStatus         = ssGetOutputPortRealSignal(S,  0);
    real_T *yAzimuthAngle   = ssGetOutputPortRealSignal(S,  1);
    real_T *yPitchAngle     = ssGetOutputPortRealSignal(S,  2);
    real_T *yAzimuthRPM     = ssGetOutputPortRealSignal(S,  3);
    real_T *yPitchRPM       = ssGetOutputPortRealSignal(S,  4);
    real_T *yTmrCounter     = ssGetOutputPortRealSignal(S,  5);
    real_T *yThermStatus    = ssGetOutputPortRealSignal(S,  6);
   
    tid = tid;
    yStatus[0] = Status;
    if( Status <0 ) {
        return;
    }
    
    // Input signals
    RTDACUSBBuffer.CN1Output = 0;
    for(i=0;i<2;i++) {
    //  RTDACUSBBuffer.PWM[i].Mode      = 1; // 12-bit
      RTDACUSBBuffer.PWM[i].Mode      = 0; // 8-bit
      RTDACUSBBuffer.PWM[i].Prescaler = (int)(*uPtrsPWMPrescaler[0]); 
      dAux = 0.0;
      if( i == 0 ) {
        dAux = *uPtrsAzimuthCtrl[0];
        if( dAux > 0.0 ) RTDACUSBBuffer.CN1Output |= 0x2000000;
      }
      if( i == 1 ) {
        dAux = *uPitchCtrl[0];
        if( dAux > 0.0 ) RTDACUSBBuffer.CN1Output |= 0x0200000;
      }
      if( dAux > +1.0 ) dAux = +1.0;
      if( dAux < -1.0 ) dAux = -1.0;
    //  RTDACUSBBuffer.PWM[i].Width     = (int)(fabs(dAux)*4095); //12-bit
      RTDACUSBBuffer.PWM[i].Width     = (int)(fabs(dAux)*255);  // 8-bit
    }
    RTDACUSBBuffer.PWMFlag = 0x03;  // Turm on PWM therm flags
    
    for(i=0;i<2;i++) RTDACUSBBuffer.Encoder[i].Reset = (int)(*uPtrsEncoderReset[i]); 

    RTDACUSBBuffer.TmrCnt[0].Reset = 0; 
    RTDACUSBBuffer.TmrCnt[0].Mode  = 1; // Timer

    for(i=0;i<16;i++) RTDACUSBBuffer.AD[i].Gain = 0;   // Gain 1

    RetVal = (*lpRTDACCommandSend_TRAS)( BoardIdx, &RTDACUSBBuffer );
    if( RetVal <0 ) {
        yStatus[0] = -100 + RetVal;
        return;
    }
    
    RetVal = (*lpRTDACCommandRead_TRAS)( BoardIdx, &RTDACUSBBuffer );
    if( RetVal <0 ) {
        yStatus[0] = -200 + RetVal;
        return;
    }

    // Output Signals
    yStatus[0] = RetVal;
    yAzimuthAngle[0] = RTDACUSBBuffer.Encoder[0].Counter;
    yPitchAngle[0]    = RTDACUSBBuffer.Encoder[1].Counter;

    yTmrCounter[0] = RTDACUSBBuffer.TmrCnt[0].Counter; 
    
    yThermStatus[0] = RTDACUSBBuffer.PWMStatus;

    yPitchRPM[0] = RTDACUSBBuffer.AD[0].Result & 0x0FFF;
    if( yPitchRPM[0] >= 2048 ) yPitchRPM[0] -= 4096;
    yPitchRPM[0] = 10.0*(yPitchRPM[0]/2048.0/2.0);
    yAzimuthRPM[0] = RTDACUSBBuffer.AD[1].Result & 0x0FFF;
    if( yAzimuthRPM[0] >= 2048 ) yAzimuthRPM[0] -= 4096;
    yAzimuthRPM[0] = 10.0*(yAzimuthRPM[0]/2048.0/2.0);
#endif    
}



/* Function to perform cleanup at execution termination */
static void mdlTerminate(SimStruct *S)
{
#ifndef MATLAB_MEX_FILE
    int i;
    double dAux;
    
    InputRealPtrsType uPtrsAzimuthCtrlTerm = ssGetInputPortRealSignalPtrs(S,  4);
    InputRealPtrsType uPtrsPitchCtrlTerm   = ssGetInputPortRealSignalPtrs(S,  5);

    if( Status   <  0 ) return;
    if( BoardIdx <  0 ) return;

    (*lpRTDACCommandRead_TRAS)( BoardIdx, &RTDACUSBBuffer );

    RTDACUSBBuffer.CN1Output = 0;
    for(i=0;i<2;i++) {
      dAux = 0.0;
      if( i == 0 ) {
        dAux = *uPtrsAzimuthCtrlTerm[0];
        if( dAux > 0.0 ) RTDACUSBBuffer.CN1Output |= 0x2000000;
      }
      if( i == 1 ) {
        dAux = *uPtrsPitchCtrlTerm[0];
        if( dAux > 0.0 ) RTDACUSBBuffer.CN1Output |= 0x0200000;
      }
      if( dAux > +1.0 ) dAux = +1.0;
      if( dAux < -1.0 ) dAux = -1.0;
      RTDACUSBBuffer.PWM[i].Width     = (int)(fabs(dAux)*4095); 
    }

    (*lpRTDACCommandSend_TRAS)( BoardIdx, &RTDACUSBBuffer );
    i = (*lpRTDACUSB2Close)( BoardIdx );

    FreeLibrary( DLLHLib );
    
#endif
}

#ifdef MATLAB_MEX_FILE  /* Is this file being compiled as a MEX-file? */
#include "simulink.c"        /* MEX-File interface mechanism */
#else
#include "cg_sfun.h"    /* Code generation registration function */
#endif
