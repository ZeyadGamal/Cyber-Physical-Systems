function sl_customization(cm)
%SL_CUSTOMIZATION Model Advisor callback

% Copyright 2006 The MathWorks, Inc.
% $Revision: 1.1.6.4 $  $Date: 2006/12/20 08:34:07 $

% KK KK 
cm.ExtModeTransports.add('rtcon_tras_usb2.tlc', 'RT-CON tcpip', 'rtcon_ext_comm', 'Level1');
cm.ExtModeTransports.add('rtcon_tras_usb2.tlc', 'Default', 'ext_comm', 'Level1');

% Get default (factory) customizations
hObj = cm.RTWBuildCustomizer;

% Register build process hooks
hObj.addUserHook('entry',      'copyfile ( [ matlabroot ''\rtw\c\Tras\rtcon_tras_usb2.tlc''], pwd) ;');


end




