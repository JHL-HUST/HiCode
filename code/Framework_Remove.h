class Framework_Remove:public Framework{
public:
	//去掉社团内部的边
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		Graph g=Graph(cur.N);cout<<"sfsjflksjdfs"<<endl;
		vector<int> cid=comm.getPartitionID();

		//遍历图的每条边
		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			if (cid[i]==cid[j]) continue;	//i j在同一个社团时 忽略该边
			g.addEdge(i,j,cur.g[z].w);	//i j不在同一个社团 加到结果的图中
		}
		return g;
	}
};
