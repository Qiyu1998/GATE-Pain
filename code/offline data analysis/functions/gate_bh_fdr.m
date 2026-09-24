function adjusted=gate_bh_fdr(p)
%GATE_BH_FDR Benjamini-Hochberg adjustment for ONE explicitly defined family.
% Keep all planned tests in p, including nonsignificant ones; do not prefilter.
validateattributes(p,{'numeric'},{'vector','finite','nonempty','>=',0,'<=',1});
originalSize=size(p);
[sortedP,order]=sort(p(:)); n=numel(sortedP);
sortedAdjusted=sortedP.*n./(1:n)';
for k=n-1:-1:1
    sortedAdjusted(k)=min(sortedAdjusted(k),sortedAdjusted(k+1));
end
adjusted=zeros(n,1); adjusted(order)=min(sortedAdjusted,1);
adjusted=reshape(adjusted,originalSize);
end
