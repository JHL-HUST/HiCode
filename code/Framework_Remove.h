class Framework_Remove:public Framework{
public:
	//ȥ�������ڲ��ı�
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		Graph g=Graph(cur.N);cout<<"sfsjflksjdfs"<<endl;
		vector<int> cid=comm.getPartitionID();

		//����ͼ��ÿ����
		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			if (cid[i]==cid[j]) continue;	//i j��ͬһ������ʱ ���Ըñ�
			g.addEdge(i,j,cur.g[z].w);	//i j����ͬһ������ �ӵ������ͼ��
		}
		return g;
	}
};
