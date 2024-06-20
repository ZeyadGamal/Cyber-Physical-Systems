

CWD = pwd;
powrot = pwd;
cd( [ matlabroot '\rtw\c\Tras' ] );

copyfile ('rtcon_tras_usb2.tlc', CWD) ;
cd (powrot);