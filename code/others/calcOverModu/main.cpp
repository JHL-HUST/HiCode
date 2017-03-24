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

#include "../basic.h"
#include "../Graph.h"
#include "../Community.h"
#include "../Metrics_Overlapping.h"

int main(int argc,char** argv){
	cout<<calcModularity(argv[1],argv[2])<<endl;
	vector<int> c,d;
	d.push_back(2);
	c.push_back(1);
	vector<int> diff = difference(c, d);
    cout<<diff.size()<<endl;
}
