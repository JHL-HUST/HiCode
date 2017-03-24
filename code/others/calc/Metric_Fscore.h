double calcPrecision(Community comm1,Community comm2)
{
	comm1.attach(comm1.leftover);
	comm2.attach(comm2.leftover);
	
	vector<int> cid1=comm1.getPartitionID();
	vector<int> cid2=comm2.getPartitionID();

	vector<double> rc;rc.resize(comm.NC+1);
	vector<double> pr;pr.resize(comm.NC+1);
	for (int i=1;i<=comm.NC;i++) rc[i]=0;
	for (int i=1; i<=comm.NC; i++) copri]=0;
	
	int N = comm.N;
}

double calcRecall(Community comm1,Community comm2)
{
	
}

double calcFscore(Community comm1,Community comm2){
	double hx=calcEntropy(comm1);
    double hy=calcEntropy(comm2);
    double MI=calcMutualInfo(comm1,comm2);
    return 2*MI/(hx+hy);
}
