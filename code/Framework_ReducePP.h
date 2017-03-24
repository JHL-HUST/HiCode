class Framework_ReducePP:public Framework{//overlapping reduce++
public:
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		Graph g=Graph(cur.N);
		double sum=0;
		vector<vector<int> > cid=comm.getPartitionIDOver();
		vector<double> cs;cs.resize(comm.NC+1);
		vector<double> co;co.resize(comm.NC+1);

		for (int i=1;i<=comm.NC;i++) cs[i]=0;
		#ifdef DEBUG			
			cout<<"G_SIZE"<<cur.g.size()-1<<endl;
		#endif

		for (int z=0;z<cur.g.size();z++){
		#ifdef DEBUG			
			//cout<<"here z : "<<z<<endl;
		#endif
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			sum+=cur.g[z].w;
            int flag = 0, k=0, s=0;
			int cid_i_k=0, cid_j_s=0;
            if (i<=comm.N && j<=comm.N) {
				for(;k<cid[i].size();k++){
					cid_i_k = cid[i][k];
					if( cid_i_k>0 )
					{
			        	co[cid_i_k]+=cur.g[z].w;
					}
					for(;s<cid[j].size();s++){
						cid_j_s = cid[j][s];
						
						if ( (cid_i_k==cid_j_s) && (cid_i_k>0) ){
							flag = 1;
							cs[cid_i_k]+=cur.g[z].w;
							break;		
						}
					}
					//if(flag) break;
				}			
			}

			if(!flag) {
				g.addEdge(i,j,cur.g[z].w);
			}
		}

		for (int z=0;z<cur.g.size();z++){
			int i=cur.g[z].i;
			int j=cur.g[z].j;
			int flag = 0, k=0, s=0;
			int cid_i_k=0, cid_j_s=0;
            if (i<=comm.N && j<=comm.N) {
				for(;k<cid[i].size();k++){
					for(;s<cid[j].size();s++){
						cid_i_k = cid[i][k];
						cid_j_s = cid[j][s];
						if ( (cid_i_k==cid_j_s) && (cid_i_k>0) ){
							flag = 1;
							break;			
						}
					}
					if(flag) break;
				}			
			}

			if (flag) {
				int tcid=cid_i_k;
				int tc_size = comm.id[tcid].size();
				int tc_size_sqr = tc_size*tc_size;
				double pi=cs[tcid]/tc_size_sqr;
				double qi=(co[tcid]-cs[tcid])/(g.N*tc_size-tc_size_sqr);
				if (pi>0)
					g.addEdge(i,j,cur.g[z].w*qi/pi);
			}
		}
		#ifdef DEBUG
			cout<<"reduce end"<<endl;
		#endif
		return g;
	}
};
