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
#include "Metrics_Overlapping.h"

int main(int argc,char** argv){
	cout<<calcNMI(argv[1],argv[2])<<endl;
}
