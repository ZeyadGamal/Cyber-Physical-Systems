clear all;
global SD vel_pm   pos_pm  time_m pos_pd   t  a  b c d e f g h
load  tras_char;
x0=[0.6784    0.4415    1.3196    1.43];
oo=[];
oo(1)=-1;oo(2)=.001;oo(3)=.001;oo(14)=100;
y=fmins('crit_pitch',x0 ,oo);y
figure(2);plot(t,pos_pm,'k:',t,pos_pd);grid;title('Pitch and reference');








