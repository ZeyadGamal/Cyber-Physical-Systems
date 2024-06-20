clear all;
global  pos_pm   pos_am  time_m pos_ad  pos_pd t  a  b c d e f g h q11
q11=[];
load  tras_char;
x0=[ 3.1352
    0.0082
    2.2094
    1.2627
    1.4014
    1.2074 ];
oo=[];
oo(1)=-1;oo(2)=.0001;oo(3)=.0001;oo(14)=500;
y=fmins('crit_simple',x0 ,oo);y
figure(2);plot(t,pos_am,'k:',t,pos_ad);grid;title('Azimuth and reference');xlabel('time [s]');
figure(3);plot(t,pos_pm,'k:',t,pos_pd);grid;title('Pitch and reference');xlabel('time [s]');










