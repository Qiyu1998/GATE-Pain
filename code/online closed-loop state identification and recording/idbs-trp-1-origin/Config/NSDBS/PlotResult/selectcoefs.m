function [coefs] = selectcoefs(coefs, N)
%SELECTCOEFS Select the n coefficients from the coefs vector.
%  Use as:
%    [coefs] = selectcoefs(coefs, N);
%  Input:
%    - coefs, coefficient vector
%    - N, number of coeffcients
%  Output:
%    - coefs, coefficient vector
%
%  Author: Yingnan Nie
%  Date: Oct.29th, 2019

L = length(coefs);
if mod(L,2)==1
    if mod(N,2)==1
        coefs=coefs((L+1)/2-(N-1)/2:(L+1)/2+(N-1)/2);
    else
        coefs=coefs((L+1)/2-N/2:(L+1)/2+(N-2)/2);
    end
else
    n=L/2;
    if mod(N,2)==1
        n1=n;n2=n1+1;    
        if data(n1)>data(n2)
            coefs=coefs(n1-(N-1)/2:n1+(N-1)/2);
        else
            coefs=coefs(n2-(N-1)/2:n2+(N-1)/2);
        end
    else
        coefs=coefs(L/2-(N-2)/2:(L+2)/2+(N-2)/2);
    end
end

end

