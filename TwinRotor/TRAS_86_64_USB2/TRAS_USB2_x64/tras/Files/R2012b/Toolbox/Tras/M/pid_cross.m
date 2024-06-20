clear all;
global  vel_pm  pos_pd pos_am  time_m pos_ad   pos_pm t  a  b c d e f g h i j k l q11
load  tras_char;q11=[];
%Q= 847...
x0=[ 3.24656799901910
   0.03674097046462
   2.15204253681359
   0.49786014035824
   0.43926482615375
   0.44646301726895
  -0.93346698212308
  -0.78453530084337
  -0.03630142809237
  -0.02231422574266];
oo=[];oo(1)=-1;oo(2)=.000001;oo(3)=.001;oo(14)=100;
y=fmins('crit_cross',x0 ,oo);y
figure(2);plot(t,pos_am,'k:',t,pos_ad);grid;title('Azimuth and reference');xlabel('time [s]');
figure(3);plot(t,pos_pm,'k:',t,pos_pd);grid;title('Pitch and reference');xlabel('time [s]');







