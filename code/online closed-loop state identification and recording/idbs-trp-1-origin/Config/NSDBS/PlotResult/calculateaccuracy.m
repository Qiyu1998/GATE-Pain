function calculateaccuracy(datapath, filedate, simupath, czone)
%CALCULATEACCURACY Calculate the detection accuracy.
%  Use as:
%    calculateaccuracy(datapath, filedate, simupath);
%  Input:
%    datapath, the path of NSDBS data
%    filedate, the date of the NSDBS data or 'latest'
%    simupath, the path of the simulated data (signal.txt, state.txt)
%    zone, compare zone
%
%  Author: Yingnan Nie
%  Date: Feb 5th, 2020

%% Load NDDBS data
[~, ~, ~, state, ~, info] = readnsdbsdata(datapath, filedate);

%% Load stimulation data
[~,stateTrue] = readsimulation(simupath);

%% Re-sampling
%  Re-sample the data vectors to a same value, to make them comparable.
nPointStep = info.step;
fs = info.fs;

state = repmat(state, 1, nPointStep);
state = reshape(state', numel(state), 1);

%% Comparison
cState = state;
cStateTrue = stateTrue;

idx = czone(1)*fs:czone(2)*fs;

nTP = 0; % True positive
nTN = 0; % True negetive
nFP = 0; % False positive
nFN = 0; % False negetive
for i=idx
    if(cStateTrue(i)==1)
        if(cState(i)==1)
            nTP = nTP+1;
        else
            nFN = nFN+1;
        end
    else
        if(cState(i)==1)
            nFP = nFP+1;
        else
            nTN = nTN+1;
        end
    end
end

sensitivity = nTP/(nTP+nFN);
specificity = nTN/(nFP+nTN);

fprintf('Sensitivity: %f\n', sensitivity);
fprintf('Specificity: %f\n', specificity);
end