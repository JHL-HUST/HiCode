class Framework_RemoveLinks:public Framework{
public:
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		Graph g=Graph(cur.N);
		vector<int> cid=comm.getPartitionID();
		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			if (cid[i]==cid[j]) continue;
			g.addEdge(i,j,cur.g[z].w);
		}
		return g;
	}
};