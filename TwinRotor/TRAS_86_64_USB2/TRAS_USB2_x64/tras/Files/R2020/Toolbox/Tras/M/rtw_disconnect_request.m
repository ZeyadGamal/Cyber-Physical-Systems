%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Called by the rtcon_ext_comm when executed the "Disconnect From Target"

function rtw_disconnect_request

  %disp( 'rtw_disconnect_request' );
  set_param(get_param( 0, 'CurrentSystem' ), 'SimulationCommand', 'stop')
  
  
