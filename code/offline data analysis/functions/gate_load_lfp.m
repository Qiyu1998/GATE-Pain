function [signal,fs]=gate_load_lfp(path)
%GATE_LOAD_LFP Read one MAT vector without filtering, scaling or modifying it.
% Required fields: data1 (or data), and scalar fs in Hz.
assert(isfile(path),'Input MAT file was not found: %s',path);
S=load(path);
assert(isfield(S,'fs'),'MAT must contain its true sampling rate as fs.');
fs=double(S.fs);
validateattributes(fs,{'double'},{'scalar','finite','positive'});
if isfield(S,'data1')
    signal=S.data1;
elseif isfield(S,'data')
    signal=S.data;
else
    error('Expected one continuous vector named data1 or data.');
end
validateattributes(signal,{'numeric'},{'vector','finite','nonempty','real'});
signal=double(signal(:));
end
