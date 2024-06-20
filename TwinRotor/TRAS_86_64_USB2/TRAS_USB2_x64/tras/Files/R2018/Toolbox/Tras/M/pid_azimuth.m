clear all;
global SD pos_pm   pos_am  time_m pos_ad pos_pd  t  a  b c d e f g h
load  tras_char;
x0=[    4.9395    0.0022    5.1898 ];
oo=[];
oo(1)=-1;oo(2)=.001;oo(3)=.001;oo(14)=100;
y=fmins('crit_azimuth',x0 ,oo);
y
figure(2);plot(t,pos_am,'k:',t,pos_ad);grid;title('Azimuth and reference');








