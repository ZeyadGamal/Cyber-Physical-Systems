function TRAS_DCCharacteristics ( action )
%
%  Measure Velocity vs. PWM characteristics
%

%   Copyright (c) 2005 by InTeCo, Inc.  (2K)



if nargin<1,  action='init';  end;

if strcmp(action,'init'),

    WndTitle = 'Measure Velocity vs. PWM';
    HdHandles = get( 0, 'ShowHiddenHandles' );
    set( 0, 'ShowHiddenHandles', 'on' );
    WndHnd = findobj('Name',WndTitle);
    set( 0, 'ShowHiddenHandles', HdHandles )    
    if ~isempty( WndHnd )
      figure( WndHnd );
      return;
    end
    
    OldFigNumber = watchon;
	FigNum = figure( 'Visible', 'on', ...
                     'NumberTitle', 'off', ...
	                 'Name', WndTitle, ...
	                 'backingstore', 'off',...
                     'Units', 'normalized',...
                     'Color',[0.8 0.8 0.8], ...
                     'Position',[0.3 0.3 0.30 0.3], ...
                     'Menubar', 'none', ...
                     'Resize', 'off' );

    uicontrol( 'Style', 'text', 'Units', 'normalized', ...
               'Position', [0.05 0.85 0.5 0.1], 'BackgroundColor', [0.8 0.8 0.8], ...
               'HorizontalAlignment','left','String', 'Minimal control value:' )
    uicontrol( 'Style', 'text', 'Units', 'normalized', ...
               'Position', [0.05 0.72 0.5 0.1], 'BackgroundColor', [0.8 0.8 0.8], ...
               'HorizontalAlignment','left','String', 'Maximal control value:' )
    uicontrol( 'Style', 'text', 'Units', 'normalized', ...
               'Position', [0.05 0.59 0.5 0.1], 'BackgroundColor', [0.8 0.8 0.8], ...
               'HorizontalAlignment','left','String', 'Control order:' )
    uicontrol( 'Style', 'text', 'Units', 'normalized', ...
               'Position', [0.05 0.46 0.5 0.1], 'BackgroundColor', [0.8 0.8 0.8], ...
               'HorizontalAlignment','left','String', 'No of measured points:' )
    uicontrol( 'Style', 'text', 'Units', 'normalized', ...
               'Position', [0.05 0.33 0.5 0.1], 'BackgroundColor', [0.8 0.8 0.8], ...
               'HorizontalAlignment','left','String', 'Axis:' )

    uicontrol( 'Style', 'edit', 'Units', 'normalized', ...
               'Position', [0.65 0.85 0.3 0.1], 'BackgroundColor', [1 1 1], ...
               'Tag', 'MinCtrl', 'HorizontalAlignment','right','String', '-1.0' );
    uicontrol( 'Style', 'edit', 'Units', 'normalized', ...
               'Position', [0.65 0.72 0.3 0.1], 'BackgroundColor', [1 1 1], ...
               'Tag', 'MaxCtrl', 'HorizontalAlignment','right','String', ' 1.0' );
    uicontrol( 'Style', 'popupmenu', 'Units', 'normalized', ...
               'Position', [0.65 0.59 0.3 0.1], 'BackgroundColor', [1 1 1], ...
               'Tag', 'Order', 'HorizontalAlignment','right','String', 'Ascending|Descending|Reverse' );
    uicontrol( 'Style', 'edit', 'Units', 'normalized', ...
               'Position', [0.65 0.46 0.3 0.1], 'BackgroundColor', [1 1 1], ...
               'Tag', 'NoOfPoints', 'HorizontalAlignment','right','String', '11' );
    uicontrol( 'Style', 'popupmenu', 'Units', 'normalized', ...
               'Position', [0.65 0.33 0.3 0.1], 'BackgroundColor', [1 1 1], ...
               'Tag', 'Axis', 'HorizontalAlignment','right','String', 'Azimuth|Pitch' );


    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % The pushbuttons
    uicontrol( 'Style','pushbutton', 'Units','normalized', ...
	           'Position', [0.55 0.05 0.4 0.1 ], ...
               'String',' C L O S E ', 'Callback','set( 0, ''ShowHiddenHandles'', ''on'' );close(gcf)');
    uicontrol( 'Style','pushbutton', 'Units','normalized', ...
	           'Position', [0.05 0.20 0.9 0.1 ], ...
               'BackgroundColor', [1 0 0], ...
               'String',' R U N ', 'Callback','TRAS_DCCharacteristics(''run'')');
    uicontrol( 'Style','pushbutton', 'Units','normalized', ...
	           'Position', [0.05 0.05 0.4 0.1 ], ...
               'String',' Help ', 'Callback','TRAS_DCCharacteristics(''help'')');

    % Do the GUI elements visible
    watchoff( OldFigNumber );
	set( FigNum, 'HandleVisibility','off' );

elseif strcmp(action,'run'),
    HdHandles = get( 0, 'ShowHiddenHandles' );
    set( 0, 'ShowHiddenHandles', 'on' );

    Hnd = findobj('Tag','MinCtrl');    MinCtrl    = str2num(get(Hnd, 'String'));
    Hnd = findobj('Tag','MaxCtrl');    MaxCtrl    = str2num(get(Hnd, 'String'));
    Hnd = findobj('Tag','Order');      Order      = get(Hnd, 'Value');
    Hnd = findobj('Tag','NoOfPoints'); NoOfPoints = str2num(get(Hnd, 'String'));
    Hnd = findobj('Tag','Axis');       Axis       = get(Hnd, 'Value');

    set( 0, 'ShowHiddenHandles', HdHandles )    

    if (MinCtrl < -1.0) | (MinCtrl > 1.0)
      msgbox( 'Minimal control must be between -1.0 and +1.0', ...
              'Uncorrect control range', 'error' );
      return;          
    end              
    if (MaxCtrl < -1.0) | (MaxCtrl > 1.0)
      msgbox( 'Maximal control must be between -1.0 and +1.0', ...
              'Uncorrect control range', 'error' );
      return;          
    end              
    if (MaxCtrl <= MinCtrl)
      msgbox( 'Maximal control must be greater then minimal control', ...
              'Uncorrect control values', 'error' );
      return;          
    end              
    switch Order
      case 1
        OrderStr = 'A';   % Ascendinging
      case 2
        OrderStr = 'D';   % Descending
      case 3
        OrderStr = 'R';   % Ascendinging + descending
      otherwise
        disp('Unknown order.')
    end
    if (NoOfPoints < 1) 
      msgbox( 'Number of measured points must be positive', ...
              'Uncorrect number of points', 'error' );
      return;          
    end              
    switch Axis
      case 1
        AxisStr = 'A';   % Azimuth
      case 2
        AxisStr = 'P';   % Pitch
      otherwise
        disp('Unknown order.')
    end
    
    ChStat = TRAS_PWM2RPM( AxisStr, OrderStr, MinCtrl, MaxCtrl, NoOfPoints );
    save ChStat ChStat

elseif strcmp(action,'help'),
    ttlStr=' Collect velocity vs. control characteristic ';
    hlpStr= ...
    ['                                                               '
     ' This experiment allows to measure velocity of the shaft       '
     ' in the function of the control signal.                        '
     ' The minimal and maximal control values determine the span     '
     ' of the control signal.                                        '
     ' The number of measured points define how many control         '
     ' values are generated in the given control range.              '
     ' The control order defines how the control signal changes.     '
     ' In the case of ascending order the control steps from         '
     ' the minimal to the maximal control value. The descending      '
     ' order causes control changes from the maximal to the minimal  '
     ' value. The reverse order performes two successive experiments.'
     ' The first one changes the control in ascending order and      '
     ' the second one in descending order.                           '
     '                                                               '
     ' At the end of the measurements the control and velocities     '
     ' data is stored in the ChStat.mat file                         '
     '                                                               '
     ' File name: TRAS_DCCharacteristics.m                           '];
    helpwin( hlpStr, ttlStr );


end;    % if strcmp(action, ...

