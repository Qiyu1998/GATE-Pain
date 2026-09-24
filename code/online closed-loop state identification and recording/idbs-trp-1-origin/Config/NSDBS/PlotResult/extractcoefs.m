function [coefs] = extractcoefs(signal, node, basis, entropy)
%EXTRACTCOEFS Extract coefficients for target frequency band.
%  Use as:
%    [coefs] = extractcoefs(signal, node);
%  Input:
%    - signal, signal vector
%    - node, target node of the wavelet packet tree
%    - basis, wavelet basis
%    - entropy, the method for computing entropy
%  Output:
%    - coefs, coefficients
%
%  Author: Yingnan Nie
%  Date: Oct. 29th, 2019

dLevel = node(1);
nCoef = ceil(length(signal)/(2^dLevel));

wpTree = wpdec(signal, dLevel, basis, entropy);
coefs = wpcoef(wpTree, node);

coefs = selectcoefs(coefs, nCoef);

end

