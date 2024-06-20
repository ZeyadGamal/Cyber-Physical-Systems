function Value = get( ctras, Property, Arg )
% Get properties of the CTRAS class
%   Reads data from the RT-DAC/USB2 / TRAS board

%   Copyright (c) 2011 by InTeCo, Inc.  (2K)

Property = lower( Property );

ret = tras_usb2_get;
if ret.Status == -1 
  disp('Can not find any RT-DAC/USB device!!!' );
end

switch Property
   case 'status'
      Value = ret.Status;
   
   case 'bitstreamversion'
      Value = ret.BitstreamVersion;
   
   case 'encoder'   
      Value(1) = ret.EncoderCounter(2);
      Value(2) = ret.EncoderCounter(1);

   case 'resetencoder'   
      Value(1) = ret.EncoderReset(2);
      Value(2) = ret.EncoderReset(1);
      
   case 'angle'      
      Value(1) = ret.EncoderCounter(2);
      Value(2) = ret.EncoderCounter(1);
      Value = Value.*ctras.AngleScaleCoeff;
      
   case 'voltage'   
      Value(1) = ret.Voltage(2);
      Value(2) = ret.Voltage(1);

  case 'ad'   
      Value(1) = ret.AD(Arg);
    
  case 'rpm'      
      Value(1) = ret.Voltage(2);
      Value(2) = ret.Voltage(1);
      Value = Value.*ctras.RPMScaleCoeff;
      
   case 'pwm'
      Value(1)= ret.PWMWidth(1); 
      Value(2)= ret.PWMWidth(2); 
      
   case 'pwmprescaler'
      Value(1)= ret.PWMPrescaler(1); 
      Value(2)= ret.PWMPrescaler(2); 
      
   case 'pwmtherm'
      Value(1)= ret.PWMStatus(1); 
      Value(2)= ret.PWMStatus(2); 
       
   case 'pwmthermflag'
     Value = [1 1];

   case 'anglescalecoeff'
      Value = ctras.AngleScaleCoeff;
      
   case 'rpmscalecoeff'
      Value = ctras.RPMScaleCoeff;
      
   case 'time'
      Value = gettime - ctras.Time;

   otherwise
      % This should not happen
      error('Unexpected property name.')
      
   end % switch
   
% Finally, assign ctras in caller's workspace
name = inputname(1);
assignin( 'caller', name, ctras )
   
return;

function ret=AD( BA, ChanNo, Gain )
  ret = mex_rtdac4pci_ad( BA, ChanNo, Gain );
  if ret > 2047
    ret = ret -4096;
  end
  ret = 10*ret/2048;