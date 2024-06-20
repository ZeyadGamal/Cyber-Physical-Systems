function Out = set(ctras,varargin)
% Set properties of the CTRAS class
%   Writes data from the RT-DAC4/USB / TRAS board

%   Copyright (c) 2011 by InTeCo, Inc.  (2K)

ni = nargin;

ret = tras_usb2_get;
if ret.Status == -1 
  disp('Can not find any RT-DAC/USB device!!!' );
end


% Now left with SET(ctras,'Prop1',Value1)
name = inputname(1);
for i=1:2:ni-1,
   Property = lower( varargin{i} );
   if ni > i+1
      Value = varargin{i+1};
   end;

   switch Property
        
      case 'resetencoder'   
         if Value(1) > 0, ret.EncoderReset(2) = 1; else ret.EncoderReset(2) = 0; end;
         if Value(2) > 0, ret.EncoderReset(1) = 1; else ret.EncoderReset(1) = 0; end;
      
      case 'pwm'
         ret.PWMWidth(1) = Value(1);
         ret.PWMWidth(2) = Value(2);
                 
     case 'pwmprescaler'
         ret.PWMPrescaler(1) = Value(1);
         ret.PWMPrescaler(2) = Value(2);
        
     case 'stop'
         ret.PWMWidth(1) = 0;
         ret.PWMWidth(2) = 0;
      
      otherwise
         % This should not happen
         error('Unexpected property name.')

   end % switch
end % for

ret = tras_usb2_set( ret );

% Finally, assign ctras in caller's workspace
assignin( 'caller', name, ctras )


