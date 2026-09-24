function [onset, offset] = nsstatistic(datapath, filedate, section)
%NSSTATISTIC Do statistic for NSDBS data
%  Use as:
%    [onset, offset] = nsstatistic(datapath, filedate);
%  Input:
%    datapath, data path
%    filedate, the date of NSDBS data or 'latest'
%    section, time section of interest
%  Output:
%    onset, onset time
%    offset, offset time
%
%  Author: Yingnan Nie
%  Date: Feb 5th, 2020

%% Load NDDBS data
[~, ~, ~, state, ~, info] = readnsdbsdata(datapath, filedate);

%% Calculate state onset and offset
fs = info.fswpt/info.step;
% Detect edge by minus shifted array, onset point will be 1, offset will be
% -1.
temp1 = [state;0];
temp2 = [0;state];
edge = temp1-temp2;
edge(end) = [];
% Onset & offset time
onset = find(edge==1)/fs;
offset = find(edge==-1)/fs;
% Remove the last incomplete state
if ~isequal(size(onset),size(offset))
    onset(end) = [];
end

%% Time section of interest
onset = onset(onset>section(1)&onset<section(2));
offset = offset(offset>section(1)&offset<section(2));

offset = offset(offset>onset(1));
onset = onset(onset<offset(end));

%% Calculate occurrence rate (1/min)
occurrate = length(onset)/((section(2)-section(1))/60);
fprintf("Occurrence rate(1/min): %f\n", occurrate);

%% Calculate duration
duration = offset-onset;
averageDuration = mean(duration);
stdDuration = std(duration);
fprintf("Duration: %fs (SD:%f)\n", averageDuration, stdDuration);

% Plot histogram
histogram(duration,'BinWidth',0.1); 
end

