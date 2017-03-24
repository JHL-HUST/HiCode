function [overlap]=calcCommOverlap(comm1, comm2)
	for i=1:max(comm1)
		for j=1:max(comm2)
			overlap(i,j)=sum((comm1==i).*(comm2==j));
			if (overlap(i,j)>0)
				overlap(i,j)=overlap(i,j)/sum(min(1,(comm1==i)+(comm2==j)));
		end
	end
end