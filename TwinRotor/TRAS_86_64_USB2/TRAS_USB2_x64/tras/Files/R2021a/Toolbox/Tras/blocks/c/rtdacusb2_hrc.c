 /*
 * HighResolutionCounter.c
 *
 *   S-Function output device driver for the RT-DAC PCI board.
 *   Impulse counter
 *
 *  Copyright (c) 2001 by INTECO/2K
 *  All Rights Reserved
 */

 
#define S_FUNCTION_NAME rtdacusb2_hrc
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <conio.h>
#include <math.h>
#include <windows.h>
#include <winbase.h>
#ifdef MATLAB_MEX_FILE
#include <mex.h>
#endif

/* Input Arguments */
#define SAMPLE_TIME_ARG  (ssGetSFcnParam(S,0))
#define NUMBER_OF_ARGS   (1)
#define NSAMPLE_TIMES    (1)

#define SAMPLE_TIME      ((real_T) mxGetPr(SAMPLE_TIME_ARG)[0])


static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, NUMBER_OF_ARGS);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch will be reported by Simulink */
    }

    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 2);

    if (!ssSetNumInputPorts(S, 0)) return;
    //ssSetInputPortWidth(S, 0, 1);
    //ssSetInputPortDirectFeedThrough(S, 0, 1);

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
static void mdlOutputs(SimStruct *S, int_T tid) {
    real_T *y = ssGetOutputPortRealSignal(S,0);

	LARGE_INTEGER Frequency, Cnt;
    tid = tid;
	if( QueryPerformanceFrequency( &Frequency ) ) 
	  ;
	else {
	  *(y+0) = *(y+1) = -1.0;
	  return;
	}
	if( QueryPerformanceCounter( &Cnt ) ) 
	  ;
	else {
	  *(y+0) = *(y+1) = -2.0;
	  return;
	}

    *(y+0) = (double)Frequency.QuadPart;
    *(y+1) = (double)Cnt.QuadPart;
}


/* Function to perform cleanup at execution termination */
static void mdlTerminate(SimStruct *S) {
    S = S;
}

#ifdef MATLAB_MEX_FILE  /* Is this file being compiled as a MEX-file? */
#include "simulink.c"        /* MEX-File interface mechanism */
#else
#include "cg_sfun.h"    /* Code generation registration function */
#endif
