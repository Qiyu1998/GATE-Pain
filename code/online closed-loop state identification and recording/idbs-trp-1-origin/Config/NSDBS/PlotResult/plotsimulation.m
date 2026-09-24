function plotsimulation(datapath, filedate, simupath)
%PLOTSIMULATION Plot the simulation result.
%  Use as:
%    plotsimulation(datapath, filedate, simupath);
%  Input:
%    datapath, the path of NSDBS data
%    filedate, the date of the NSDBS data or 'latest'
%    simupath, the path of the simulated data (signal.txt, state.txt)
%
%  Author: Yingnan Nie
%  Date: Feb 9th, 2020

%% Load NDDBS data
[~, ~, coef, state, thr, info] = readnsdbsdata(datapath, filedate);

%% Load stimulation data
[signal,stateTrue] = readsimulation(simupath);

%% Re-sampling
%  Re-sample the data vectors to a same value, to make them comparable.
nPointStep = info.step;
fs = info.fs;

coef = reshape(coef', numel(coef), 1);

thr = repmat(thr, 1, nPointStep);
thr = reshape(thr', numel(thr), 1);

state = repmat(state, 1, nPointStep);
state = reshape(state', numel(state), 1);

time = 1/fs:1/fs:length(signal)/fs;

%% Plot
fig = figure;
subplot(411);
plot(time, signal);
set(gca, 'FontSize', 20);
title('Raw Signal');

subplot(412);
hold on;
plot(time, coef);
plot(time, thr, 'r');
plot(time, -thr, 'r')
hold off;
set(gca, 'FontSize', 20);
title('Wavelet Coefficients & Adaptive Thresholds');

subplot(413);
plot(time, state);
set(gca, 'YLim', [-0.2, 1.2]);
set(gca, 'FontSize', 20);
title('Detected States');

subplot(414);
plot(time, stateTrue);
set(gca, 'YLim', [-0.2, 1.2]);
set(gca, 'FontSize', 20);
title('True State');

set(fig,'windowkeypressfcn',@keypressfcn);

function keypressfcn(~,evt)
    axes = get(gcf,'children');
    x = get(axes(1),'XLim');
    
    key = evt.Key;
    switch key
        case 'uparrow'
            lim = [x(1), x(1)+(x(2)-x(1))*0.9];
            for i=1:length(axes)
                set(axes(i), 'XLim', lim);
            end
        case 'downarrow'
            lim = [x(1), x(1)+(x(2)-x(1))*1.1];
            for i=1:length(axes)
                set(axes(i), 'XLim', lim);
            end
        case 'leftarrow'
        	lim = x-(x(2)-x(1))/10;
            for i=1:length(axes)
                set(axes(i), 'XLim', lim);
            end
        case 'rightarrow'
            lim = x+(x(2)-x(1))/10;
            for i=1:length(axes)
                set(axes(i), 'XLim', lim);
            end
    end
end

end