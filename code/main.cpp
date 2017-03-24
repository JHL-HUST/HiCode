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
//#include <ctime>
#include <sys/time.h>
#include <unistd.h>
//#include <assert.h>
using namespace std;

map<string,string> config;

#define overlap //using overlap headfiles
//#define DEBUG

#include "basic.h"
#include "Graph.h"
#include "Community.h"
#include "SingleLayer_Method.h"
#include "SingleLayer_Modularity.h"
#include "SingleLayer_Infomap.h"
#include "SingleLayer_OSLOM.h"
#include "SingleLayer_LinkCommunity.h"
#include "Framework.h"
#include "Framework_Remove.h"
#include "Framework_ReducePP.h"


#ifdef overlap
#include "Metrics_Overlapping.h"
#else
#include "Metrics.h"
#endif // overlap
#define lowbit(x) ((x)&(-(x)))
#define sqr(x) ((x)*(x))
#define PB push_back
#define MP make_pair
#define LINESEP "###########################\n"


SingleLayer_Method *SLM,*SLM_Modularity;
Framework *FW1;
Framework *FW2;

void loadConfig(char* filename)
{
    FILE* fin=fopen(filename,"r");
    config.clear();
    char name[1000],value[1000];
    for (; fscanf(fin,"%s%s",name,value)==2;)
        config[name]=value;
    fclose(fin);
}

SingleLayer_Method* getSingleLayerMethod(string name)
{
    if (name=="Modularity")
        return new SingleLayer_Modularity();
    if (name=="Infomap")
        return new SingleLayer_Infomap();
    if (name=="OSLOM")
        return new SingleLayer_OSLOM();
    if (name=="LinkCommunity")
        return new SingleLayer_LinkCommunity();
    return new SingleLayer_Method();
}

bool checkRequire()
{
    if (!checkRequiredFile(config["DATA_DIR"]+"graph")) return false;
    //cout<<config["SingleLayer_Method"]<<":here"<<endl;
    SLM_Modularity=new SingleLayer_Modularity();
    SLM_Modularity->setConfig(config);

    if (config["SingleLayer_Method"]=="Modularity")
        SLM=new SingleLayer_Modularity();
    else if (config["SingleLayer_Method"]=="Infomap")
        SLM=new SingleLayer_Infomap();
    else if (config["SingleLayer_Method"]=="LinkCommunity")
        SLM=new SingleLayer_LinkCommunity();
    else if (config["SingleLayer_Method"]=="OSLOM")
        SLM=new SingleLayer_OSLOM();
    else
    {
        puts("UNKNOWN SingleLayer_Method");
        return false;
    }

    SLM->setConfig(config);
    if (!SLM->checkRequire()) return false;

    if (config["Framework"]=="Remove")
        FW1=FW2=new Framework_Remove();
    else if (config["Framework"]=="Reduce++")
        FW1=FW2=new Framework_ReducePP();
    else
    {
        puts("UNKNOWN Framework");
    }
    FW1->setConfig(config);
    FW2->setConfig(config);
    if (!FW1->checkRequire()) return false;
    if (!FW2->checkRequire()) return false;
    return true;
}


void generateLayerGraph(string graphFile,vector<string> otherCommFiles,string outputFile,bool isFirst)
{
    Graph cur;
    cout<<graphFile<<endl;
    if (!checkFileExist(graphFile))
        cout<<graphFile<<" not exist"<<endl;
    cur.load(graphFile);
    vector<Community> comms;
    comms.clear();
    for (int i=0; i<otherCommFiles.size(); i++)
    {
        Community comm;
        cout<<i<<" : "<<otherCommFiles[i]<<endl;
        comm.load(otherCommFiles[i]);
        if (config.find("Framework_CommunitySizeThres")!=config.end())
            comm.removeSmallComms(atoi(config["Framework_CommunitySizeThres"].c_str()));
        comms.PB(comm);
    }
    Graph layerGraph;
    if (isFirst)
    {
        cout<<"isFirst"<<endl;
        layerGraph=FW2->calcLayerGraph(cur,comms);
    }
    else
    {
        vector<Community> c1;
        c1.clear();
        c1.PB(comms[0]);
        for (int i=1; i<comms.size(); i++) comms[i-1]=comms[i];
        comms.pop_back();
        cout<<"notFirst 1"<<endl;
        layerGraph=FW1->calcLayerGraph(cur,c1);
        cout<<"notFirst 2"<<endl;
        cout<<"comms.size() = "<<comms.size();
        layerGraph=FW1->calcLayerGraph(layerGraph,comms);
        cout<<"notFirst 3"<<endl;
    }
    layerGraph.save(outputFile);
}

void generateLayerGraphAll(string graphFile,vector<string> otherCommFiles,string outputFile,bool isFirst)
{
    Graph cur;
    cur.load(graphFile);
    vector<Community> comms;
    comms.clear();
    for (int i=0; i<otherCommFiles.size(); i++)
    {
        Community comm;
        comm.load(otherCommFiles[i]);
        if (config.find("Framework_CommunitySizeThres")!=config.end())
            comm.removeSmallComms(atoi(config["Framework_CommunitySizeThres"].c_str()));
        comms.PB(comm);
    }
    Graph layerGraph;
    if (isFirst)
        layerGraph=FW2->calcLayerGraphAll(cur,comms);
    else
    {
        //vector<Community> c1;c1.clear();c1.PB(comms[0]);
        //for (int i=1;i<comms.size();i++) comms[i-1]=comms[i];
        //comms.pop_back();
        //layerGraph=FW1->calcLayerGraphAll(cur,c1);
        layerGraph=FW2->calcLayerGraphAll(cur,comms);
    }
    layerGraph.save(outputFile);
}


void generateCommunity(string graphFile,string communityFile,int layerid, vector<int> Truth_NC)
{
//Truth_NC[0] = -1, Truth_NC[i] = truth[i-1].NC, 1<=i<truth.size() //if there is k truths, Truth_NC.size() = k+1 and truth.size() = k
//layerid [1, nlayer]
    /*if (layerid==1)
    	SLM_Modularity->generateCommunity(graphFile,commnityFile, Truth_NC);
    else */
    cout<<graphFile<<" "<<communityFile<<endl;
    int truth_NC;
    if (layerid < Truth_NC.size())
        truth_NC = Truth_NC[layerid];
    else
    {
        /*int randid = ceil(getRand(0, Truth_NC.size()-1));
        cout<<"random choosing similar truth : "<<randid<<endl;
        truth_NC = Truth_NC[randid];*/
        vector<int>::iterator result = max_element(Truth_NC.begin(), Truth_NC.end());
        int truth_NC_idx = distance(Truth_NC.begin(), result);
        truth_NC = Truth_NC[truth_NC_idx];
        //getchar();
    }
    cout<<"truth Number of C = "<<truth_NC<<endl;
    SLM->generateCommunity(graphFile, communityFile, truth_NC);
}

vector<pair<string,string> > showCommunityStat(string filename)
{
    vector<pair<string,string> > stat;
    stat.clear();
    Community comms;
    comms.load(filename);
    stat.PB(MP("#Comm",int2str(comms.NC)));
    stat.PB(MP("Avg Comm Size",double2str(comms.N/(double)comms.NC)));
    stat.PB(MP("Comm Entropy",double2str(calcEntropy(comms))));
    vector<vector<int > > cid=comms.getPartitionIDOver();
    double counter = 0, OM_counter=0;
    for (int cid_i = 1; cid_i < cid.size(); cid_i++)
    {
        if (cid[cid_i].size() != 0)
        {
            counter++;
            OM_counter+=cid[cid_i].size();
        }
    }
    stat.PB(MP("Coverage",double2str(counter/(double)comms.N)));
    stat.PB(MP("OM",double2str(OM_counter/(double)comms.N)));
    return stat;
}

vector<pair<string,string> > showGraphStat(string filename)
{
    vector<pair<string,string> > stat;
    stat.clear();
    Graph g;
    g.load(filename);
    double W=0;
    for (int i=0; i<g.g.size(); i++) W+=g.g[i].w;
    stat.PB(MP("Graph Density",double2str(W/(double)(g.N*g.N))));
    return stat;
}

void showConfig(FILE* fout,FILE* fout_tex)
{
    vector<pair<string,string> > data;
    data.clear();
    for (map<string,string>::iterator itr=config.begin(); itr!=config.end(); itr++)
        data.PB(MP(itr->first,itr->second));
    Tex_Table(VSS2VVS(data),"Experiment Setting",fout_tex);
}

vector<string> truth,frameworks;
vector<pair<string,TVVS> > iterationDetail;
map<string,vector<string> > iterativeStat;
vector<string> iterativeStatNames;
vector<bool> iterativeStatNames_graphic;

//for save max origal and layer
double maxmodOriG = 0;
double maxmodLayerG = 0;
double tmpmodOriG = 0;
double tmpmodLayerG = 0;
int maxOriIter = 0;
int maxLayIter = 0;

int nLayer;

bool isLayer(string layerName)
{
    return layerName.substr(0,5)=="Layer";
}

void autoShrinkLayer()//自动确定层数的策略，已经废弃
{
    int cnt=1;
    for (int i=1; i<=nLayer; i++)
    {
        double ma=0;
        for (int z=1; z<cnt; z++)//cnt=1???????
            ma=max(ma,calcNMI(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen",
                              config["RESULT_DIR"]+"Layer"+int2str(z)+".gen"));
        if (ma<str2double(config["ShrinkLayerThres"]))
        {
            systemCall("cp "+config["RESULT_DIR"]+"Layer"+int2str(i)+".gen "
                       +config["RESULT_DIR"]+"Layer"+int2str(cnt)+".gen");
            cnt++;
        }
    }
    nLayer=cnt-1;
}

vector< vector<double> > preVScur;//for draw the NMI between the current and previous

pair<string,TVVS> analyze(string name)
{
    cout<<"analyze begin"<<endl;
    vector<pair<string,string> > comms;
    comms.clear();
    for (int i=0; i<truth.size(); i++)
        comms.PB(MP(truth[i],config["DATA_DIR"]+truth[i]));
    cout<<"comes 1"<<endl;
    for (int i=1; i<=nLayer; i++)
        comms.PB(MP("Layer"+int2str(i),config["RESULT_DIR"]+"Layer"+int2str(i)+".gen"));
    cout<<"comes 2"<<endl;
    vector<vector<string> > result;
    result.clear();

    vector<double> _modOriG;
    _modOriG.clear();
    vector<double> _modLayerG;
    _modLayerG.clear();
    vector<double> _NMICL;
    _NMICL.clear();
    vector<double> _NMIPL;
    _NMIPL.clear();
    double score;

    for (int i=0; i<comms.size(); i++)
    {
        cout<<"comes for start "<<int2str(i)<<endl;
        vector<string> row;
        row.clear();
        row.PB(comms[i].first);
        vector<pair<string,string> > res=showCommunityStat(comms[i].second);
        //vs Previous Layer
        if (checkFileExist(comms[i].second+".last"))
        {
            res.PB(MP("NMI(Last Iteration)",
                      double2str(score=calcNMI(comms[i].second,comms[i].second+".last"))));
            _NMIPL.PB(score);
        }

        else res.PB(MP("NMI(Last Iteration)",double2str(log(0))));
        //getchar();
        //Comm VS Graph
        string originalGraphFile=config["DATA_DIR"]+"graph";
        string layerGraphFile=config["RESULT_DIR"]+comms[i].first+".graph";
        //cout<<"main3 analyze here"<<endl;
        //cout<<"comm file:"<<comms[i].second<<endl;
        res.PB(MP("Modularity(Original)",
                  double2str(score=calcModularity(originalGraphFile,comms[i].second))));
        //cout<<"main2 analyze here"<<endl;
        if (isLayer(comms[i].first))
            _modOriG.PB(score);
        //cout<<"main3 analyze here"<<endl;
        res.PB(MP("Modularity(Layer Graph)",
                  double2str(score=calcModularity(layerGraphFile,comms[i].second))));

        if (isLayer(comms[i].first))
            _modLayerG.PB(score);
        //Comm VS Comm

        for (int j=0; j<comms.size(); j++)
        {
            res.PB(MP("NMI("+comms[j].first+")",
                      double2str(score=calcNMI(comms[i].second, comms[j].second))));
            //cout<<" Overlapping NMI = "<<score<<endl;
            if (j>i&&isLayer(comms[i].first)&&isLayer(comms[j].first))
                _NMICL.PB(score);
        }

        if (i==0)
        {
            vector<string> title;
            title.clear();
            title.PB("");
            for (int z=0; z<res.size(); z++)
                title.PB(res[z].first);
            result.PB(title);
        }
        for (int z=0; z<res.size(); z++)
            row.PB(res[z].second);
        result.PB(row);
        cout<<"comes for end "<<int2str(i)<<endl;
    }

    //for save max
    cout<<"PB start"<<endl;
    tmpmodOriG = getAverage(_modOriG);
    tmpmodLayerG = getAverage(_modLayerG);
    preVScur.PB(_NMIPL);
    string tmpstr;
    iterativeStat["Name"].PB(name);
    for (int index = 0; index < nLayer; index++)
    {
        tmpstr = "Mod_Layer"+int2str(index+1)+"_in_"+"OriG";
        iterativeStat[tmpstr].PB(double2str(_modOriG[index]));
    }
    for (int index = 0; index < nLayer; index++)
    {
        tmpstr = "Mod_Layer"+int2str(index+1)+"_in_"+"LayerG";
        iterativeStat[tmpstr].PB(double2str(_modLayerG[index]));
    }
    for (int index = 0; index < nLayer; index++)
    {
        tmpstr = "NMI_Layer"+int2str(index+1)+"_vs_"+"inPreIter";
        if (_NMIPL.size() == 0)
            iterativeStat[tmpstr].PB(double2str(getAverage(_NMIPL)));
        else
            iterativeStat[tmpstr].PB(double2str(_NMIPL[index]));
    }
    iterativeStat["AvgMod_Layer_in_OriG"].PB(double2str(getAverage(_modOriG)));
    iterativeStat["AvgMod_Layer_in_LayerG"].PB(double2str(getAverage(_modLayerG)));
    iterativeStat["AvgNMI_Iter_vs_PrevIter"].PB(double2str(getAverage(_NMIPL)));
    iterativeStat["AvgNMI_CrossLayer"].PB(double2str(getAverage(_NMICL)));
    cout<<"PB end"<<endl;
    return MP(name,transVVS(result));
}

void generateGraphic()
{
    FILE* fout=fopen("graphicData","w");
    TVVS data;
    data.clear();
    for (int i=0; i<iterativeStatNames.size(); i++)
        if (iterativeStatNames_graphic[i])
        {
            data.PB(iterativeStat[iterativeStatNames[i]]);
        }
    cout<<"VVS start"<<endl;
    data=transVVS(data);
    cout<<"VVS stop"<<endl;
    for (int i=0; i<data.size(); i++)
    {
        for (int j=0; j<data[i].size(); j++)
        {
            if (j) fprintf(fout,",");
            fprintf(fout,"%s",data[i][j].c_str());
        }
        fprintf(fout,"\n");
    }
    fclose(fout);
    systemCall("Rscript tools/graphic/genGraphics.R");//systemCall("Rscript tools/graphic/genGraphics.R");
}

void showAnalyze()
{
    FILE* fout=fopen((config["RESULT_DIR"]+"analyze").c_str(),"w");
    FILE* fout_tex=fopen((config["RESULT_DIR"]+"analyze.tex").c_str(),"w");
    fprintf(fout_tex,"\\documentclass[4pt]{article}\n");
    fprintf(fout_tex,"\\usepackage{graphicx}");
    fprintf(fout_tex,"\\usepackage[papersize={30cm,50cm}]{geometry}");
    fprintf(fout_tex,"\\begin{document}\n");

    showConfig(fout,fout_tex);

    TVVS expSummary;
    expSummary.clear();
    cout<<iterativeStatNames.size()<<endl;
    for (int i=0; i<iterativeStatNames.size(); i++)
    {
        cout<<iterativeStatNames[i]<<endl;
        expSummary.PB(iterativeStat[iterativeStatNames[i]]);
    }

    //Tex_Table(transVVS(expSummary),"Experiment Summary",fout_tex);

    generateGraphic();
    int includeG_i=0;
    for (int i=0; i<iterativeStatNames.size(); i++)
        if (iterativeStatNames_graphic[i])
        {

            Tex_includeGraphics(iterativeStatNames[i]+".pdf",
                                iterativeStatNames[i],5,fout_tex);
            fprintf(fout,"%s\n",iterativeStatNames[i].c_str());
            for (int z=0; z<iterativeStat[iterativeStatNames[i]].size(); z++)
                fprintf(fout,"%s\t",iterativeStat[iterativeStatNames[i]][z].c_str());
            fprintf(fout,"\n");
            includeG_i++;
            if (includeG_i % 3 == 0)
                fprintf(fout_tex,"\\clearpage\n");
        }

    int layers = atoi(config["Number_Of_Layers"].c_str());
    for(int i=1; i<=layers; ++i)
    {
        string str = "layer"+int2str(i)+"_vs_pre";
        Tex_includeGraphics(str+".pdf",
                            str.c_str(),5,fout_tex);
    }
    for(int i=1; i<=layers; ++i)
    {
        for (int t=1; t<=truth.size(); t++)
        {
            string str2 = "truth"+int2str(t)+"_vs_layer"+int2str(i);
            //string str2 = truth[t-1]+"_vs_layer"+int2str(i);
            Tex_includeGraphics(str2+".pdf", str2.c_str(),5,fout_tex);
        }
        fprintf(fout_tex,"\\clearpage\n");
    }

    //Tex_includeGraphics("deptconnectgen_vs_layer1.pdf", "deptconnect.gen_vs_layer1.pdf",5,fout_tex);

    int cnt=0;
    for (int i=0; i<iterationDetail.size(); i++)
    {
        if (i%1==0)
        {
            cnt++;
            if (cnt%5==0) fprintf(fout_tex,"\\clearpage\n");
            Tex_Table(iterationDetail[i].second,iterationDetail[i].first,fout_tex);
        }
    }
    Tex_Table(iterationDetail[maxOriIter].second,iterationDetail[maxOriIter].first,fout_tex);//maxOriIter
    Tex_Table(iterationDetail[maxLayIter].second,iterationDetail[maxLayIter].first,fout_tex);//maxLayIter
    fprintf(fout_tex,"\\end{document}\n");

    fclose(fout);
    fclose(fout_tex);

    systemCall("cat "+config["RESULT_DIR"]+"analyze");
    systemCall("xelatex "+config["RESULT_DIR"]+"analyze.tex");
    systemCall("cp analyze.pdf "+config["RESULT_DIR"]+"analyze.pdf");
}

void PrintCurAndPre()
{
    FILE *fout=fopen("Cur_vs_Pre", "w");
    int layers = atoi(config["Number_Of_Layers"].c_str());
    for(int i=0; i<layers; ++i)
    {
        string str = "layer"+int2str(i+1)+"_vs_pre";
        if(i!=layers-1)
            str += ",";
        else
            str += "\n";
        fprintf(fout, str.c_str());
    }
    //cout<<"total: "<<preVScur.size()<<endl;
    for (int i=0; i<preVScur.size(); i++)
    {
        for (int j=0; j<preVScur[i].size(); j++)
        {
            if (j!=preVScur[i].size()-1) fprintf(fout,"%f,",preVScur[i][j]);
            else fprintf(fout,"%f",preVScur[i][j]);
            //cout<<" llss ";
        }
        fprintf(fout,"\n");
    }
    fclose(fout);
    //cout<<"here"<<endl;
    systemCall("Rscript tools/graphic/genCurVsPre.R");//systemCall("Rscript tools/graphic/genCurVsPre.R");
}

void PrintTruthAndLayer()
{
    FILE* fout=fopen("Truth_vs_layer","w");
    int cnt=0;
    TVVS data;
    data.clear();
    int layers = atoi(config["Number_Of_Layers"].c_str());
    for(int k=0; k<layers; k++)
    {
        for (int t=1; t<=truth.size(); t++)
        {
            vector<string>  vstmp;
            string str = "truth"+int2str(t)+"_vs_layer"+int2str(k+1);
            //cout<<truth[t-1]<<endl;
            vstmp.PB(str);
            //int index1=1;
            //double in1 = 0;
            //vector<vector<string> > tmp = iterationDetail[iterationDetail.size()-1].second;
            //找层数与groundtruth最像的；
            //for(int t=1; t<=truth.size(); ++t)
            //    if(str2double(tmp[9+k+truth.size()][t]) > in1)
            //    {
            //        in1 = str2double(tmp[9+k+truth.size()][t]);
            //        index1 = t;
            //    }
            //puts("hereim1");

            for (int i=0; i<iterationDetail.size(); i++)//what the hell it is??????????
            {
                vector<vector<string> > table = iterationDetail[i].second;
                //puts("hereim2");
                vstmp.PB(table[9+k+truth.size()][t].c_str());//if you want to add table elements, do not forget to change the number 9.
                //puts("hereim3");
                //if(i!=iterationDetail.size()-1)
                // fprintf(fout,",");
            }
            //fprintf(fout,"\n");
            data.PB(vstmp);
            //puts("hereim4");
        }
    }
    data=transVVS(data);
    for (int i=0; i<data.size(); i++)
    {
        for (int j=0; j<data[i].size(); j++)
        {
            if (j) fprintf(fout,",");
            fprintf(fout,"%s",data[i][j].c_str());
        }
        fprintf(fout,"\n");
    }
    //puts("hereim5");
    fclose(fout);
    systemCall("Rscript tools/graphic/genTruth.R");//systemCall("Rscript tools/graphic/genCurVsPre.R");
}

int main(int argc,char** argv)
{
    if (argc!=2)
    {
        puts("Argument Error");
        return 0;
    }
    loadConfig(argv[1]);
    config["TMP_DIR"]=config["RESULT_DIR"]+"tmp/";

    systemCall("mkdir -p "+config["RESULT_DIR"]+"Iteration/");//new add
    systemCall(("rm -f -r "+config["RESULT_DIR"]+"*").c_str());
    systemCall("mkdir -p "+config["TMP_DIR"]);

    truth=split(config["Ground_Truth"]);
    frameworks=split(config["Frameworks"]);


    systemCall("mkdir -p "+config["RESULT_DIR"]+"BasicAlgs");
    systemCall("mkdir -p "+config["RESULT_DIR"]+"Time");
    //for save max
    systemCall("mkdir -p "+config["RESULT_DIR"]+"maxOriginal");
    systemCall("mkdir -p "+config["RESULT_DIR"]+"maxLayer");

    //read NC in truths
    vector<int> Truth_NC;
    Truth_NC.PB(-1);//Truth_NC[0]=-1, Truth_NC[i]=truth[i-1].NC, 1<=i<=truth.size()
#ifdef DEBUG
    cout<<"I'm here"<<endl;
#endif
    for (int truth_i = 0; truth_i < truth.size(); truth_i++)
    {
        Community truth_com;
        truth_com.load(config["DATA_DIR"]+truth[truth_i]);
        cout<<truth[truth_i]<<" NC: "<<truth_com.NC<<endl;
        Truth_NC.PB(truth_com.NC);
    }
    //time_t start, end;
    struct timeval start,end;

    double span = 0.0;


    //systemCall("cp "+config["RESULT_DIR"]+"Basictime.txt "+config["RESULT_DIR"]+"Time/");
    //for (;;);
	
    nLayer=atoi(config["Number_Of_Layers"].c_str());

    systemCall("touch " + config["RESULT_DIR"]+"Time/" + config["SingleLayer_Method"]+"_Frameworktime.txt");
    ofstream outfile2;
    outfile2.open((config["RESULT_DIR"]+"Time/" + config["SingleLayer_Method"]+"_Frameworktime.txt").c_str(), ios::out);

    for (int nf=0; nf<frameworks.size(); nf++)
    {
        cout<<"running time of algorithm "+frameworks[nf]<<endl;
        outfile2<<"running time of "+frameworks[nf] +" in seconds:"<<endl;
        //start = time(NULL);
        gettimeofday(&start,NULL);
        config["Framework"]=frameworks[nf];
        if (!checkRequire()) continue;
        iterationDetail.clear();
        iterativeStat.clear();
        iterativeStatNames.clear();
        iterativeStatNames_graphic.clear();
        iterativeStatNames.PB("Name");
        iterativeStatNames_graphic.PB(false);
        iterativeStatNames.PB("AvgMod_Layer_in_OriG");
        iterativeStatNames_graphic.PB(true);
        iterativeStatNames.PB("AvgMod_Layer_in_LayerG");
        iterativeStatNames_graphic.PB(true);
        iterativeStatNames.PB("AvgNMI_CrossLayer");
        iterativeStatNames_graphic.PB(true);
        iterativeStatNames.PB("AvgNMI_Iter_vs_PrevIter");
        iterativeStatNames_graphic.PB(true);
        string tmpstr;
        for (int i=1; i<=nLayer; i++)
        {
            tmpstr = "NMI_Layer"+int2str(i)+"_vs_"+"inPreIter";
            iterativeStatNames.PB(tmpstr);
            iterativeStatNames_graphic.PB(true);
            tmpstr = "Mod_Layer"+int2str(i)+"_in_"+"LayerG";
            iterativeStatNames.PB(tmpstr);
            iterativeStatNames_graphic.PB(true);
            tmpstr = "Mod_Layer"+int2str(i)+"_in_"+"OriG";
            iterativeStatNames.PB(tmpstr);
            iterativeStatNames_graphic.PB(true);
        }
        for (int i=0; i<iterativeStatNames.size(); i++)
        {
            iterativeStat[iterativeStatNames[i]]=vector<string>(0);
            iterativeStat[iterativeStatNames[i]].PB(iterativeStatNames[i]);
        }
        systemCall(("cp "+config["DATA_DIR"]+"graph "+config["RESULT_DIR"]+"Layer1.graph").c_str());
#ifdef DEBUG
        cout<<"here i come"<<endl;
#endif

		//Identification stage
		cout<<"nLayer = "<<nLayer<<endl;
        for (int i=1; i<=nLayer; i++)
        {
			cout<<"i = "<<i<<" nLayer = "<<nLayer<<endl;
            if (i!=1)
            {
                vector<string> otherCommFiles;
                for (int j=1; j<i; j++)
                    if (i!=j)
                        otherCommFiles.PB(config["RESULT_DIR"]+"Layer"+int2str(j)+".gen");
                generateLayerGraph(config["DATA_DIR"]+"graph",otherCommFiles,config["RESULT_DIR"]+"Layer"+int2str(i)+".graph",i==1);
            }

#ifdef DEBUG
        cout<<"here i come 2"<<endl;
#endif
            generateCommunity(config["RESULT_DIR"]+"Layer"+int2str(i)+".graph"
                              ,config["RESULT_DIR"]+"Layer"+int2str(i)+".gen",i, Truth_NC);
#ifdef DEBUG
            getchar();
#endif
            if (config.find("Save_CommunitySizeThres")!=config.end())
            {
                Community comm;
                comm.load(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
                comm.removeSmallComms(atoi(config["Save_CommunitySizeThres"].c_str()));
                comm.save(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
            }
            systemCall("mv LC_output.txt "+config["RESULT_DIR"]+"DSvalue"+int2str(i));
        }
        systemCall("mkdir -p "+config["RESULT_DIR"]+"Iteration/0/");//new add
        systemCall("cp "+config["RESULT_DIR"]+"Layer*.gen "+config["RESULT_DIR"]+"Iteration/0/");
        systemCall("cp "+config["RESULT_DIR"]+"Layer*.graph "+config["RESULT_DIR"]+"Iteration/0/");
        systemCall("cat "+config["RESULT_DIR"]+"Iteration/0/Layer?.gen > "+config["RESULT_DIR"]+"Iteration/0/AllLayer.gen");
        systemCall("cp "+config["RESULT_DIR"]+"DSvalue* "+config["RESULT_DIR"]+"Iteration/0/");
#ifdef DEBUG
        cout<<"come 2"<<endl;
#endif
        iterationDetail.PB(analyze("Initialize Iteration"));
#ifdef DEBUG
        cout<<"come 3"<<endl;
#endif
        //for save maxoriginal and layer
        if(tmpmodOriG > maxmodOriG)
        {
            maxmodOriG = tmpmodOriG;
            for(int nk=1; nk<=nLayer; nk++)
            {
                systemCall("cp "+config["RESULT_DIR"]+"*.gen "+config["RESULT_DIR"]+"maxOriginal");
                systemCall("cp "+config["RESULT_DIR"]+"*.graph "+config["RESULT_DIR"]+"maxOriginal");
            }
        }
        if(tmpmodLayerG > maxmodLayerG)
        {
            maxmodLayerG = tmpmodLayerG;
            for(int nk=1; nk<=nLayer; nk++)
            {
                systemCall("cp "+config["RESULT_DIR"]+"*.gen "+config["RESULT_DIR"]+"maxLayer");
                systemCall("cp "+config["RESULT_DIR"]+"*.graph "+config["RESULT_DIR"]+"maxLayer");
            }
        }
#ifdef DEBUG
        cout<<"come1"<<endl;
#endif

		//Refinement stage
        for (int r=1; r<=atoi(config["Number_Of_Iteration"].c_str()); r++)
        {
            fprintf(stderr,"####### Running Iteration %d with %d Layers #########\n",r,nLayer);
            for (int i=1; i<=nLayer; i++)
                systemCall(("cp "+config["RESULT_DIR"]+"Layer"+int2str(i)+".gen  "
                            +config["RESULT_DIR"]+"Layer"+int2str(i)+".gen.last").c_str());
            vector<int> order;
            order.clear();
            for (int i=1; i<=nLayer; i++) order.PB(i);
            //random_shuffle(order.begin(),order.end());

            for (int z=0; z<order.size(); z++)
            {
                int i=order[z];
                vector<string> otherCommFiles;
                for (int j=0; j<order.size(); j++)
                    if (i!=order[j])
                        otherCommFiles.PB(config["RESULT_DIR"]+"Layer"+int2str(order[j])+".gen");
                cout<<"gen LG start"<<endl;
                generateLayerGraph(config["DATA_DIR"]+"graph",otherCommFiles,config["RESULT_DIR"]+"Layer"+int2str(i)+".graph",i==1);
                cout<<"gen LG end"<<endl;
                generateCommunity(config["RESULT_DIR"]+"Layer"+int2str(i)+".graph"
                                  ,config["RESULT_DIR"]+"Layer"+int2str(i)+".gen",
                                  i, Truth_NC);
                cout<<"gen C end"<<endl;
                //getchar();
                if (config.find("Save_CommunitySizeThres")!=config.end())
                {
                    Community comm;
                    comm.load(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
                    comm.removeSmallComms(atoi(config["Save_CommunitySizeThres"].c_str()));
                    comm.save(config["RESULT_DIR"]+"Layer"+int2str(i)+".gen");
                }
                systemCall("mv LC_output.txt "+config["RESULT_DIR"]+"DSvalue"+int2str(i));
                cout<<"come end"<<endl;
            }
#ifdef DEBUG
            cout<<"come 5"<<endl;
#endif
            //autoShrinkLayer();//自动确定层数的策略，已经废弃。
            iterationDetail.PB(analyze("Iteration "+int2str(r)));
#ifdef DEBUG
            cout<<"come 6"<<endl;
#endif
            //for save max original and layer
            if(tmpmodOriG > maxmodOriG)
            {
                maxOriIter = r;
                maxmodOriG = tmpmodOriG;
                for(int nk=1; nk<=nLayer; nk++)
                {
                    systemCall("cp "+config["RESULT_DIR"]+"*.gen "+config["RESULT_DIR"]+"maxOriginal");
                    systemCall("cp "+config["RESULT_DIR"]+"*.graph "+config["RESULT_DIR"]+"maxOriginal");
                }
            }
            if(tmpmodLayerG > maxmodLayerG)
            {
                maxLayIter = r;
                maxmodLayerG = tmpmodLayerG;
                for(int nk=1; nk<=nLayer; nk++)
                {
                    systemCall("cp "+config["RESULT_DIR"]+"*.gen "+config["RESULT_DIR"]+"maxLayer");
                    systemCall("cp "+config["RESULT_DIR"]+"*.graph "+config["RESULT_DIR"]+"maxLayer");
                }
            }
            systemCall("mkdir -p "+config["RESULT_DIR"]+"Iteration/"+int2str(r)+"/");//new add
            systemCall("cp "+config["RESULT_DIR"]+"Layer*.gen "+config["RESULT_DIR"]+"Iteration/"+int2str(r)+"/");
            systemCall("cp "+config["RESULT_DIR"]+"Layer*.graph "+config["RESULT_DIR"]+"Iteration/"+int2str(r)+"/");
            systemCall("cat "+config["RESULT_DIR"]+"Iteration/"+int2str(r)+"/Layer?.gen > "+config["RESULT_DIR"]+"Iteration/"+int2str(r)+"/AllLayer.gen");
            systemCall("cp "+config["RESULT_DIR"]+"DSvalue* "+config["RESULT_DIR"]+"Iteration/"+int2str(r)+"/");
            //getchar();
        }
        PrintTruthAndLayer();
#ifdef DEBUG
        cout<<"here2"<<endl;
#endif
        PrintCurAndPre();
#ifdef DEBUG
        cout<<"here3"<<endl;
#endif
        showAnalyze();
#ifdef DEBUG
        cout<<"here4"<<endl;
#endif


        systemCall("mkdir "+config["RESULT_DIR"]+"Framework_"+config["Framework"]);
        systemCall("mv "+config["RESULT_DIR"]+"analyze "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv "+config["RESULT_DIR"]+"analyze.pdf "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv "+config["RESULT_DIR"]+"*.gen "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv layer?_vs_pre.pdf "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv Avg*.pdf "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv Mod_Layer?_in_*.pdf "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv NMI_Layer?_vs_inPreIter.pdf "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv truth?_vs_layer?.pdf "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv Truth_vs_layer "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv graphicData "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("mv Cur_vs_Pre  "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("cat "+config["RESULT_DIR"]+"maxLayer/Layer?.gen > "+config["RESULT_DIR"]+"maxLayer/AllLayer.gen");
        systemCall("cat "+config["RESULT_DIR"]+"maxOriginal/Layer?.gen > "+config["RESULT_DIR"]+"maxOriginal/AllLayer.gen");
		//copy graph to Framwork directory
        systemCall("mv "+config["RESULT_DIR"]+"*.graph "+config["RESULT_DIR"]+"Framework_"+config["Framework"]+"/");
        systemCall("rm "+config["RESULT_DIR"]+"*.gen.last");

        //systemCall("rm layer?_vs_pre.pdf Mod_Layer?_in_*.pdf NMI_Layer?_vs_inPreIter.pdf truth?_vs_layer?.pdf");
        //systemCall("rm -f "+config["RESULT_DIR"]+"*");

        //end = time(NULL);
        //outfile2<<difftime(end, start)<<endl;
        gettimeofday(&end,NULL);
        span = end.tv_sec-start.tv_sec + (end.tv_usec-start.tv_usec)/1000000.0;
        outfile2<<span<<endl;
    }
    outfile2.close();
    //systemCall("cp "+config["RESULT_DIR"]+"Frameworktime.txt "+config["RESULT_DIR"]+"Time/");

    //systemCall("rm -f -r "+config["TMP_DIR"]);
    systemCall("mkdir "+config["RESULT_DIR"]+"Data");
    systemCall("cp "+config["DATA_DIR"]+"graph "+config["RESULT_DIR"]+"Data/");
    for (int i=0; i<truth.size(); i++)
        systemCall("cp "+config["DATA_DIR"]+truth[i]+" "+config["RESULT_DIR"]+"Data/");


    puts("Finished");

    return 0;
}
