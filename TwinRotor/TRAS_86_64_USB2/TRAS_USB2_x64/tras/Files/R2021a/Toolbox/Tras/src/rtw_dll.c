//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  RTW_DLL.C  - main DLL function; DLL exported functions                      //
//                                                                              //
//  Package:    RT-CON                                                          //
//                                                                              //
//  MATLAB versions:             7.14 (2012a)                                   //
//  Simulink versions:           7.9  (2012a)                                   //
//  Simulink Coder:              8.2  (2012a)                                   //
//                                                                              //
//  Macro definitions:                                                          //
//           none                                                               //
//                                                                              //
//  (C) COPYRIGHT 2012 InTeCo Inc., 2K                                          //
//  Last modification: 2012.10.18                                               //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define main  MAINSUBSTITUTE
#include "rt_main.c"
#include "ext_svr.h"
#undef main

#include <windows.h>
#ifdef EXT_MODE

// The startModel variable is set by the function rt_PktServerWork (ext_svr.c file)
extern int_T volatile startModel;
extern int   volatile RunRTFlag;

#endif

#include "rtw.h"
    
// KK #include "rtmmacros.h"

int_T       NumOfBlockOutputs;
int_T       NumOfParameters;

RT_MODEL  *ptrS = NULL;
RT_MODEL  *S;

const char *status;
int_T      port      = 17725;
int_T      StartFlag, StopFlag;
int_T      TerminatedFlag;

// Set in the MODEL.c
extern real_T finaltime;

char msg[ 150 ];

static void Terminate( void ) {
    if( TerminatedFlag == TRUE ) return;
    TerminatedFlag = TRUE;
    
    startModel = FALSE;
    
    /*******************************
     * Cleanup and exit (optional) *
     *******************************/

#ifdef UseMMIDataLogging
    rt_CleanUpForStateLogWithMMI(rtmGetRTWLogInfo(RT_MDL));
#endif
    rt_StopDataLogging(MATFILE,rtmGetRTWLogInfo(RT_MDL));

    rtExtModeShutdown(NUMST);

    rt_TermModel();

	RTWMessage( 2, "Terminate\n", "Finished ...",
                       "", "", "", "" );
}


void __stdcall SingleStep( void ) {
#ifdef EXT_MODE
    double Ts = rtmGetStepSize( ptrS ); 
    static int Cnt = -1;
    if( Cnt < 0 )
        Cnt = 2+(int)(1/Ts);                
    //if( TerminatedFlag ) return;

    if ( (rtmGetErrorStatus(RT_MDL) != NULL) ||
        (rtmGetStopRequested(S)) ) {
      Terminate();
      return;
    }
                  
    rtExtModePauseIfNeeded(rtmGetRTWExtModeInfo(RT_MDL),
                           NUMST,
                           (boolean_T *)&rtmGetStopRequested(RT_MDL));
    
    if (rtmGetStopRequested(RT_MDL)) {
       Terminate();
       return;
    }

    rtExtModeOneStep(rtmGetRTWExtModeInfo(RT_MDL),
                     NUMST,
                     (boolean_T *)&rtmGetStopRequested(RT_MDL));
    // Wait for ext_comm 
    if( Cnt > 2 ) 
        Cnt --;
    else
        rt_OneStep();
                  
#endif
}

DWORD ProcessPriority;
int   ThreadPriority;
BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved) {
     lpvReserved=lpvReserved;hDLLInst=hDLLInst;

    switch (fdwReason) {
	  case DLL_PROCESS_ATTACH:
        // The DLL is being loaded for the first time by a given process.
        // Perform per-process initialization here.  If the initialization
        // is successful, return TRUE; if unsuccessful, return FALSE.

        RTWMessage( 2, "DllMain\n", "fdwReason: DLL_PROCESS_ATTACH","", "", "", "" );
        { 
          const char_T *extParseErrorMsg = ExtParseArgsAndInitUD(0, NULL);
          if (extParseErrorMsg != NULL) {
            RTWMessage( 0, "DllMain", "ExtParseArgsAndInitUD error", "", "", "", "" );
            exit(EXIT_FAILURE);
          }
        }
        ProcessPriority = GetPriorityClass(GetCurrentProcess());
        if( ProcessPriority == 0 ) {
            RTWMessage( 0, "DllMain", "GetPriorityClass error", "", "", "", "" );
        }
        ThreadPriority = GetThreadPriority(GetCurrentThread());
        if( ThreadPriority == THREAD_PRIORITY_ERROR_RETURN ) {
            RTWMessage( 0, "DllMain", "GetThreadPriority error", "", "", "", "" );
        }

        
        ////////////////////////////////
        // Init communication
        StartFlag = StopFlag = 0;
        TerminatedFlag = FALSE;
        
    /************************
     * Initialize the model *
     ************************/
    rt_InitModel();

    /* External mode */
    rtSetTFinalForExtMode(&rtmGetTFinal(RT_MDL));
    rtExtModeCheckInit(NUMST);
    rtExtModeWaitForStartPkt(rtmGetRTWExtModeInfo(RT_MDL),
                             NUMST,
                             (boolean_T *)&rtmGetStopRequested(RT_MDL));

    S = RT_MDL;
    ptrS = S;

    finaltime = rtmGetTFinal(RT_MDL);

#ifdef STARTUP_MESSAGE
        sprintf_s( msg, sizeof( msg ),
  #ifdef rtmGetStepSize       
                 "RT-CON DLL model %s loaded\nSampling period: %gsec\n"
  #else      
                 "RT-CON DLL model %s loaded\n"
  #endif
                 "Final time: %gsec\nCompiled: %s\n"
                 "\nRT-CON mode: %s\n",
//                 MODEL_NAME_STR, RT_MDL->Timing.stepSize0, // 0.0, //(float)ssGetFixedStepSize( RT_MDL ),
  #ifdef rtmGetStepSize       
                 MODEL_NAME_STR, rtmGetStepSize( ptrS ), //GetModelStepSize(), // ptrS->Timing.stepSize0, // 0.0, //(float)ssGetFixedStepSize( RT_MDL ),
  #else
                 MODEL_NAME_STR, 
  #endif
                 (float)finaltime, GetDateAndTime( ),
  #ifdef EXT_MODE
                 "external" );
  #else
                 "unknown" );
  #endif
        RTWMessage( 0, "RTW_LOAD\n\n", msg, "", "", "", "" );
#endif      
  
      break;


      case DLL_PROCESS_DETACH:
        // The DLL is being unloaded by a given process.  Do any
        // per-process clean up here, such as undoing what was done in
        // DLL_PROCESS_ATTACH.  The return value is ignored.
        Terminate( );
        
        if( ProcessPriority != 0 ) {
          if(!SetPriorityClass(GetCurrentProcess(), ProcessPriority))    
            RTWMessage( 0, "DllMain", "SetPriorityClass error", "", "", "", "" );
        }
        if( ThreadPriority != THREAD_PRIORITY_ERROR_RETURN ) {
          if(!SetThreadPriority(GetCurrentThread(), ThreadPriority)) 
            RTWMessage( 0, "DllMain", "SetThreadPriority error", "", "", "", "" );
        }
        
        // Cleanup sockets from the rtiostream_tcpip.c
//        WSACleanup();WSACleanup();WSACleanup();

        #ifdef STARTUP_MESSAGE
//        sprintf_s( msg, sizeof( msg ),
//                 "RT-CON DLL model %s removed\n\nFinal time: %f\n", 
//                 MODEL_NAME_STR, (float)rtmGetT(RT_MDL) );
//        RTWMessage( 1, "RTW_UNLOAD\n\n", msg, "", "", "", "" );
#endif          


	  break;

      case DLL_THREAD_ATTACH:
        // A thread is being created in a process that has already loaded
        // this DLL.  Perform any per-thread initialization here.  The
        // return value is ignored.

        RTWMessage( 2, "DllMain\n", "fdwReason: DLL_THREAD_ATTACH",
                       "", "", "", "" );
        break;

      case DLL_THREAD_DETACH:
        // A thread is exiting cleanly in a process that has already
        // loaded this DLL.  Perform any per-thread clean up here.  The
        // return value is ignored.

        RTWMessage( 2, "DllMain\n", "fdwReason: DLL_THREAD_DETACH",
                       "", "", "", "" );

      break;
    }
  return TRUE;
}

