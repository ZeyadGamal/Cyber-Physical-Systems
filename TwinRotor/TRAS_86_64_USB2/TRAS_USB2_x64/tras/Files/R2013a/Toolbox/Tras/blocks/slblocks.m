function blkStruct = slblocks
%SLBLOCKS Defines the block library for a specific Toolbox or Blockset.

%   Copyright 2006 InTeCo Ltd, (2K).
%   Date: 2006-12-30

% Information for "Blocksets and Toolboxes" subsystem
blkStruct.Name = sprintf('Real-Time\nConnection');
blkStruct.OpenFcn = 'tras';
blkStruct.MaskDisplay = 'disp(''Tras'')';

% Information for Simulink Library Browser
Browser(1).Library = 'tras';
Browser(1).Name    = 'Windows Real-Time Connection';
Browser(1).IsFlat  = 0;% Is this library "flat" (i.e. no subsystems)?

blkStruct.Browser = Browser;

% End of slblocks
