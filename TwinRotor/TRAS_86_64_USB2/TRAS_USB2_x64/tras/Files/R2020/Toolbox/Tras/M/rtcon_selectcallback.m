function rtcon_selectcallback(hDlg, hSrc)
disp('Run selectcallback to explicity set ExtModeMexFile to rtcon_ext_comm')
slConfigUISetVal(hDlg, hSrc, 'ExtModeMexFile', 'rtcon_ext_comm');
end

