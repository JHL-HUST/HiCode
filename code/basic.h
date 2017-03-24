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
#define TVVS vector<vector<string> >

template <typename T>
vector<int> sort_indexes(const vector<T> &v) {

	// initialize original index locations
  	vector<int> idx(v.size());
  	iota( idx.begin(), idx.end(), 0 );

  	// sort indexes based on comparing values in v
  	if (config.find("ReduceFirst")!=config.end())
    {
    	if (config["ReduceFirst"]=="MIN")
  			sort(idx.begin(), idx.end(), [&v](int i1, int i2) {return v[i1] < v[i2];});
		else if (config["ReduceFirst"]=="MAX")
			sort(idx.begin(), idx.end(), [&v](int i1, int i2) {return v[i1] > v[i2];});
		else
		{
			cout<<"error parameter with ReduceFirst"<<endl;
			exit(-1);		
		}	
	}
	else
		sort(idx.begin(), idx.end(), [&v](int i1, int i2) {return v[i1] > v[i2];});

	return idx;
}

bool checkRequiredFile(string filename)
{
    FILE* fin=fopen(filename.c_str(),"r");
    if (fin==NULL)
    {
        system(("make "+filename).c_str());
        fin=fopen(filename.c_str(),"r");
        if (fin==NULL)
        {
            fprintf(stderr,"FILE %s NOT EXIST, AND FAIL TO MAKE\n",filename.c_str());
            return false;
        }
    }
    fclose(fin);
    return true;
}
bool checkFileExist(string filename)
{
    FILE* fin=fopen(filename.c_str(),"r");
    if (fin==NULL) return false;
    fclose(fin);
    return true;
}
string int2str(int i)
{
    char s[100];
    sprintf(s,"%d",i);
    return s;
}
string double2str(double v)
{
    char s[100];
    sprintf(s,"%.6f",v);
    return s;
}
double str2double(string s)
{
    char ts[1000];
    sprintf(ts,"%s",s.c_str());
    double v;
    sscanf(ts,"%lf",&v);
    return v;
}
void systemCall(string cmd)
{
    fprintf(stderr,"Call : %s\n",cmd.c_str());
    system(cmd.c_str());
}

//返回l到r之间的随机数
double getRand(double l=0,double r=1)
{
    return (rand()%1000000/1000000.)*(r-l)+l;
}

string escape(string s)
{
    string t="";
    for (int i=0; i<s.size(); i++)
    {
        if (s[i]=='\\'||s[i]=='_'||s[i]=='#')
            t=t+"\\";
        t=t+s[i];
    }
    return t;
}

void mergeMSS(map<string,string> &a,map<string,string> b)
{
    for (map<string,string>::iterator itr=b.begin(); itr!=b.end(); itr++)
        a[itr->first]=itr->second;
}
void mergeVSS(vector<pair<string,string> >&a,vector<pair<string,string> > b)
{
    for (int i=0; i<b.size(); i++)
        a.PB(b[i]);
}

double getAverage(vector<double> d)
{
    double s=0;
    for (int i=0; i<d.size(); i++)
        s+=d[i];
    return s/d.size();
}

vector<vector<string> > VSS2VVS(vector<pair<string,string> > data)
{
    vector<vector<string> > table;
    table.clear();
    for (int i=0; i<data.size(); i++)
    {
        vector<string> row;
        row.clear();
        row.PB(data[i].first);
        row.PB(data[i].second);
        table.PB(row);
    }
    return table;
}

vector<vector<string> > transVVS(vector<vector<string> > data)
{
    vector<vector<string> > newData;
    newData.clear();
    for (int i=0; i<data[0].size(); i++)
    {
        vector<string> row;
        row.clear();
        for (int j=0; j<data.size(); j++)
            row.PB(data[j][i]);
        newData.PB(row);
    }
    return newData;
}

void Tex_Table(vector<vector<string> > table,string name,FILE* fout)
{
    if (table.size()==0) return;
    fprintf(fout,"\\begin{table}[htb!]\n\\centering\n");
    fprintf(fout,"\\caption{%s}\n",escape(name).c_str());
    fprintf(fout,"\\begin{tabular}{|");
    for (int i=0; i<table[0].size(); i++)
        fprintf(fout,"c|");
    fprintf(fout,"}\n");
    for (int i=0; i<table.size(); i++)
    {
        fprintf(fout,"\\hline\n");
        for (int j=0; j<table[i].size(); j++)
        {
            if (j) fprintf(fout,"&");
            fprintf(fout,"%s",escape(table[i][j]).c_str());
        }
        fprintf(fout,"\\\\\n");
    }
    fprintf(fout,"\\hline\n");
    fprintf(fout,"\\end{tabular}\\end{table}\n");
}

void Tex_includeGraphics(string filename,string name,double width,FILE* fout)
{
    fprintf(fout,"\\begin{figure}[htb!]\n\\centering\n");
    fprintf(fout,"\\caption{%s}\n",escape(name).c_str());
    fprintf(fout,"\\includegraphics[width=%.1fin]{%s}\n",width,filename.c_str());
    fprintf(fout, "\\end{figure}\n");
}

vector<string> split(string str)
{
    vector<string> res;
    res.clear();
    char s[10000];
    sprintf(s,"%s",str.c_str());
    for (char* p=strtok(s,";"); p; p=strtok(NULL,";"))
        res.PB(p);
    return res;
}

vector<int> intersect(vector<int> Xi, vector<int> Yj)
{
    int Xi_size = Xi.size();
    int Yj_size = Yj.size();
    vector<int> v(Xi.size()>Yj.size()?Xi.size():Yj.size());
    if (Xi_size == 0)
        return Xi;
    if (Yj_size == 0)
        return Yj;


    vector<int>::iterator it;

    int Xi_array[Xi.size()];
    int Yj_array[Yj.size()];
    int i,j;
    for (i = 0; i<Xi.size(); i++)
        Xi_array[i] = Xi[i];
    for (j = 0; j<Yj.size(); j++)
        Yj_array[j] = Yj[j];
    int* pXi = Xi_array;
    int* pYj = Yj_array;
    sort (pXi,pXi+Xi_size);
    sort (pYj,pYj+Yj_size);
    it=set_intersection (pXi, pXi+Xi_size, pYj, pYj+Yj_size, v.begin());
    v.resize(it-v.begin());
    return v;
}

int intersect_size(vector<int> Xi, vector<int> Yj)
{
    vector<int> v = intersect(Xi, Yj);
    return v.size();
}


vector<int> symmetric_difference(vector<int> Xi, vector<int> Yj)
{
    int Xi_size = Xi.size();
    int Yj_size = Yj.size();

    if (Yj_size == 0)
    {
        return Xi;
    }
    else if (Xi_size == 0)
    {
        return Yj;
    }

    vector<int> v(Xi.size()>Yj.size()?Xi.size():Yj.size());
    vector<int>::iterator it;

    int Xi_array[Xi.size()];
    int Yj_array[Yj.size()];
    int i,j;
    for (i = 0; i<Xi.size(); i++)
        Xi_array[i] = Xi[i];
    for (j = 0; j<Yj.size(); j++)
        Yj_array[j] = Yj[j];
    int* pXi = Xi_array;
    int* pYj = Yj_array;
    sort (pXi,pXi+Xi_size);
    sort (pYj,pYj+Yj_size);
    it = set_symmetric_difference(pXi, pXi+Xi_size, pYj, pYj+Yj_size, v.begin());
    v.resize(it-v.begin());
    return v;
}

vector<int> difference(vector<int> Xi, vector<int> Yj)
{
    int Xi_size = Xi.size();
    int Yj_size = Yj.size();

    if (Xi_size == 0 || Yj_size == 0)
    {
        return Xi;
    }   

    vector<int> v(Xi.size()>Yj.size()?Xi.size():Yj.size());
    vector<int>::iterator it;

    int Xi_array[Xi.size()];
    int Yj_array[Yj.size()];
    int i,j;
    for (i = 0; i<Xi.size(); i++)
        Xi_array[i] = Xi[i];
    for (j = 0; j<Yj.size(); j++)
        Yj_array[j] = Yj[j];
    int* pXi = Xi_array;
    int* pYj = Yj_array;
    sort (pXi,pXi+Xi_size);
    sort (pYj,pYj+Yj_size);
    it = set_difference(pXi, pXi+Xi_size, pYj, pYj+Yj_size, v.begin());
    v.resize(it-v.begin());
    return v;
}

