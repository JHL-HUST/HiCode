class Framework_ReplaceByNoise:public Framework{
public:
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		vector<int> cid=comm.getPartitionID();
		Graph g=Graph(cur.N);
		double sum=0;
		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			if (cid[i]==cid[j]) continue;
			sum+=cur.g[z].w;
			g.addEdge(i,j,cur.g[z].w);
		}
		int maxOutEdges=cur.N*cur.N;
		for (int i=1;i<=comm.NC;i++)
			maxOutEdges-=comm.id[i].size()*comm.id[i].size();
		double p=sum/maxOutEdges;
		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			if (cid[i]!=cid[j]) continue;
			g.addEdge(i,j,cur.g[z].w*p);
		}
		/*for (int z=1;z<=comm.NC;z++){
			for (int i=0;i<comm.id[z].size();i++)
				for (int j=i+1;j<comm.id[z].size();j++)
					{
						g.addEdge(comm.id[z][i],comm.id[z][j],p);
						g.addEdge(comm.id[z][j],comm.id[z][i],p);
					}
		}*/
		return g;
	}
};