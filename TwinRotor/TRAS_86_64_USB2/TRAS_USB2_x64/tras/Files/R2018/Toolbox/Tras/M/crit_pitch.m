function [qq]=crit_pitch(x0);
global SD vel_pm   pos_pm  time_m pos_pd   t  a  b c d e f g h
a=x0(1);b=x0(2);c=x0(3);%d=x0(4);%e=x0(5);%f=x0(6);%g=x0(7);h=x0(8);
sim('trs_pitch_pid');
figure(1);plot(t,pos_pm,t,pos_pd);
er=pos_pm-pos_pd;
er=er.^2;
%er=abs(er);
q=sum(er);
qq=q