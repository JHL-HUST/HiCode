function [G]=loadGraph(filename)
	t=textread(filename,'%f');
	n=size(t,1);
	t=reshape(t(2:n),3,(n-1)/3);
	G=sparse(t(1,:),t(2,:),t(3,:));
end