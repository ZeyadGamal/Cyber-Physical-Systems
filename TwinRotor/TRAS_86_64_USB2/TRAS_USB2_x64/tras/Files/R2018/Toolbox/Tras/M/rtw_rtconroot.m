function path = rtw_rtconroot
%
% Return RT-CON root directory
%

% Find the directory name of the current file
path = which( mfilename );
path = path( 1 : length( path ) - 4 - length( mfilename ) - 1 );
