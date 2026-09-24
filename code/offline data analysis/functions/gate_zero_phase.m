function [filtered,paddingSamples]=gate_zero_phase(sos,gain,signal)
%GATE_ZERO_PHASE Zero-phase SOS filtering with explicit edge protection.
% High-order, narrow-transition filters can create large endpoint transients
% when only filtfilt's default short extension is used. Reflect the signal far
% enough for the slowest pole's envelope to decay below 1e-8, then trim ONLY
% the artificial padding. Every original sample remains in the output.
signal=signal(:);
radius=0;
for section=1:size(sos,1)
    radius=max(radius,max(abs(roots(sos(section,4:6)))));
end
assert(radius<1,'Filter poles must lie inside the unit circle.');
paddingSamples=6*size(sos,1);
if radius>0
    paddingSamples=max(paddingSamples,ceil(log(1e-8)/log(radius)));
end
assert(numel(signal)>paddingSamples+1, ...
    'Record is too short for this filter settling time. Use a longer continuous record.');
left=2*signal(1)-signal(paddingSamples+1:-1:2);
right=2*signal(end)-signal(end-1:-1:end-paddingSamples);
extended=[left;signal;right];
extended=filtfilt(sos,gain,extended);
filtered=extended(paddingSamples+(1:numel(signal)));
end
