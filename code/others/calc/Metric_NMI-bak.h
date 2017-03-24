double calcEntropy(Community comm){
	double E=0;
	for (int i=1;i<=comm.NC;i++){
		double p=comm.id[i].size()/(double)(comm.N);
		if (p>0) E-=p*log(p);
	}
	return E;
}

double calcMutualInfo(Community comm1,Community comm2){
	double I=0;
	
	comm1.attach(comm1.leftover);
	comm2.attach(comm2.leftover);
	
	vector<int> cid1=comm1.getPartitionID();
	vector<int> cid2=comm2.getPartitionID();
	
	vector<map<int,int> > cnt;
	cnt.resize(comm1.NC+1);
	double res=log(0);
	if (cid1.size()!=0&&cid1.size()!=0){	
		for (int i=1;i<=comm1.NC;i++){
			cnt[i].resize(comm2.NC+1);
			for (int j=1;j<=comm2.NC;j++)
				cnt[i][j]=0;
		}
		int N=comm1.N;
		vector<vector<int> > nonzero;
		nonzero.resize(comm1.NC+1);
		for (int i=1;i<=comm1.NC;i++)
			nonzero[i].clear();
		for (int i=1;i<=N;i++){
			if (cnt[cid1[i]][cid2[i]]==0)
				nonzero[cid1[i]].PB(cid2[i]);
			cnt[cid1[i]][cid2[i]]++;
		}

		for (int i=1;i<=comm1.NC;i++){
			double pi=comm1.id[i].size()/(double)(N);
			for (int z=0;z<nonzero[i].size();z++){
				int j=nonzero[i][z];
				double pj=comm2.id[j].size()/(double)(N);
				double pij=cnt[i][j]/(double)(N);
				I+=pij*log(pij/pi/pj);
			}
		}
		res=I;
	}
	comm1.pop();
	comm2.pop();
	return res;
}


double calcNMI(Community comm1,Community comm2){
	double hx=calcEntropy(comm1);
    double hy=calcEntropy(comm2);
    double MI=calcMutualInfo(comm1,comm2);
    return 2*MI/(hx+hy);
}

double calcNMI(string commFile1,string commFile2){
	Community comm1;
	comm1.load(commFile1);
	Community comm2;
	comm2.load(commFile2);
	if (config.find("Metric_CommunitySizeThres")!=config.end()){
		comm1.removeSmallComms(atoi(config["Metric_CommunitySizeThres"].c_str()));
		comm2.removeSmallComms(atoi(config["Metric_CommunitySizeThres"].c_str()));
	}
	double score=calcNMI(comm1,comm2);
	return score;
}
