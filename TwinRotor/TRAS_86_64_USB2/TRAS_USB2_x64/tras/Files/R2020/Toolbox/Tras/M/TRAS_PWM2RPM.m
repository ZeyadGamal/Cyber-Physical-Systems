function ChStat = TRAS_PWM2RPM( SelectRotor, CtrlDirection, MinControl, MaxControl, NoOfPoints )
%
% ret = PWM2RPM( SelectRotor, CtrlDirection, NoOfPoints )
%
% Measure RPM( PWM ) characterictic 
% Change control value in the full control range and measure the velocity 
% of the rotor(s)
% Parameters:
%    SelectRotor   - select the rotor to be controlled (string).
%                    Available values are: 'A' for azimuth rotor, 'P' for
%                    pitch rotor and 'AP' for both rotors.
%    CtrlDirection - selects how changes the control value (string).
%                    Enter 'A' for ascending changes (from -1 to 1),
%                    'D' for descending control changes (from 1 to -1) or
%                    'R' for reverse changes (from -1 to +1 and to -1).
%    MinControl, MaxControl - minimal and maximal control signal
%    NoOfPoints    - Number of characteristic points within the -1/+1 range
%                    The Characteristics always contains the -1/+1 points
%                    and the points declared by this parameter.

%   Copyright (c) 2005 by InTeCo, Inc.  (2K)

SelectRotor   = lower( SelectRotor );
CtrlDirection = lower( CtrlDirection );
NoOfPoints    = max( 1, NoOfPoints-1 );

% Control step
Step = (MaxControl-MinControl) / NoOfPoints;

switch CtrlDirection
  case 'a'   
    Ctrl = MinControl:Step:MaxControl;
  case 'd'   
    Ctrl = MaxControl:-Step:MinControl;
  case 'r'   
    Ctrl = [ MinControl:Step:MaxControl MaxControl:-Step:MinControl];
  otherwise  % This should not happen
    error('The CtrlDirection must be ''A'',''D'' or ''R''.')
end 

switch SelectRotor
  case 'a'   
    ACtrl = Ctrl; PCtrl = 0*Ctrl;
  case 'p'   
    ACtrl = 0*Ctrl; PCtrl = Ctrl;
  case { 'ap', 'pa' }   
    ACtrl = Ctrl; PCtrl = Ctrl;
  otherwise  % This should not happen
    error('The SelectRotor must be ''A'', ''P'' or ''AP''.')
end 

FigNum = figure( 'Visible', 'on', ...
                 'NumberTitle', 'off', ...
	             'Name', 'Rotor velocity vs. PWM characteristic', ...
                 'Menubar', 'none' );
tr = ctras;
set (tr, 'PWMPrescaler',[0 0]);     
ret = [];
for i=1:length(Ctrl)
  set( tr, 'PWM', [ACtrl(i) PCtrl(i)] );
%[i ACtrl(i) PCtrl(i)]   
%get( tr, 'PWM')  
  pause( 10 )
  ret(i,1)   = Ctrl(i);
  AuxVolt = [0 0];
  for j=1:10
    AuxVolt = AuxVolt + get( tr, 'RPM' );
  end
  ret(i,2:3) = AuxVolt/10;
%  AuxVolt = 0;
%  for j=1:10
%    AuxVolt = AuxVolt + get( tr, 'AD', 10 );
%  end
%  ret(i,4) = AuxVolt/10;
  plot( ret(:,1), ret(:,2:3), 'x' ); 
  hold on; plot( ret(:,1), ret(:,2:3) ); hold off; grid
  title( 'RPM vs. PWM' ); xlabel('PWM control value'); ylabel( 'Rotor velocity [RPM]' );
end

ChStat.Control = ret(:,1);
ChStat.RPM     = ret(:,2:3);
ChStat.Force   = 0; %ret(:,4);

% Switch off the control 
set( tr, 'PWM', [0 0] );
