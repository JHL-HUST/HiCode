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

vector<int> ids;
map<int,int> rid;

int main(int argc,char** argv){
	Graph g;
	g.load(argv[1]);
	ids=g.randomSample(atoi(argv[2]));
	rid.clear();
	for (int i=0;i<ids.size();i++)
		rid[ids[i]]=i+1;
	g.sub(rid);
	g.save(argv[3]);
	for (int i=4;i<argc;i+=2){
		Community comm;
		comm.load(argv[i]);
		comm.sub(rid);
		comm.save(argv[i+1]);
	}
}