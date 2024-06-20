function display( ctras )
% Display properties of the ABS object

%   Copyright (c) 2011 by InTeCo, Inc.  (2K)

name = inputname(1);
assignin( 'caller', name, ctras );

ctras.BitstreamVersion  = get( ctras, 'BitstreamVersion' );
ctras.Encoder           = get( ctras, 'Encoder' );
ctras.ResetEncoder      = get( ctras, 'ResetEncoder' );
ctras.PWM               = get( ctras, 'PWM' );
ctras.PWMPrescaler      = get( ctras, 'PWMPrescaler' );
ctras.PWMTherm          = get( ctras, 'PWMTherm' );
ctras.PWMThermFlag      = get( ctras, 'PWMThermFlag' );
ctras.Voltage           = get( ctras, 'Voltage' );
ctras.Angle             = get( ctras, 'Angle' );
ctras.RPM               = get( ctras, 'RPM' );

disp( [ 'Type:               '   ctras.Type ] )
disp( [ 'Bitstream ver.:     x'  dec2hex( ctras.BitstreamVersion ) ] )
disp( [ 'Encoder:            [ ' num2str( ctras.Encoder(1) ) '  ' num2str( ctras.Encoder(2) ) ' ][bit]' ] )
disp( [ 'Reset Encoder:      [ ' num2str( ctras.ResetEncoder(1) ) '  ' num2str( ctras.ResetEncoder(2) ) ' ]' ] )
disp( [ 'Input voltage:      [ ' sprintf( '%.2f', ctras.Voltage(1) ) '  ' sprintf( '%.2f', ctras.Voltage(2) ) ' ][V]'  ] )
disp( [ 'PWM:                [ ' num2str( ctras.PWM(1) ) '  ' num2str( ctras.PWM(2) ) ' ]' ] )
disp( [ 'PWM Prescaler:      [ ' num2str( ctras.PWMPrescaler(1) ) '  ' num2str( ctras.PWMPrescaler(2) ) ' ]' ] )
disp( [ 'PWM Thermal Status: [ ' num2str( ctras.PWMTherm(1) ) '  ' num2str( ctras.PWMTherm(2) ) ' ]' ] )
disp( [ 'PWM Thermal Flag:   [ ' num2str( ctras.PWMThermFlag(1) ) '  ' num2str( ctras.PWMThermFlag(2) ) ' ]' ] )
disp( [ 'Angle:              [ ' num2str( ctras.Angle(1) ) '  ' num2str( ctras.Angle(2) ) ' ][rad]' ] )
disp( [ 'RPM:                [ ' sprintf( '%.0f', ctras.RPM(1) ) '  ' sprintf( '%.0f', ctras.RPM(2) ) ' ][RPM]' ] )

disp( [ 'Time:               '   sprintf( '%.1f', get( ctras, 'Time' )/1000 ) ' [sec]' ] )
