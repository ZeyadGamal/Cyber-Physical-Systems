function TRAS_BasicTestFunction( Action, Value, MsgTitle, arg1 )

Action = lower( Action );

switch Action
   case 'detectboard'
      Ttl = 'BasicTestFunction - Step 1';
      tr = ctras;
      if get( tr, 'Status' ) == -1 
        Msg =( 'Can not detect any RT-DAC4/USB board !!!');
        errordlg( Msg, Ttl );
        return;
      end
 
      Msg = { 'Detected RT-DAC4/USB board'};
      helpdlg( Msg, Ttl );

     
   case 'resetangleencoders'
      tr = ctras;
      Msg = { 'Move the TRAS system to the origin position'; ' '; ...
              'Do you want to set the origin angles of the load?;'; ' '};
      Ans = questdlg( Msg, 'BasicTestFunction - Step 2' );  
      if( strcmp( Ans, 'Yes' ) == 1 )
        set( tr, 'ResetEncoder', [ 1 1 ] );
        set( tr, 'ResetEncoder', [ 0 0 ] );
      end
      
   case 'checkangles'
      tr = ctras;
      Msg = { 'Move the arm and observe the results on the screen'; ' '; ...
              'Do you want to start?'; ' ' };
      Ans = questdlg( Msg, 'BasicTestFunction - Step 3' );       
      if( strcmp( Ans, 'Yes' ) == 1 )      
        HFig = figure; 
        xlabel( 'Azimuth Angle' ); ylabel( 'Pitch Angle' );
        title( 'Close this figure to terminate the test' );
        plotedit on
        Pos = get( tr, 'Angle' ); 
        BuffLen = 80;
        for i=1:BuffLen; Arr(i,1:2) = Pos; end
        while( ishandle( HFig ) == 1 )
           plot( Arr(:,1), Arr(:,2), Arr(:,1), Arr(:,2), 'x' ); 
           axis( [ -4 4 -1 1 ] ); grid; xlabel( 'Azimuth Angle' ); ylabel( 'Pitch Angle' );
           title( 'Close this figure to terminate the test' );
           drawnow
           Pos = get( tr, 'Angle' );
           Arr( 1:BuffLen-1, : ) = Arr( 2:BuffLen, : ); 
           Arr( BuffLen, 2:-1:1 ) = Pos;
        end
      end
      
   case 'usemouse'
      TRAS_Mouse;
      
   otherwise
      % This should not happen
      error('Unexpected action name.')

end % switch


