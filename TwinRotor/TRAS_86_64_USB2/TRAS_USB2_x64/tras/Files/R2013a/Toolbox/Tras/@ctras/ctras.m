function ctras = TRAS
% CTRAS constructor function
%
% Properties of the CTRAS class:
%   ( g) 'BaseAddress'      - base address of the RTDAC board
%   ( g) 'BitstreamVersion' - bitstream version of the XILINX chip logic
%   ( g) 'Encoder'          - read encoders
%   (sg) 'ResetEncoder'     - reset encoder(s)
%   ( g) 'Voltage'          - voltage of analog inputs
%   ( g) 'AD'               - voltage of the given analog input
%   (sg) 'PWM'              - PWM duty cycles
%   (sg) 'PWMPrescaler'     - PWM frequency prescalers
%   ( g) 'PWMTherm'         - PWM thermal status
%   (sg) 'PWMThermFlag'     - PWM thermal status flag
%   ( g) 'AngleScaleCoeff'  - encoder pulse/angle units coefficients
%   ( g) 'RPMScaleCoeff'    - voltage/RPM units coefficients
%   ( g) 'Angle'            - pitch and azimuth angles
%   ( g) 'RPM'              - pitch and azimuth rotor velocities
%   ( g) 'Time'             - time in ms since object was created
%
%
%   ( g) - available for the get function
%   ( s) - available for the set function
%   (sg) - available for the get and set functions

%   Copyright (c) 2011 by InTeCo, Inc.  (2K)

ctras.Type = 'CTRAS Object/RT-DAC/USB2';

ctras.Status           = NaN;
ctras.BitstreamVersion = NaN;
ctras.Encoder          = NaN;
ctras.ResetEncoder     = NaN;
ctras.Voltage          = NaN;
ctras.AD               = NaN;
ctras.PWM              = NaN;
ctras.PWMPrescaler     = NaN;
ctras.PWMTherm         = NaN;
ctras.PWMThermFlag     = NaN;
ctras.AngleScaleCoeff  = NaN;
ctras.RPMScaleCoeff    = NaN;
ctras.Angle            = NaN;
ctras.RPM              = NaN;
ctras.Time             = gettime;

ctras=class(ctras,'CTRAS');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

ctras.Status           = get( ctras, 'Status' );
ctras.BitstreamVersion = get( ctras, 'BitstreamVersion' );
ctras.Encoder          = get( ctras, 'Encoder' );
ctras.ResetEncoder     = get( ctras, 'ResetEncoder' );
ctras.PWM              = get( ctras, 'PWM' );
ctras.PWMPrescaler     = get( ctras, 'PWMPrescaler' );
ctras.PWMTherm         = get( ctras, 'PWMTherm' );
ctras.PWMThermFlag     = get( ctras, 'PWMThermFlag' );
ctras.AngleScaleCoeff  = [ 2*pi/4096 2*pi/4096 ];      % [ rad/imp rad/imp ]
ctras.RPMScaleCoeff    = [ 1000/0.52/2 1000/0.52/2 ];  % [ RPM/V RPM/V ] - includes amplifier gain (=2)
ctras.Angle            = get( ctras, 'Angle' );
ctras.RPM              = get( ctras, 'RPM' );

