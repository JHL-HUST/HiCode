class Framework_AdaptiveDecreaseWeights:public Framework{
public:
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		Graph g=Graph(cur.N);
		double sum=0;
		vector<int> cid=comm.getPartitionID();
		vector<double> cs;cs.resize(comm.NC+1);
		for (int i=1;i<=comm.NC;i++) cs[i]=0;
		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			sum+=cur.g[z].w;
			if (cid[i]==cid[j]&&cid[i]!=-1) {
				cs[cid[i]]+=cur.g[z].w;
				continue;
			}
			g.addEdge(i,j,cur.g[z].w);
		}
		int M=g.N*g.N;
		int gM=M;
		double gsum=sum;
		for (int i=1;i<=comm.NC;i++){
			gsum-=cs[i];
			gM-=comm.id[i].size()*comm.id[i].size();
		}
		double gp=gsum/gM;
		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			if (cid[i]==cid[j]&&cid[i]!=-1) {
				int tcid=cid[i];
				double pi=cs[tcid]/comm.id[tcid].size()/comm.id[tcid].size();
				double qi=(sum-cs[tcid])/(M-comm.id[tcid].size()*comm.id[tcid].size());
				//qi=gp;
				if (pi>0)
					g.addEdge(i,j,cur.g[z].w*qi/pi);
			}
		}
		return g;
	}
	// void estimateEdgeProbability(Graph g,vector<Community> comms, double &gp,vector<vector<double> >&pi){
	// 	vector<vector<int> > Mi;
	// 	pi.resize(comms.size());
	// 	Mi.resize(comms.size());
		
	// 	for (int i=0;i<pi.size();i++){
	// 		comms[i].calcid();
	// 		pi[i].clear();
	// 		Mi[i].clear();
	// 		for (int j=0;j<comms[i].id.size();j++){
	// 			Mi[i].PB(0);
	// 			pi[i].PB(0);
	// 		}
	// 	}
	// 	gp=0;
		
	// 	int M=0;
	// 	for (int i=1;i<=g.N;i++)
	// 		for (int j=1;j<=g.N;j++){
	// 			pair<int,int> type=MP(-1,0);
	// 			for (int z=0;z<comms.size();z++)
	// 				if (comms[z].cid[i]==comms[z].cid[j]&&comms[z].cid[i]!=1){
	// 					if (type.first==-1) type=MP(z,comms[z].cid[i]);
	// 					else type=MP(-2,0);
	// 				}
	// 			if (type.first==-1) M++;
	// 			else if (type.first!=-2) Mi[type.first][type.second]++;
	// 		}
	// 	for (int z=0;z<g.g.size();z++){
	// 		int i=g.g[z].i;
	// 		int j=g.g[z].j;
	// 		pair<int,int> type=MP(-1,0);
	// 			for (int z=0;z<comms.size();z++)
	// 				if (comms[z].cid[i]==comms[z].cid[j]&&comms[z].cid[i]!=1){
	// 					if (type.first==-1) type=MP(z,comms[z].cid[i]);
	// 					else type=MP(-2,0);
	// 				}
	// 		if (type.first==-1) gp+=g.g[z].w/M;
	// 		else if (type.first!=-2) pi[type.first][type.second]+=g.g[z].w/Mi[type.first][type.second];
	// 	}
	// 	// for (int i=0;i<pi.size();i++){
	// 	// 	for (int j=2;j<pi[i].size();j++)
	// 	// 		printf("%.6f(%d)\t",pi[i][j],Mi[i][j]);
	// 	// 	puts("");
	// 	// }
	// }

	// virtual Graph calcLayerGraph1(Graph ori,vector<Community> comms){
	// 	Graph g=Graph(ori.N);
	// 	double gp;
	// 	vector<vector<double> > pi;
	// 	estimateEdgeProbability(ori,comms,gp,pi);
	// 	for (int z=0;z<ori.g.size();z++){
	// 		int i=ori.g[z].i;
	// 		int j=ori.g[z].j;
	// 		vector<double> tpi;
	// 		tpi.clear();
	// 		for (int q=0;q<comms.size();q++)
	// 			if (comms[q].cid[i]==comms[q].cid[j]&&comms[q].cid[i]!=1)
	// 				tpi.PB(pi[q][comms[q].cid[i]]);
	// 		double spi=1;
	// 		for (int q=0;q<tpi.size();q++)
	// 			spi*=(1-tpi[q]);
	// 		double p=gp/(gp+(1-gp)*(1-spi));
	// 		g.addEdge(i,j,ori.g[z].w*p);
	// 	}
	// 	return g;
	// }
};