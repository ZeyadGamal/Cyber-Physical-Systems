function [qq]=crit_cross(x0);
global  vel_pm  pos_pd  pos_am  time_m pos_ad   pos_pm t  a  b c d e f g h i j k l q11
   a=x0(1);b=x0(2);
   c=x0(3);d=x0(4);e=x0(5);f=x0(6);
   g=x0(7);h=x0(8);i=x0(9);j=x0(10);%k=x0(5);l=x0(6);
   sim('trs_cross_pid');
   er1=pos_am(1000:end)-pos_ad(1000:end);
   er2=pos_pm(1000:end)-pos_pd(1000:end);
   er3=control_am(1000:end)+.1;
   er1=abs(er1);er2=abs(er2);er3=abs(er3);
   q1=sum(er1);q2=sum(er2);q3=sum(er3);
   q=(4*q1+q2+q3);
   q11=[q11,q];figure(1);semilogy(q11);title('Objective function');
   
   qq=q;