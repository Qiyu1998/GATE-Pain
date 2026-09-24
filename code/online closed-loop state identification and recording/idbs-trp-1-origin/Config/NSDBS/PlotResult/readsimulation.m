function [signal, state] = readsimulation(datapath)
%READSIMULATION Read simulated signal and state file.
%  Use as:
%    [signal, state] = readsimulation(datapath);
%  Input:
%    datapath, data path (signal.txt, state.txt)
%  Output:
%    signal, simulated signal
%    state, simulated state
%
%  Author: Yingnan Nie
%  Date: Feb 5th, 2020

% Simulated signal
[signal,~] = readnspltxt(fullfile(datapath, 'signal.txt'));
[state, ~] = readnspltxt(fullfile(datapath, 'state.txt')); 

end

