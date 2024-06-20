//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  RTW.H  - main header file                                                   //
//                                                                              //
//  Package:    RT-CON                                                          //
//                                                                              //
//  MATLAB versions:             7.14 (2012a)                                    //
//  Simulink versions:           7.9  (2012a)                                    //
//  Simulink Coder:              8.2  (2012a)                                    //
//                                                                              //
//  Macro definitions:                                                          //
//           none                                                               //
//                                                                              //
//  (C) COPYRIGHT 2012 InTeCo Inc., 2K                                          //
//  Last modification: 2012.10.18                                               //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#include "rtmodel.h"

////////////////////////////////////////////////////////////////////////////////////
//
// Functions for RTW DLL library
//
#define REQ_GetTime             "GetTime"        /* get current time */
#define POKE_ResetTime          "ResetTime"
#define REQ_GetModelName        "GetModelName"   /* return empty istring if none DLL is loaded */
#define REQ_GetVersion          "GetVersion"

//
// MATLAB 7 interface functions
//
#define REQ_GetStartFlag        "GetStartFlag"

//
// Execute the single simulation step (off-line mode only)
//
#define REQ_ExecuteSingleStep   "ExecuteSingleStep"     

//
// Functions for RTW_CALL.DLL only
// Load and remove from memory the DLL library
//
#define POKE_LoadLibrary        "LoadLibrary"
#define POKE_UnloadLibrary      "UnloadLibrary"


//
// Functions for manage the level of the displayed messages
//
#define REQ_GetMessageLevel     "GetMessageLevel"
#define POKE_SetMessageLevel    "SetMessageLevel"

#define REQ_PriorityBoost       "GetPriorityBoost"

//
// Functions for RTWTIMER.DLL only
//
#define REQ_GetExecutionTime    "GetExecutionTime"
#define POKE_SetDelay           "SetDelay"       
#define REQ_GetDelay            "GetDelay"
#define POKE_ResetStatistics    "ResetStatistics"   

#define EXPAND_CONCAT(name1,name2) name1 ## name2
#define CONCAT(name1,name2) EXPAND_CONCAT(name1,name2)
#define RT_MDL_SIMSTR            CONCAT(MODEL,_rtModel)



//
//   $(MODEL).c
//
char *GetDateAndTime( void );

// Definition from the $(MODEL).bio
#include "bio_sig.h"

//
//   rtw_dll.c
//
//extern RT_MDL_SIMSTR  *ptrS;   /* Pointer to global RT_MODEL */
//extern RT_MODEL_rtcon_x64_rtvdp  *ptrS;   /* Pointer to global RT_MODEL */
extern HINSTANCE  hInst;
extern int_T volatile startModel;
int FAR PASCAL PrivateWEP ( short nParameter );
void __stdcall SingleStep( void );

//
//   rtw_aux.c
//
extern int MsgLevel;
extern double TimeBegin;
void   rtDisableInterrupts( void );
void   rtEnableInterrupts(void);
int    StartStreamRedirection( void );
void   StopStreamRedirection( void );
void   RTWMessage( int Lev, char s1[], char s2[], char s3[],
                            char s4[], char s5[], char s6[] );
void   RTWMsg2File( char far str[ ] );
void   ResetTime( void );
double GetCurrentRTWTime( void );

//
//   rtw_ipc.c
//
int    FAR PASCAL GetTimerPeriod( void );
int    FAR PASCAL DLL_IPC_Proc( LPSTR str, LPSTR inp, LPSTR out );


