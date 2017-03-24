double calcModularity(Graph g,Community comm){
	double Q=0;
	vector<double> e;e.clear();
	vector<double> a;a.clear();
	double M=0;
	comm.attach(comm.leftover);
	int nc=comm.NC;
	vector<int> cid=comm.getPartitionID();

	for (int i=0;i<=nc;i++){
		e.PB(0);
		a.PB(0);
	}

	if (cid.size()==0) {
		comm.pop();
		return log(0);
	}

	for (int z=0;z<g.g.size();z++){
		int i=g.g[z].i;
		int j=g.g[z].j;
		double w=g.g[z].w;
		if (cid[i]==cid[j] && cid[i]!=-1)
			e[cid[i]]+=w;
		if(cid[i]!=-1)a[cid[i]]+=w;
		M+=w;
	}
	for (int i=1;i<=nc;i++)
		Q+=e[i]/M-a[i]/M*a[i]/M;
	comm.pop();
	/*for(int i=0; i<=cid.size();i++)
		cout<<cid[i]<<" ";
	cout<<"here"<<endl;
	cout<<"calcModularity Graph and comm"<<endl;*/
	return Q;
}

double calcModularity(string graphFile,string commFile){
	Graph g;
	if (!g.load(graphFile)) return log(0);
	Community comm;
	comm.load(commFile);
	if (config.find("Metric_CommunitySizeThres")!=config.end())
		comm.removeSmallComms(atoi(config["Metric_CommunitySizeThres"].c_str()));
	return calcModularity(g,comm);
}

