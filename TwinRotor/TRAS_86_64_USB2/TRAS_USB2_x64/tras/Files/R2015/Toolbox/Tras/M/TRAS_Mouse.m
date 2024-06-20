function tras_mouse( action, in1, in2 );
% TRAS_Mouse Interactive open-loop control

%   Copyright (c) 2005 by InTeCo, Inc.  (2K)

tr = ctras;

if nargin<1,
  action = 'start';
end;

persistent aux var;
var = 0;
%
%  aux( 1 ) - handler to vertical line
%  aux( 2 ) - handler to horizontal line
%  aux( 3 ) - handler to tacho voltage line
%

%Line width
lw = 6;
if strcmp( action, 'start' ),

    %====================================
    % Graphics initialization
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Detect if window exists. If exists move it to the foreground
    %
    WndTitle = 'Use mouse to open-loop control';
    HdHandles = get( 0, 'ShowHiddenHandles' );
    set( 0, 'ShowHiddenHandles', 'on' );
    WndHnd = findobj('Name',WndTitle);
    set( 0, 'ShowHiddenHandles', HdHandles )    
    if ~isempty( WndHnd )
      figure( WndHnd );
      return;
    end

    OldFigNumber = watchon;

    figNumber = figure;
    set( gcf, ...
              'NumberTitle', 'off',...
              'Name',WndTitle,...
	       'backingstore', 'off',...
              'Units', 'normalized',...
              'Color',[0.7 0.7 0.7], ...
              'Position',[ 0.1488 0.1983 0.7 0.7 ],...
              'Menubar', 'none'); 
     rem = uimenu( gcf,...
	      'Label','Color &options', ...
	      'Tag','Option_Tag' );
     uimenu( rem,...
	      'Callback','whitebg;', ...
	      'Label','Background &dark / bright', ...
	      'Tag','Dark_Tag' );
     uimenu( rem,...
	      'Callback','whitebg; set( gcf, ''Color'', [ 0.55 0.66 0.143 ] );', ...
             'Label','&Background light green, axes black / white', ...
             'Tag','Light_Green_Tag' );
     
    %====================================
    % Information for all buttons
    top = 0.95;
    bottom = 0.05;
    left = 0.82;
    yInitLabelPos = 0.90;
    btnWid = 0.13;
    btnHt = 0.08;
    RadRefPos = 0.8;
    RadWid = 0.13;
    RadHt = 0.08;
    % Spacing between the label and the button for the same command
    btnOffset = 0.02;
    % Spacing between the button and the next command's label
    spacing = 0.02;
    %bottom=bottom+spacing;
 
    %====================================
     
    
   %====================================
    % The CONSOLE frame
    frmBorder=0.02;
    yPos=0.02;
    frmPos=[left-frmBorder bottom-frmBorder btnWid+2*frmBorder ...
	    0.9+2*frmBorder];
    h=uicontrol( ...
        'Style','frame', ...
        'Units','normalized', ...
        'Position',frmPos, ...
        'BackgroundColor',[0.5 0.5 0.5]);

    %====================================
    % The STOP button
    uicontrol( ...
        'Style','push', ...
        'Units','normalized', ...
        'Position',[left 0.9+2*frmBorder-btnHt-spacing  btnWid btnHt], ...
        'BackgroundColor', [1 0 0], ...
        'String','S T O P', ...
        'Callback','TRAS_Stop');
 
    %====================================
    % The INFO button
    uicontrol( ...
        'Style','push', ...
        'Units','normalized', ...
        'Position',[left bottom+btnHt+spacing  btnWid btnHt], ...
        'String','Info', ...
        'Callback','TRAS_Mouse(''info'')');
 
   %========================================
   % The CLOSE button
    done_button=uicontrol('Style','Pushbutton', ...
        'Units','normalized',...
        'Position',[left bottom btnWid btnHt], ...
        'Units','normalized',...
        'Callback','TRAS_Mouse(''done'')','String','Close');
   %====================================

    % time domain
    ax=axes('Position',[.12 .12 .6 .78],'XLim',[-1 1],'YLim',[-1 1]);
    hold on

    % (set to xor mode to prevent re-rendering, that is, for speed)
    axis([-1 1 -1 1]);
    grid;
    xlabel('Azimuth');
    ylabel('Pitch');
    title('Click and drag to set new control values');

    set(ax,'ButtonDownFcn','TRAS_Mouse(''down'',1)');

    aux( 1 ) = plot( [ 0 0 ], [ 0 0], 'EraseMode', 'xor' );
    aux( 2 ) = plot( [ 0 0 ], [ 0 0], 'EraseMode', 'xor' );
    aux( 2 ) = plot( [ 0 0 ], [ 0 0], 'EraseMode', 'xor' );

    drawnow;
    set( figNumber, 'HandleVisibility', 'Callback');
elseif strcmp(action,'down'),
    TRAS_Mouse('plot');

    %set(gcf,'WindowButtonMotionFcn', sprintf('TRAS_Mouse(''move'',%g)',in1));
    set(gcf,'WindowButtonDownFcn', sprintf('TRAS_Mouse(''down'',%g)',in1));

elseif strcmp(action,'move'),
    TRAS_Mouse('plot');

elseif strcmp(action,'down'),
    TRAS_Mouse('plot');

    set(gcf,'WindowButtonMotionFcn','');
    set(gcf,'WindowButtonUpFcn','');

    TRAS_Mouse('redraw');

elseif strcmp(action,'redraw'),
    drawnow;

elseif strcmp(action,'plot'),
    pt=get(gca,'currentpoint');
    x=pt(1,1);
    y=pt(1,2);
    if (x >  1) x =  1; end
    if (x < -1) x = -1; end
    if (y >  1) y =  1; end
    if (y < -1) y = -1; end
    delete( aux( 1 ) );   delete( aux( 2 ) );
    if( x >= 0 ) col = 'r'; else col = 'g'; end
    aux( 1 ) = plot( [ 0 x ], [ y y ], 'EraseMode', 'xor', ...
                     'Color', col, 'LineWidth', lw );
    if( y >= 0 ) col = 'r'; else col = 'g'; end
    aux( 2 ) = plot( [ x x ], [ 0 y ], 'EraseMode', 'xor', ...
                     'Color', col, 'LineWidth', lw );
    
    % Set open-loop control
    set( tr, 'PWM', [ x y ] );
    
    Volt=get(tr,'Voltage');
    Volt = Volt/10; % To fit into the window range
    plot([0 Volt(1)], [0 Volt(2)]); plot(Volt(1),Volt(2),'or');
    drawnow     

elseif strcmp(action,'info'),
    ttlStr='Use mouse to set new open-loop control values';
    hlpStr= ...                                              
        {' '
         ' Click or drag mouse to set new control values.'
         ' Azimuth and pitch control values may be set simultaneously.'
         ' Observe the relation between the control signals'
         ' and the rotor velocities.'
         ' '
         ' Press the ''S T O P'' button to switch the controls off.'
         ' '
         ' File name: TRAS_Mouse.m '};
 
     helpwin( hlpStr, ttlStr );

elseif strcmp(action,'done'),
    TRAS_Stop;
    close(gcf);
    clear global aux

end
