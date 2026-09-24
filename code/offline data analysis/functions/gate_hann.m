function w=gate_hann(n,mode)
%GATE_HANN Explicit Hann coefficients; avoids a shadowed/custom hann function.
% 'periodic': spectral Welch window; 'symmetric': original notch helper window.
if nargin<2,mode='periodic';end
validateattributes(n,{'numeric'},{'scalar','integer','positive'});
assert(ismember(mode,{'periodic','symmetric'}),'Unknown window mode.');
if n==1,w=1;return;end
denominator=n;
if strcmp(mode,'symmetric'),denominator=n-1;end
w=0.5-0.5*cos(2*pi*(0:n-1)'/denominator);
end
