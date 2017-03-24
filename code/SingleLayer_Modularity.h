class SingleLayer_Modularity: public SingleLayer_Method{
public:
	bool checkRequire(){
		if (!checkRequiredFile(config["Modularity_Dir"]+"convert")) return false;
		if (!checkRequiredFile(config["Modularity_Dir"]+"community")) return false;
		if (!checkRequiredFile(config["Modularity_Dir"]+"hierarchy")) return false;
		return true;
	}
	void generateCommunity(string graphFile,string communityFile, int truth_NC){
#ifdef DEBUG
		cout<<"into Modularity SingleLayer"<<endl;
#endif
		string singleEdgeGraphFile=config["TMP_DIR"]+"singleEdgeGraph";
		string modularityGraphFile=config["TMP_DIR"]+"ModularityGraph";
		string modularityWeightsFile=modularityGraphFile+".weights";
		string modularityCommunityFile=config["TMP_DIR"]+"ModularityCommunity";
		Graph g;
		g.load(graphFile);
		if (config["WeightedGraph"]!="TRUE")
			g.sampleByWeights();
		g.saveSingleEdge(singleEdgeGraphFile);
		systemCall((config["Modularity_Dir"]+"convert -i "+singleEdgeGraphFile
			+" -o "+modularityGraphFile+" -w " +modularityWeightsFile).c_str());
		systemCall((config["Modularity_Dir"]+"community "+modularityGraphFile
			+" -l -1 -w "+modularityWeightsFile+" > "+modularityCommunityFile+" ").c_str());
		//systemCall(("cp "+modularityCommunityFile+" "+communityFile+".Modularity").c_str());
		Community comms;
		comms.loadModularity(modularityCommunityFile);
#ifdef PAUSEDEBUG
		getchar();
#endif
		comms.save(communityFile);
#ifdef PAUSEDEBUG
		getchar();
#endif
		puts("Modularity Finished");
	}
};
