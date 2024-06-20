Ctrl = [0:0.1:1 1:-0.1:0 ];

ACtrl = 1*Ctrl;
PCtrl = 0*Ctrl;

tr = ctras;
ret = [];
for i=1:length(Ctrl)
  
  set( tr, 'PWM', [ACtrl(i) PCtrl(i)] );
[i ACtrl(i) PCtrl(i)]   
get( tr, 'PWM')  
  pause( 20 )
  ret(i,1)   = Ctrl(i);
  sum = 0;
  for j=1:100
    sum = sum + get( tr, 'AD',10 );
  end
  ret(i,2) = sum / 100;
  ret(i,2) = ret(i,2)/2.895;
  plot( ret(:,1), ret(:,2), 'x' ); 
  hold on; plot( ret(:,1), ret(:,2) ); hold off; grid
  title( 'Force vs. PWM' ); xlabel('PWM control value'); ylabel( 'Force [N]' );
end

% Switch off the control 
set( tr, 'PWM', [0 0] );
