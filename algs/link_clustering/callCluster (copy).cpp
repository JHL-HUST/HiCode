clusterJaccsFile#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <utility>   // for pairs
#include <algorithm> // for swap
#include<unistd.h>
#include <sys/time.h>
//#include "clusterJaccsFile.h"
using namespace std;



int main(int argc, char const *argv[]){
    //************* make sure args are present:
    if (argc != 5){
        cout << "ERROR: something wrong with the inputs" << endl;
        cout << "usage:\n    " << argv[0] << " network.pairs network.jaccs network.clusters network.cluster_stats threshold" << endl;
        exit(1);
    }
    
    //for saving the time
    struct timeval start,end;
    gettimeofday(&start,NULL);	
    

    
	double maxDm = 0.0;
	double Dmns = 0.0;
	double counti = 0.1;
	for(int i = 1; i <= 9; ++i)
	{
		argv[4] = counti;
		double tmpd = 0.2;//clusterJaccsFile(argc, argv, maxDm);
		if(tmpd > maxDm)
			maxDm = tmpd;
		counti += 0.1;
	}
    
	gettimeofday(&end,NULL);
    ifstream inTime;  inTime.open( "./LCtime.txt" );
    double all, span;
    inTime>>all;
    inTime.close();
    
    span = end.tv_sec-start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;

    ofstream otime;
    otime.open("./LCtime.txt", ios::out);
    otime << all+span<<endl;
    otime.close();

    return 0;
}
