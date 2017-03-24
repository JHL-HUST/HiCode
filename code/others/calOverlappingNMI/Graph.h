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

#define lowbit(x) ((x)&(-(x)))
#define sqr(x) ((x)*(x))
#define PB push_back
#define MP make_pair

struct Edge{
	int i,j;
	double w;
	Edge(int i=0,int j=0,double w=0):i(i),j(j),w(w){}
};

class Graph{
public:
	vector<Edge> g;
	vector<vector<pair<int,double> > > links;
	vector<double> nodeWeight;
	double M;
	int N;
	bool calc;
	Graph(int N=0):N(N){g.clear();calc=false;}
	bool load(string filename){
		bool ispair=false;
		if (filename.substr(filename.size()-4)=="pair") ispair=true;
		FILE* fin=fopen(filename.c_str(),"r");
		if (fin==NULL) return false;
		fscanf(fin,"%d",&N);
		g.clear();
		int i,j;
		double w;
		set<pair<int,int> >t;t.clear();
		if (!ispair) for (;fscanf(fin,"%d%d%lf",&i,&j,&w)==3;){
			if (t.find(MP(i,j))!=t.end()) continue;
			g.PB(Edge(i,j,w));
			g.PB(Edge(j,i,w));
			t.insert(MP(i,j));
			t.insert(MP(j,i));
		}
		else {
			int cnt=0;
			int THRES=1000000;
			for (;fscanf(fin,"%d%d",&i,&j)==2;){
				cnt++;if (cnt%10000==0) printf("\rLoading #%d",cnt);
				if (t.find(MP(i,j))!=t.end()) continue;
				if (i>THRES||j>THRES) continue;
				g.PB(Edge(i,j,1));
				g.PB(Edge(j,i,1));
				t.insert(MP(i,j));
				t.insert(MP(j,i));
			}
		}
		fclose(fin);
		calc=false;
		return true;
	}
	void save(string filename){
		FILE* fout=fopen(filename.c_str(),"w");
		fprintf(fout,"%d\n",N);
		for (int i=0;i<g.size();i++)
			if (g[i].i<=g[i].j){
			fprintf(fout,"%d\t%d\t%f\n",g[i].i,g[i].j,g[i].w);
		}
		fclose(fout);
	}
	void calc_links(){
		if (calc) return;
		links.resize(N+1);
		nodeWeight.resize(N+1);
		for (int i=0;i<=N;i++) {
			links[i].clear();
			nodeWeight[i]=0;
		}
		M=0;
		for (int i=0;i<g.size();i++){
			links[g[i].i].PB(MP(g[i].j,g[i].w));
			nodeWeight[g[i].i]+=g[i].w;
			M+=g[i].w;
		}
		M/=2;
		calc=true;
	}
	void saveSingleEdge(string filename){
		FILE* fout=fopen(filename.c_str(),"w");
		for (int i=0;i<g.size();i++)
			if (g[i].i<g[i].j)
				fprintf(fout,"%d\t%d\t%f\n",g[i].i,g[i].j,g[i].w);
		fprintf(fout,"%d\t%d\t%d\n",N,N,0);
		fclose(fout);
	}
	void savePairs(string filename){
		FILE* fout=fopen(filename.c_str(),"w");
		for (int i=0;i<g.size();i++)
			if (g[i].i<g[i].j&&getRand()<g[i].w)
				fprintf(fout,"%d\t%d\n",g[i].i,g[i].j);
		//fprintf(fout,"%d\t%d\n",N,N);
		fclose(fout);
	}
	void savePairWeight(string filename){
		FILE* fout=fopen(filename.c_str(),"w");
		for (int i=0;i<g.size();i++)
			if (g[i].i<g[i].j&&getRand()<g[i].w)
				fprintf(fout,"%d\t%d\t%f\n",g[i].i,g[i].j,g[i].w);
		//fprintf(fout,"%d\t%d\n",N,N);
		fclose(fout);
	}
	void savePairsBiDir(string filename){
		FILE* fout=fopen(filename.c_str(),"w");
		for (int i=0;i<g.size();i++)
			if (getRand()<g[i].w)
				fprintf(fout,"%d\t%d\n",g[i].i,g[i].j);
		fprintf(fout,"%d\t%d\n",N,N);
		fclose(fout);
	}
	void addEdge(int i,int j,double w){
		g.PB(Edge(i,j,w));
		calc=false;
	}
	void sampleByWeights(){
		vector<Edge> tg=g;
		g.clear();
		for (int i=0;i<tg.size();i++)
			if (getRand(0,1)<=tg[i].w)
				g.PB(Edge(tg[i].i,tg[i].j,1));
	}
	void sub(map<int,int> rid){
		N=rid.size();
		for (int z=0;z<g.size();){
			int i=g[z].i;
			int j=g[z].j;
			if (rid.find(i)==rid.end()||rid.find(j)==rid.end()){
				swap(g[z],g[g.size()-1]);
				g.pop_back();
			}
			else {
				g[z]=Edge(rid[i],rid[j],g[z].w);
				z++;
			}
		}
		M=g.size();
	}
	vector<int> randomSample(int cnt){
		vector<int> res;
		set<int> vi;
		vi.clear();
		calc_links();
		for (int cur=0;res.size()<cnt;){
			if (cur==res.size()){
				int k=rand()%N+1;
				for (;vi.find(k)!=vi.end();k=rand()%N+1);
				vi.insert(k);
				res.PB(k);
			}
			else {
				int i=res[cur];cur++;
				for (int z=0;z<links[i].size();z++)
					if (links[i][z].second>0&&vi.find(links[i][z].first)==vi.end()){
						vi.insert(links[i][z].first);
						res.PB(links[i][z].first);
					}
			}
		}
		res.resize(cnt);
		return res;
	}
	void saveWalkTrap(string filename){
		FILE* fout=fopen(filename.c_str(),"w");
		for (int i=0;i<=N;i++){
			fprintf(fout,"%d %d 1\n",i,i);
		}
		for (int z=0;z<g.size();z++){
			if (g[z].i>=g[z].j) continue;
			fprintf(fout, "%d %d %.10f\n",g[z].i,g[z].j,g[z].w);
		}
		fclose(fout);
	}
};
