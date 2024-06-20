%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Commands of the ' S T A R T ' button
%
% Called by the rtcon_ext_comm when executed "Connect To Target"
% Detects the hardware key; loads the DLL executable
%
function rtw_start

  ModelName = rtw_call('getmodelname');
  if isempty( ModelName ) ~= 1 
    rtw_call( 'UnloadLibrary' );
  end  

   name = bdroot(get_param( 0, 'CurrentSystem' ));
  fr = [ '.\' name '_RTCON\' name '.DLL' ];

  if exist(fr, 'file') > 0
    ret = rtw_call( 'LoadLibrary', fr );
  else
    errordlg( [ 'Can not copy the ' fr ' file' ], 'rtw_start' );
  end
  



