#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <sstream>
#include <cctype>
#include <fstream>
using namespace std;

map<string,string> config;

#include "basic.h"
#include "Graph.h"
#include "Community.h"
#include "SingleLayer_Method.h"
#include "SingleLayer_Modularity.h"
#include "SingleLayer_Infomap.h"
#include "Framework.h"
#include "Framework_RemoveLinks.h"
#include "Framework_ReplaceByNoise.h"
#include "Framework_DecreaseWeights.h"
#include "Framework_AdaptiveDecreaseWeights.h"

#include "Metrics.h"

#define lowbit(x) ((x)&(-(x)))
#define sqr(x) ((x)*(x))
#define PB push_back
#define MP make_pair
#define LINESEP "###########################\n"


SingleLayer_Method *SLM;
Framework *FW1;
Framework *FW2;

void loadConfig(char* filename){
	FILE* fin=fopen(filename,"r");
	config.clear();
	char name[1000],value[1000];
	for (;fscanf(fin,"%s%s",name,value)==2;)
		config[name]=value;
	fclose(fin);
}

bool checkRequire(){
	if (!checkRequiredFile(config["DATA_DIR"]+"graph")) return false;

	if (config["SingleLayer_Method"]=="Modularity")
		SLM=new SingleLayer_Modularity();
	else if (config["SingleLayer_Method"]=="Infomap")
		SLM=new SingleLayer_Infomap();
	else {
		puts("UNKNOWN SingleLayer_Method");
		return false;
	}
	SLM->setConfig(config);
	if (!SLM->checkRequire()) return false;

	if (config["Framework"]=="RemoveLinks")
		FW1=FW2=new Framework_RemoveLinks();
	else if (config["Framework"]=="ReplaceByNoise")
		FW1=FW2=new Framework_ReplaceByNoise();
	else if (config["Framework"]=="DecreaseWeights")
		FW1=FW2=new Framework_DecreaseWeights();
	else if (config["Framework"]=="AdaptiveDecreaseWeights")
		FW1=FW2=new Framework_AdaptiveDecreaseWeights();
	else if (config["Framework"]=="FirstLayer"){
		FW1=new Framework_RemoveLinks();
		FW2=new Framework_AdaptiveDecreaseWeights();
	}
	else{
		puts("UNKNOWN Framework");
	}
	FW1->setConfig(config);
	FW2->setConfig(config);
	if (!FW1->checkRequire()) return false;
	if (!FW2->checkRequire()) return false;
	return true;
}

void generateLayerGraph(string graphFile,vector<string> otherCommFiles,string outputFile,bool isFirst){
	Graph cur;
	cur.load(graphFile);
	vector<Community> comms;
	comms.clear();
	for (int i=0;i<otherCommFiles.size();i++){
		Community comm;
		comm.load(otherCommFiles[i]);
		if (config.find("Framework_CommunitySizeThres")!=config.end())
			comm.removeSmallComms(atoi(config["Framework_CommunitySizeThres"].c_str()));
		comms.PB(comm);
	}
	Graph layerGraph;
	if (isFirst)
		layerGraph=FW2->calcLayerGraph(cur,comms);
	else{
		vector<Community> c1;c1.clear();c1.PB(comms[0]);
		for (int i=1;i<comms.size();i++) comms[i-1]=comms[i];
		comms.pop_back();
		layerGraph=FW1->calcLayerGraph(cur,c1);
		layerGraph=FW1->calcLayerGraph(layerGraph,comms);
	}
	layerGraph.save(outputFile);
}

void generateCommunity(string graphFile,string commnityFile){
	SLM->generateCommunity(graphFile,commnityFile);
}

vector<pair<string,string> > showCommunityStat(string filename){
	vector<pair<string,string> > stat;stat.clear();
	Community comm;
	comm.load(filename);
	stat.PB(MP("#Comm",int2str(comm.NC)));
	stat.PB(MP("Avg Comm Size",double2str(comm.N/(double)comm.NC)));
	stat.PB(MP("Comm Entropy",double2str(calcEntropy(comm))));
	stat.PB(MP("Coverage",double2str(1-comm.leftover.size()/(double)comm.N)));
	return stat;
}

vector<pair<string,string> > showGraphStat(string filename){
	vector<pair<string,string> > stat;stat.clear();
	Graph g;
	g.load(filename);
	double W=0;
	for (int i=0;i<g.g.size();i++) W+=g.g[i].w;
	stat.PB(MP("Graph Density",double2str(W/(double)(g.N*g.N))));
	return stat;
}

void showConfig(FILE* fout,FILE* fout_tex){
	vector<pair<string,string> > data;
	data.clear();
	for (map<string,string>::iterator itr=config.begin();itr!=config.end();itr++)
		data.PB(MP(itr->first,itr->second));
	Tex_Table(VSS2VVS(data),"Experiment Setting",fout_tex);
}

vector<string> truth;
vector<pair<string,TVVS> > iterationDetail;
map<string,vector<string> > iterativeStat;
vector<string> iterativeStatNames;
vector<bool> iterativeStatNames_graphic;

int nLayer;

bool isLayer(string layerName){
	return layerName.substr(0,5)=="Layer";
}

void autoShrinkLayer(){
	int cnt=1;
	for (int i=1;i<=nLayer;i++){
		double ma=0;
		for (int z=1;z<cnt;z++)
			ma=max(ma,calcNMI(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen",
				config["RESULT_DIR"]+"Layer"+int2str(z)+".gen"));
		if (ma<str2double(config["ShrinkLayerThres"])){
			systemCall("cp "+config["RESULT_DIR"]+"Layer"+int2str(i)+".gen "
				+config["RESULT_DIR"]+"Layer"+int2str(cnt)+".gen");
			cnt++;
		}
	}
	nLayer=cnt-1;
}

pair<string,TVVS> analyze(string name){

	vector<pair<string,string> > comms;

	comms.clear();
	for (int i=0;i<truth.size();i++)
		comms.PB(MP(truth[i],config["DATA_DIR"]+truth[i]));
	for (int i=1;i<=nLayer;i++)
		comms.PB(MP("Layer"+int2str(i),config["RESULT_DIR"]+"Layer"+int2str(i)+".gen"));

	vector<vector<string> > result;
	result.clear();

	vector<double> _modOriG;_modOriG.clear();
	vector<double> _modLayerG;_modLayerG.clear();
	vector<double> _NMICL;_NMICL.clear();
	vector<double> _NMIPL;_NMIPL.clear();
	double score;

	for (int i=0;i<comms.size();i++){
		vector<string> row;
		row.clear();
		row.PB(comms[i].first);
		vector<pair<string,string> > res=showCommunityStat(comms[i].second);
		//vs Previous Layer
		if (checkFileExist(comms[i].second+".last")){
			res.PB(MP("NMI(Last Iteration)",
				double2str(score=calcNMI(comms[i].second,comms[i].second+".last"))));
			_NMIPL.PB(score);
		}
		else res.PB(MP("NMI(Last Iteration)",double2str(log(0))));
		//Comm VS Graph
		string originalGraphFile=config["DATA_DIR"]+"graph";
		string layerGraphFile=config["RESULT_DIR"]+comms[i].first+".graph";

		res.PB(MP("Modularity(Original)",
			double2str(score=calcModularity(originalGraphFile,comms[i].second))));
		if (isLayer(comms[i].first))
			_modOriG.PB(score);

		res.PB(MP("Modularity(Layer Graph)",
			double2str(score=calcModularity(layerGraphFile,comms[i].second))));
		if (isLayer(comms[i].first))
			_modLayerG.PB(score);

		//Comm VS Comm
		for (int j=0;j<comms.size();j++){
			res.PB(MP("NMI("+comms[j].first+")",
				double2str(score=calcNMI(comms[i].second,comms[j].second))));
			if (j>i&&isLayer(comms[i].first)&&isLayer(comms[j].first))
				_NMICL.PB(score);
		}

		if (i==0){
			vector<string> title;
			title.clear();
			title.PB("");
			for (int z=0;z<res.size();z++)
				title.PB(res[z].first);
			result.PB(title);
		}
		for (int z=0;z<res.size();z++)
			row.PB(res[z].second);
		result.PB(row);


	}
	iterativeStat["Name"].PB(name);
	iterativeStat["AvgModOriG"].PB(double2str(getAverage(_modOriG)));
	iterativeStat["AvgModLayerG"].PB(double2str(getAverage(_modLayerG)));
	iterativeStat["FirstLayerMod"].PB(double2str(_modOriG[0]));
	iterativeStat["AvgNMICrossLayer"].PB(double2str(getAverage(_NMICL)));
	iterativeStat["AvgNMIvsPrevItr"].PB(double2str(getAverage(_NMIPL)));
	return MP(name,transVVS(result));
}

void generateGraphic(){
	FILE* fout=fopen("graphicData","w");
	TVVS data;
	data.clear();
	for (int i=0;i<iterativeStatNames.size();i++)
		if (iterativeStatNames_graphic[i])
			data.PB(iterativeStat[iterativeStatNames[i]]);
	data=transVVS(data);
	for (int i=0;i<data.size();i++){
		for (int j=0;j<data[i].size();j++){
			if (j) fprintf(fout,",");
			fprintf(fout,"%s",data[i][j].c_str());
		}
		fprintf(fout,"\n");
	}
	fclose(fout);
	systemCall("r -f tools/graphic/genGraphics.R");
}

void showAnalyze(){
	FILE* fout=fopen((config["RESULT_DIR"]+"analyze").c_str(),"w");
	FILE* fout_tex=fopen((config["RESULT_DIR"]+"analyze.tex").c_str(),"w");
	fprintf(fout_tex,"\\documentclass[12pt]{article}\n");
	fprintf(fout_tex,"\\usepackage{graphicx}");
	fprintf(fout_tex,"\\usepackage[papersize={30cm,50cm}]{geometry}");
	fprintf(fout_tex,"\\begin{document}\n");

	showConfig(fout,fout_tex);

	TVVS expSummary;
	expSummary.clear();
	for (int i=0;i<iterativeStatNames.size();i++)
		expSummary.PB(iterativeStat[iterativeStatNames[i]]);

	//Tex_Table(transVVS(expSummary),"Experiment Summary",fout_tex);

	generateGraphic();

	for (int i=0;i<iterativeStatNames.size();i++)
		if (iterativeStatNames_graphic[i]){
			Tex_includeGraphics(iterativeStatNames[i]+".pdf",
				iterativeStatNames[i],5,fout_tex);
			fprintf(fout,"%s\n",iterativeStatNames[i].c_str());
			for (int z=0;z<iterativeStat[iterativeStatNames[i]].size();z++)
				fprintf(fout,"%s\t",iterativeStat[iterativeStatNames[i]][z].c_str());
			fprintf(fout,"\n");
		}

	int cnt=0;
	for (int i=0;i<iterationDetail.size();i++)
		if (i%5==0){
			cnt++;
			if (cnt%5==0) fprintf(fout_tex,"\\clearpage\n");
			Tex_Table(iterationDetail[i].second,iterationDetail[i].first,fout_tex);
		}

	fprintf(fout_tex,"\\end{document}\n");

	fclose(fout);
	fclose(fout_tex);

	systemCall("cat "+config["RESULT_DIR"]+"analyze");
	systemCall("xelatex "+config["RESULT_DIR"]+"analyze.tex");
	systemCall("cp analyze.pdf "+config["RESULT_DIR"]+"analyze.pdf");
}

int main(int argc,char** argv){
	if (argc!=2) {
		puts("Argument Error");
		return 0;
	}
	loadConfig(argv[1]);
	config["TMP_DIR"]=config["RESULT_DIR"]+"tmp/";

	systemCall(("rm -f -r "+config["RESULT_DIR"]+"*").c_str());

	systemCall("mkdir "+config["TMP_DIR"]);

	truth.clear();
	char s[10000];
	sprintf(s,"%s",config["Ground_Truth"].c_str());
	for (char* p=strtok(s,";");p;p=strtok(NULL,";"))
		truth.PB(p);

	if (!checkRequire()) return 0;

	nLayer=atoi(config["Number_Of_Layers"].c_str());

	iterationDetail.clear();

	iterativeStat.clear();
	iterativeStatNames.clear();
	iterativeStatNames_graphic.clear();
	iterativeStatNames.PB("Name");iterativeStatNames_graphic.PB(false);
	iterativeStatNames.PB("AvgModOriG");iterativeStatNames_graphic.PB(true);
	iterativeStatNames.PB("AvgModLayerG");iterativeStatNames_graphic.PB(true);
	iterativeStatNames.PB("FirstLayerMod");iterativeStatNames_graphic.PB(true);
	iterativeStatNames.PB("AvgNMICrossLayer");iterativeStatNames_graphic.PB(true);
	iterativeStatNames.PB("AvgNMIvsPrevItr");iterativeStatNames_graphic.PB(true);
	for (int i=0;i<iterativeStatNames.size();i++){
		iterativeStat[iterativeStatNames[i]]=vector<string>(0);
		iterativeStat[iterativeStatNames[i]].PB(iterativeStatNames[i]);
	}

	systemCall(("cp "+config["DATA_DIR"]+"graph "+config["RESULT_DIR"]+"Layer1.graph").c_str());
	
	for (int i=1;i<=nLayer;i++){
		if (i!=1){
			vector<string> otherCommFiles;
			for (int j=1;j<i;j++)
				if (i!=j)
					otherCommFiles.PB(config["RESULT_DIR"]+"Layer"+int2str(j)+".gen");
			generateLayerGraph(config["DATA_DIR"]+"graph",otherCommFiles
				,config["RESULT_DIR"]+"Layer"+int2str(i)+".graph",i==1);
		}

		generateCommunity(config["RESULT_DIR"]+"Layer"+int2str(i)+".graph"
			,config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");

		if (config.find("Save_CommunitySizeThres")!=config.end()){
			Community comm;
			comm.load(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
			comm.removeSmallComms(atoi(config["Save_CommunitySizeThres"].c_str()));
			comm.save(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
		}
	}

	iterationDetail.PB(analyze("Initialize Iteration"));

	for (int r=1;r<=atoi(config["Number_Of_Iteration"].c_str());r++){
		fprintf(stderr,"####### Running Iteration %d with %d Layers #########\n",r,nLayer);
		for (int i=1;i<=nLayer;i++)
			systemCall(("cp "+config["RESULT_DIR"]+"Layer"+int2str(i)+".gen "
				+config["RESULT_DIR"]+"Layer"+int2str(i)+".gen.last").c_str());
		vector<int> order;
		order.clear();
		for (int i=1;i<=nLayer;i++) order.PB(i);
		//random_shuffle(order.begin(),order.end());
		for (int z=0;z<order.size();z++){
			int i=order[z];
			vector<string> otherCommFiles;
			for (int j=0;j<order.size();j++)
				if (i!=order[j])
					otherCommFiles.PB(config["RESULT_DIR"]+"Layer"+int2str(order[j])+".gen");
			generateLayerGraph(config["DATA_DIR"]+"graph",otherCommFiles
				,config["RESULT_DIR"]+"Layer"+int2str(i)+".graph",i==1);

			generateCommunity(config["RESULT_DIR"]+"Layer"+int2str(i)+".graph"
				,config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");

			if (config.find("Save_CommunitySizeThres")!=config.end()){
				Community comm;
				comm.load(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
				comm.removeSmallComms(atoi(config["Save_CommunitySizeThres"].c_str()));
				comm.save(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
			}
		}
		autoShrinkLayer();
		iterationDetail.PB(analyze("Iteration "+int2str(r)));
	}

	showAnalyze();

	systemCall("rm -f -r "+config["TMP_DIR"]);
	systemCall("mkdir "+config["RESULT_DIR"]+"FrameworkResults");
	systemCall("cp "+config["RESULT_DIR"]+"*.gen "+config["RESULT_DIR"]+"FrameworkResults/");
	systemCall("cp "+config["RESULT_DIR"]+"*.graph "+config["RESULT_DIR"]+"FrameworkResults/");
	systemCall("mkdir "+config["RESULT_DIR"]+"Data");
	systemCall("cp "+config["DATA_DIR"]+"graph "+config["RESULT_DIR"]+"Data/");
	for (int i=0;i<truth.size();i++)
		systemCall("cp "+config["DATA_DIR"]+truth[i]+" "+config["RESULT_DIR"]+"Data/");
	puts("Finished");
	systemCall("rm -f "+config["RESULT_DIR"]+"analyze.tex");
	systemCall("rm -f "+config["RESULT_DIR"]+"Layer*");
	systemCall("mkdir "+config["RESULT_DIR"]+"BasicAlgs");
	
	return 0;
}