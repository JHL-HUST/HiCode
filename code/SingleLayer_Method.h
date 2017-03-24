class SingleLayer_Method{
public:
	map<string,string> config;
	void setConfig(map<string,string> config){
		this->config=config;
	}
	virtual bool checkRequire(){
		puts("ERR");
		return true;
	}
	virtual void generateCommunity(string graphFile,string communityFile, int truth_NC){
		puts("ERR");
	}
};
