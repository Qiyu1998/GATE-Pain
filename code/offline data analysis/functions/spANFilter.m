function output=spANFilter(attenuation,filterParameters,psdParameters,signal,outputType)
%SPANFILTER Objective/filter used by the archived adaptive-notch method.
% outputType 0: center-minus-neighbor power residual (optimizer objective).
% outputType 1: filtered signal. outputType 2: before/after linear PSD table.
stopAttenuation=attenuation(1); passRipple=stopAttenuation/10;
fs=filterParameters(1); center=filterParameters(2);
stopWidth=filterParameters(3); passWidth=filterParameters(4);
passEdges=[center-passWidth center+passWidth]/(fs/2);
stopEdges=[center-stopWidth center+stopWidth]/(fs/2);
[n,cutoff]=buttord(passEdges,stopEdges,passRipple,stopAttenuation);
[b,a]=butter(n,cutoff,'stop');
filtered=filtfilt(b,a,signal);
if outputType==1,output=filtered;return;end
nFFT=psdParameters(1); overlap=psdParameters(2); nWindow=psdParameters(3);
assert(psdParameters(4)==1,'This example uses the original symmetric Hann window.');
window=gate_hann(nWindow,'symmetric');
[power,frequency]=pwelch(detrend(filtered),window,overlap,nFFT,fs,'onesided');
centerMask=frequency>center-stopWidth & frequency<center+stopWidth;
leftMask=frequency>psdParameters(5) & frequency<center-2*passWidth;
rightMask=frequency>center+2*passWidth & frequency<psdParameters(6);
assert(any(centerMask)&&any(leftMask)&&any(rightMask),'Notch reference bands are empty.');
if outputType==0
    output=mean(power(centerMask))-(mean(power(leftMask))+mean(power(rightMask)))/2;
elseif outputType==2
    [before,~]=pwelch(signal,window,overlap,nFFT,fs,'onesided');
    output=[frequency before power];
else
    error('Unknown outputType.');
end
end
