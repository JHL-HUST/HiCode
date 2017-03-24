#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <algorithm> // for swap
using namespace std;

int main (int argc, char const *argv[])
{
	//printf("%f", pow(2, -3));

	ifstream inFile;
    inFile.open (argv[1]);
    if (!inFile) {
        cout << "ERROR: unable to open input file" << endl;
        exit(1); // terminate with error
    }
    
   set< pair< int,int > > edge;

    int ni, nj, wij, index = 0;
	int tempV; inFile >> tempV;
    while (inFile >> ni >> nj >> tempV){
		//ni--; nj--; //start from 0
        //if (ni >= nj) swap(ni,nj); // undirected!
        //if (ni != nj)
            edge.insert(make_pair(ni, nj));
    }
    inFile.close(); inFile.clear();

	ofstream outfile;
	outfile.open(argv[2], ios::out);
	set< pair <int,int > >::iterator itr;
	for(itr = edge.begin(); itr != edge.end(); ++itr)
        {
		outfile << itr->first << "\t" << itr->second <<endl;
        //outfile << itr->second << " " << itr->first <<endl;
        }
	outfile.close();
	return 0;
}
