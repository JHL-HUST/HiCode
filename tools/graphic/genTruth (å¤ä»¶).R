table=read.csv('Truth_vs_layer');
names=colnames(table);
for (i in 1:length(names)){
  name=names[i];
  pdf(paste(name,'pdf',sep='.'));
  plot(1:nrow(table),table[1:nrow(table),name],type="n"
       ,xlab="Iteration",ylab=name,main=paste(name,"vs","Iteration")
       );
  points(1:nrow(table),table[1:nrow(table),name]);
  grid();
  graphics.off();
}
