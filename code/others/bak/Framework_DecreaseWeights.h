class Framework_DecreaseWeights:public Framework{
public:
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		Graph g=Graph(cur.N);
		double rate;
		char s[1000];
		sprintf(s,"%s",config["DecreaseWeights_Rate"].c_str());
		sscanf(s,"%lf",&rate);
		vector<int> cid=comm.getPartitionID();

		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			double multi=1;
			if (cid[i]==cid[j]) multi=rate;
			g.addEdge(i,j,cur.g[z].w*multi);
		}
		return g;
	}
};