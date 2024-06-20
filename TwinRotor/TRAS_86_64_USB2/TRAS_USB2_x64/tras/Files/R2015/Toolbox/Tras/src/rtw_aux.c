//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  RTW_AUX.C - auxiliary functions                                             //
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


#define  STRICT
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <time.h>

#include <stdlib.h>
#include <math.h>

#ifdef __WATCOMC__
#include <i86.h>
#endif

#include "rtw.h"

double TimeBegin = 0.0;

//////////////////////////////////////////////////////
// Message levels:
//    0 - basic; only the most important messages
//    1 - medium
//    2 - detailed
//
int MsgLevel = 0;

// rtDisableInterrupts - Generic function for disabling processor interrupts
void rtDisableInterrupts( void ) {
//    _disable( );
}

// rtEnableInterrupts - Generic function for Enabling processor interrupts
void rtEnableInterrupts(void) {
//    _enable();
}

static FILE *new_stdout = NULL, *new_stderr = NULL;

int StartStreamRedirection( void ) {
  char str[ 128 ];

  sprintf( str, "%s.lou", MODEL_NAME_STR );
  if( ( new_stdout = freopen( str, "wt", stdout ) ) == NULL )
  {
    RTWMessage( 0, "Can not redirect the stdout stream\n",
                   "", "", "", "", "" );
    return( -1 );
  }
  printf( "RT-CON DLL model %s stdout stream redirection file\n",
           MODEL_NAME_STR );

  sprintf( str, "%s.ler", MODEL_NAME_STR );
  if( ( new_stderr = freopen( str, "wt", stderr ) ) == NULL )
  {
    RTWMessage( 0, "Can not redirect the stderr stream\n",
                   "", "", "", "", "" );
    return( -2 );
  }
  fprintf( stderr, "RT-CON DLL model %s stderr stream redirection file\n",
                   MODEL_NAME_STR );

  return( 0 );
}

void StopStreamRedirection( void ) {
  if( new_stdout != NULL )   fclose( new_stdout );
  if( new_stderr != NULL )   fclose( new_stderr );
}


void RTWMessage( int Lev, char s1[], char s2[], char s3[],
                          char s4[], char s5[], char s6[] ) {
  static char aux[ 512 ];
  static char title[ 128 ];

  // Skip all messages above the current level
  if( Lev > MsgLevel )
    return;

  sprintf( aux, "%s%s%s%s%s%s", s1, s2, s3, s4, s5, s6 );
  sprintf( title, "InTeCo/2K®™ RT-CON for Windows XP / 7    %s", MODEL_NAME_STR );
  if( Lev > 0 )
    MessageBox( ( HWND) NULL, (LPCSTR) aux, (LPCSTR) title, (UINT) (MB_OK + MB_ICONSTOP) );
  else
    MessageBox( ( HWND) NULL, (LPCSTR) aux, (LPCSTR) title, (UINT) MB_OK );
}

void RTWMsg2File( char far str[ ] ) {
    FILE *fd;
    static char aux[ 1024 ];
    struct tm  time_of_day;
    time_t     ltime;

    time( &ltime );
#ifdef __WATCOMC__
    _localtime( &ltime, &time_of_day );
    _asctime( &time_of_day, aux );
#else
    memcpy( &time_of_day, localtime( &ltime ), sizeof( time_of_day ) );
    strcpy( aux, asctime( &time_of_day ) );    
#endif
    aux[ strlen( aux ) -1 ] = '\0';
    strncat( aux, "  ", 1023 );
    strncat( aux, (char *)str, 1023 );
    fd = fopen( "rtw.msg", "a+t" );
    fprintf( fd, "%s\n", aux );
    fclose( fd );
}

void ResetTime( void ) {
// KK    TimeBegin = rtmGetT( ptrS );
}

double GetCurrentRTWTime( void ) {
// KK     return( rtmGetT( ptrS ) - TimeBegin );
    return 0.0;
}
