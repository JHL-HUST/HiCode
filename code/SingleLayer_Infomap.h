class SingleLayer_Infomap: public SingleLayer_Method{
public:
	bool checkRequire(){
		if (!checkRequiredFile(config["Infomap_Dir"]+"Infomap")) return false;
		return true;
	}
	void generateCommunity(string graphFile, string communityFile, int truth_NC){
		string infomapGraphFile=config["TMP_DIR"]+"InfomapGraph";
		string infomapCommunityDir=config["TMP_DIR"];
		string infomapCommunityFile=config["TMP_DIR"]+"InfomapGraph.clu";

		Graph g;
		g.load(graphFile);
		g.saveSingleEdge(infomapGraphFile);

		string tags=" -i link-list --clu -s "+int2str(rand());
		systemCall((config["Infomap_Dir"]+"Infomap "
			+infomapGraphFile+" "+infomapCommunityDir+tags).c_str());
		
		Community comms;
		comms.loadInfomap(infomapCommunityFile);
		comms.save(communityFile);
	}
};
