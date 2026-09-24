function data=spAdaptiveNotchFilter(signal,fs,notchWidthHz,referenceWidthHz,centerHz)
%SPADAPTIVENOTCHFILTER Original adaptive-notch algorithm, documented interface.
% signal: samples x channels; fs: Hz. Fits stopband attenuation to neighboring
% spectral power with the original bounds and PSD settings. Optimization and
% Signal Processing Toolboxes are required. No frequency bins are overwritten.
validateattributes(signal,{'numeric'},{'2d','finite','nonempty'});
stopHalfWidth=notchWidthHz/2;
passHalfWidth=stopHalfWidth+1;
filterParameters=[fs,centerHz,stopHalfWidth,passHalfWidth];
% nfft, overlap, window samples, window selector, lower/upper reference limits.
psdParameters=[16384,round(0.4*fs),round(fs),1, ...
    centerHz-referenceWidthHz,centerHz+referenceWidthHz];
initialAttenuation=0.0001; lowerBound=0.00005; upperBound=10;
options=optimoptions('lsqnonlin','Display','off');
data=zeros(size(signal));
for channel=1:size(signal,2)
    x=signal(:,channel);
    attenuation=lsqnonlin(@spANFilter,initialAttenuation,lowerBound,upperBound, ...
        options,filterParameters,psdParameters,x,0);
    data(:,channel)=spANFilter(attenuation,filterParameters,psdParameters,x,1);
end
end
