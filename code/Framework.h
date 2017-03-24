class Framework{
public:
	map<string,string> config;
	void setConfig(map<string,string> config){
		this->config=config;
	}
	virtual bool checkRequire(){
		return true;
	}
	virtual Graph calcNextLayerGraph(Graph cur,Community comm){
		return cur;
	}
	virtual Graph calcLayerGraph(Graph cur,vector<Community> comms){
		for (int i=0;i<comms.size();i++)
			cur=calcNextLayerGraph(cur,comms[i]);
		return cur;
	}
	virtual Graph calcLayerGraphAll(Graph cur,vector<Community> comms){
		//cout<<"here"<<endl;
		//cur=calcLayerGraphAll(cur,comms);
		return cur;
	}
};
