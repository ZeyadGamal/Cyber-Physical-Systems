%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Commands of the ' R E M O V E ' button
% 
% Called by the rtcon_ext_comm when executed the "Disconnect From Target" and 
%                                                "Stop real-time code"
%
function rtw_remove

  %disp( 'rtw_remove' );
  
  if isempty(rtw_call('getmodelname')) == 1 
    return;
  else
    rtw_call UnloadLibrary;
  end  
      

    