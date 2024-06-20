/*
 * PC_SPKR.C
 *
 *   S-Function output device driver for the IBM-PC speaker.
 *
 *  Copyright (c) 1993-2001 by 2K/InTeCo
 *  All Rights Reserved
 */


#define S_FUNCTION_NAME pc_spkr
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <conio.h>
#include <math.h>


#ifdef __WATCOMC__
  static int WriteByte( int port, int value ) { 
    return( outp( port, value ) );
  }
  static int ReadByte( int port ) { 
    return( inp( port ) );
  }
#endif  /* __WATCOMC__ */

#ifdef _MSC_VER
  static int WriteByte( int port, int value ) { 
    return( outp( (unsigned short)port, value ) );
  }
  static unsigned short WriteWord( int port, unsigned int value ) { 
    return( outpw( (unsigned short)port, (unsigned short)value ) );
  }
  static int ReadByte( int port ) { 
    return( inp( (unsigned short)port ) );
  }
  static unsigned int ReadWord( int port ) { 
    return( inpw( (unsigned short)port ) );
  }
#endif  /* _MSC_VER */


/* Input Arguments */
#define TYPE_ARG         (ssGetSFcnParam(S,0))
#define CONT_ARG         (ssGetSFcnParam(S,1))
#define IMP_ARG          (ssGetSFcnParam(S,2))
#define SAMPLE_TIME_ARG  (ssGetSFcnParam(S,3))
#define NUMBER_OF_ARGS   (4)
#define NSAMPLE_TIMES    (1)

#define TYPE             ((real_T) mxGetPr(TYPE_ARG)[0])
#define CONT_PTR         ((real_T *) mxGetPr(CONT_ARG))
#define IMP_PTR          ((real_T *) mxGetPr(IMP_ARG))
#define SAMPLE_TIME      ((real_T) mxGetPr(SAMPLE_TIME_ARG)[0])


#ifndef MATLAB_MEX_FILE
static void StartBeep( double freq )
{
  unsigned char mode = 0xBE, lo, hi;
  unsigned      div;
  unsigned char port;
  double         aux;

  if( freq < 20.0 ) freq = 20.0;
  aux = 1000.0 * 1193.0 / freq;
  div = (unsigned) aux;
  lo = div & 0xFF;   hi = div >> 8;
  WriteByte( 0x43, mode );
  WriteByte( 0x42, lo );
  WriteByte( 0x42, hi );
  port = ReadByte( 0x61 );
  WriteByte( 0x61, port | 0x03 );
}

static void StopBeep( void )
{
  unsigned char port;
  port = ReadByte( 0x61 );
  WriteByte( 0x61, port & 0xFD );
}
#endif

static void mdlInitializeSizes(SimStruct *S)

{
   #ifdef MATLAB_MEX_FILE
    mexEvalString( "startio;" );
   #endif
    ssSetNumSFcnParams(S, NUMBER_OF_ARGS);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch will be reported by Simulink */
    }

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 0, 0);

    if (!ssSetNumOutputPorts(S, 0)) return;

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
 

/* Function to compute outputs */
static void mdlOutputs(SimStruct *S, int_T tid)
{
#ifndef MATLAB_MEX_FILE
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);

    static int aux      = 0;
    double conv;

    int    type         = (int)TYPE;
    double min_inp      = CONT_PTR[ 0 ];
    double max_inp      = CONT_PTR[ 1 ];
    int    min_freq     = (int)CONT_PTR[ 2 ];
    int    max_freq     = (int)CONT_PTR[ 3 ];
    int    downsampling = (int)IMP_PTR[ 0 ];
    double imp_freq     = IMP_PTR[ 1 ];
  
    switch( type )
    {
        case 1:
          conv = (max_freq-min_freq)*( ((*uPtrs[0])-min_inp) / (max_inp-min_inp) );
          conv = conv + min_freq;
          StartBeep( conv );
          break;
        case 2:
          if( aux < 1 )   StartBeep( imp_freq );
          else            StopBeep( );
          aux++;
          if( aux > downsampling ) aux = 0;
          break;
        default:
          StopBeep( );
          break;
    }
#endif    
}


/* Function to perform cleanup at execution termination */
static void mdlTerminate(SimStruct *S)
{
#ifndef MATLAB_MEX_FILE
    StopBeep( );
#endif
}

#ifdef MATLAB_MEX_FILE  /* Is this file being compiled as a MEX-file? */
#include "simulink.c"        /* MEX-File interface mechanism */
#else
#include "cg_sfun.h"    /* Code generation registration function */
#endif
