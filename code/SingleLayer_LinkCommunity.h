class SingleLayer_LinkCommunity: public SingleLayer_Method{
public:
	bool checkRequire(){
		if (!checkRequiredFile(config["LinkCommunity_Dir"]+"calcAndWrite_Jaccards")) return false;
		if (!checkRequiredFile(config["LinkCommunity_Dir"]+"callCluster")) return false;
		return true;
	}
	void generateCommunity(string graphFile,string communityFile, int truth_NC){
		string LCGraphFile=config["TMP_DIR"]+"LC.graph";cout<<graphFile<<endl;
		string JaccardsFile=config["TMP_DIR"]+"LC.jaccs";
		//cout<<graphFile<<endl;
		Graph g;
		g.load(graphFile);
		if (config["WeightedGraph"]!="TRUE")
			g.sampleByWeights();
		g.savePairs(LCGraphFile);
		string LCCommunityFile=config["TMP_DIR"]+"LC.cluster";
		
		systemCall(config["LinkCommunity_Dir"]+"calcAndWrite_Jaccards "+LCGraphFile+" "+JaccardsFile);
		systemCall(config["LinkCommunity_Dir"]+"callCluster "+LCGraphFile+" "+JaccardsFile+" "+LCCommunityFile+" threshhold");

		//systemCall(config["LinkCommunity_Dir"]+"calcJaccards "+LCGraphFile+" "+JaccardsFile);
		//systemCall(config["LinkCommunity_Dir"]+"clusterJaccards "+LCGraphFile+" "+JaccardsFile+" "+LCCommunityFile+" "+config["TMP_DIR"]+"LC.stats"+" "+config["LinkCommunity_Thres"]);

		Community comms;
		comms.loadLinkCommunity(LCCommunityFile);
		comms.save(communityFile);
		
		puts("LC Finished");
		systemCall("rm -f "+config["TMP_DIR"]+"*");
	}
};
