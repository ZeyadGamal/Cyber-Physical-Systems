//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  RTW_IPC.C  - Inter-process communication function                           //
//           Set/get parameters of the executable file                          //
//           Called from the MATLAB Workspace or by any Windows program         //
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

#define  STRICT
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>


#include "pt_info.h"
#include "bio_sig.h"

#include "rtw.h"

#define VERSION_NO        "7.3"
#define RTK_VERSION_DSRP  "Windows XP/7, 64-bit application"

//#define USE_STANDARD_FILE_FUNCTIONS
//#define CRT_SECURE_NO_WARNINGS

// 
// Applied to delay the RT task after initial messages processed
//
int RunRTFlag = 0;
extern real_T StepSize;

int __stdcall GetTimerPeriod( void ) {
//    return( (unsigned) ( 1000.0 * rtmGetStepSize( ptrS ) ) );
//char Aux[256];
//sprintf(Aux, "Sample time: %f", (float)StepSise );
//MessageBox(NULL, Aux, "ccc", MB_OK );    
    return (unsigned) ( 1000.0 * StepSize );
    //return 10;
}

////////////////////////////////////////////////////////////////////////
// Main IPC procedure; exported from the DLL file
//
int __stdcall DLL_IPC_Proc( LPSTR str, LPSTR inp_arg, LPSTR out_arg ) {
   char far   *str_ptr;
   double far *inp_ptr;
   double far *out_ptr;
   char   far  aux[ 40 ];
   char   far  upr[ 40 ];


   str_ptr = (char far *)   str;
   inp_ptr = (double far *) inp_arg;
   out_ptr = (double far *) out_arg;

   strncpy_s( aux, sizeof(aux), str_ptr, sizeof( aux ) - 1 ); aux[ sizeof( aux ) - 1 ] = '\0';
   _strupr_s( aux, sizeof(aux) );

   // GetTime
   strcpy_s( upr, sizeof(upr), REQ_GetTime ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = GetCurrentRTWTime( );
     return ( 1 );
   }

   // GetModelName
   strcpy_s( upr, sizeof(upr), REQ_GetModelName ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
      strcpy_s( (char far *)out_ptr, 64, (char far *)MODEL_NAME_STR );
      return ( 1 );
   }

   // ResetTime
   strcpy_s( upr, sizeof(upr), POKE_ResetTime ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     ResetTime( );
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = 1.0;
     return ( 1 );
   }
   
   // GetStartFlag
   strcpy_s( upr, sizeof(upr), REQ_GetStartFlag ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = startModel;
     return ( 1 );
   }
   
  // REQ_GetVersion
   strcpy_s( upr, sizeof(upr), REQ_GetVersion ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
    static char Msg[ 256 ], Msg1[ 32 ];
    static OSVERSIONINFO  info;
    static char *ptr;

    info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &info );

    switch ( info.dwPlatformId )
    {
      case VER_PLATFORM_WIN32s:
        ptr = "Win32s on Windows 3.1";    break;
      case VER_PLATFORM_WIN32_WINDOWS:
        ptr = "Win32 on Windows 95";      break;
      case VER_PLATFORM_WIN32_NT:
        ptr = "Windows NT";               break;
      default:
        ptr = "Unknown Operating System"; break;
    }
    sprintf_s( Msg, sizeof(Msg), "Version: %s\nCompiled: %s\n%s\n\n"
                  "%s ver. %ld.%ld (Build %ld)\n%s", 
                  VERSION_NO, GetDateAndTime( ), RTK_VERSION_DSRP,
                  ptr, info.dwMajorVersion, info.dwMinorVersion,
                  info.dwBuildNumber >> 16, info.szCSDVersion );
    sprintf_s( Msg1, sizeof(Msg1), "RT-Con Professional: %s", MODEL_NAME_STR ); 
    MessageBox( NULL, Msg, Msg1, MB_OK );

    return( 1 );
  }

   // SetMessageLevel
   strcpy_s( upr, sizeof(upr), POKE_SetMessageLevel ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     MsgLevel = (int) *inp_ptr;
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = 1;
     return ( 1 );
   }

   // GetMessageLevel
   strcpy_s( upr, sizeof(upr), REQ_GetMessageLevel ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = MsgLevel;
     return ( 1 );
   }

   // SetRTFlag
   strcpy_s( upr, sizeof(upr), POKE_SetRTFlag ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     RunRTFlag = (int) *inp_ptr;
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = 1;
     return ( 1 );
   }

   // GetRTFlag
   strcpy_s( upr, sizeof(upr), REQ_GetRTFlag ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = RunRTFlag;
     return ( 1 );
   }
   // ExecuteSingleStep
   strcpy_s( upr, sizeof(upr), REQ_ExecuteSingleStep ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     SingleStep( );
	 // Return current simulation time
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = GetCurrentRTWTime( );
     return ( 1 );
   }

   // GetPriorityBoost
   strcpy_s( upr, sizeof(upr), REQ_PriorityBoost ); 
   _strupr_s( upr, sizeof(upr) );
   if( strcmp( aux, upr ) == 0 ) {
     *(out_ptr) = *(out_ptr + 1) = 1.0;
     *(out_ptr + 2) = 0;
#ifdef USE_HIGH_PRIORITY
#ifndef USE_REALTIME_PRIORITY
     *(out_ptr + 2) = 1;    // HIGH_PRIORITY
#endif
#endif
#ifdef USE_REALTIME_PRIORITY
     *(out_ptr + 2) = 2;    // REALTIME_PRIORITY
#endif
     return ( 1 );
   }

   
  // Unrecognised command
  *(out_ptr) = *(out_ptr+1) = 1.0;
  *(out_ptr+2) = -999.999;
  return ( -1 );
}

