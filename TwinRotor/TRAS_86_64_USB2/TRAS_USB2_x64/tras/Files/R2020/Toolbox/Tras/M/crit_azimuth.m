function [qq]=crit_azimuth(x0);
global SD vel_pm   pos_am  time_m pos_ad   t  a  b c d e f g h
a=x0(1);b=x0(2);c=x0(3);%d=x0(4);%e=x0(5);%f=x0(6);%g=x0(7);h=x0(8);
sim('trs_azimuth_pid');
figure(3);plot(t,pos_am,t,pos_ad);
er=pos_am-pos_ad;
%er=er.^2;
er=abs(er);
q=sum(er);
qq=q