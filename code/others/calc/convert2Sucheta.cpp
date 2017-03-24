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
#include "Community.h"
using namespace std;

#define lowbit(x) ((x)&(-(x)))
#define sqr(x) ((x)*(x))
#define PB push_back
#define MP make_pair
#define LINESEP "###########################\n"


int main(int argc,char** argv){
	if (argc<3) return 0;
	FILE* fout=fopen(argv[1],"w");
	vector<int> rid;
	rid.clear();rid.PB(0);
	FILE* fin=fopen(argv[2],"r");
	for (int i;fscanf(fin,"%d",&i)==1;)
		rid.PB(i);
	for (int i=3;i<argc;i++){
		Community comm;
		comm.load(argv[i]);
		fprintf(fout,"level %d\n",i-3);
		comm.saveSucheta(fout,rid);
	}
	fclose(fout);
}