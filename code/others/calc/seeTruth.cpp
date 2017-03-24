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
using namespace std;

map<string,string> config;

#include "basic.h"
#include "Graph.h"
#include "Community.h"
#include "SingleLayer_Method.h"
#include "SingleLayer_Modularity.h"
#include "SingleLayer_Infomap.h"
#include "SingleLayer_OSLOM.h"
#include "SingleLayer_Walktrap.h"
#include "SingleLayer_LinkCommunity.h"
#include "SingleLayer_CONGA.h"
#include "Framework.h"
#include "Framework_Remove.h"
#include "Framework_Replace.h"
#include "Framework_Reduce.h"
#include "Framework_ReduceP.h"

#include "Metrics.h"

#define lowbit(x) ((x)&(-(x)))
#define sqr(x) ((x)*(x))
#define PB push_back
#define MP make_pair
#define LINESEP "###########################\n"

int main(int argc,char** argv){
	Community comm;
	comm.load(argv[1]);

	Graph g;
	if (!g.load(argv[2])) return 0;

	int sum = 0;
	double p = g.N;

	for(int i=1; i <=comm.NC; ++i)
	{
		sum+=comm.id[i].size();
	}
	cout<<"total:"<<g.N<<endl;
	cout<<"equal 0:"<<(g.N-sum)<<endl;
	cout<<"percent:"<<(g.N-sum)/p<<endl;
	return 0;
}

