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

class Community
{
private:
    vector<int> cid;	//node i belongs to community cid[i]
    void calcid()
    {
        id.clear();
        for (int i=1; i<cid.size(); i++)
        {
            for (; cid[i]>=id.size();)
            {
                vector<int> t;
                t.clear();
                id.PB(t);
            }
            id[cid[i]].PB(i);
        }
        NC=id.size()-1;
    }
public:
    vector<vector<int> > id;
	vector<int> id_size;
    vector<int> leftover;
	int NC;	//how namy communities
	int N; //max node No.
    bool isPartition;	//is not overlapping

    bool testPartition()
    {
        set<int> t;
        t.clear();
        for (int i=1; i<=NC; i++)
            for (int z=0; z<id[i].size(); z++)
            {
                if (t.find(id[i][z])!=t.end()) return false;
                t.insert(id[i][z]);
            }
        return true;
    }

    Community() {}

	void sync_id_size()
	{
		id_size.resize(id.size());
		for (int i=0; i< id.size(); i++)
			id_size[i] = id[i].size();
	}
    void load(string filename)
    {
		string suffix = filename.substr(filename.size()-3);
        if (suffix == "par")
            loadPartition(filename);
        else
			loadGeneral(filename);
    }
    void save(string filename)
    {
		string suffix = filename.substr(filename.size()-3);
        if (suffix == "par")
            savePartition(filename);
        else if (suffix == "gen")
			saveGeneral(filename);
		else 
		{
			cout<<"ERROR: save error: please check the suffix of the community file"<<endl;
			exit(-1);//exit with error
		}	    
	}
    void loadPartition(string filename)
    {
        FILE* fin=fopen(filename.c_str(),"r");
        cid.clear();
        cid.PB(0);
        for (int i; fscanf(fin,"%d",&i)==1;)
            cid.PB(i);
        fclose(fin);
        N=cid.size()-1;
        isPartition=true;

        reId();
        leftover.clear();
        calcid();
    }
    void savePartition(string filename)
    {
        FILE* fout=fopen(filename.c_str(),"w");
        for (int i=1; i<cid.size(); i++)
            fprintf(fout,"%d\n",cid[i]);
        fclose(fout);
    }

	
    void loadGeneral(string filename)
    {
        FILE* fin=fopen(filename.c_str(),"r");
        map<int,int> tcid;	//i belongs to community tcid[i]
        tcid.clear();
        char s[1000000];
        vector<int> cur;
        cur.clear();
        id.clear();
        id.PB(cur);	//id[1] is the first community. id[0] is unused
        isPartition=true;
        int curid=0;
        N=0;	
		
        for (; fgets(s,1000000,fin);)
        {
            curid++;
            cur.clear();
            for (char* p=strtok(s,"\t \n\r"); p; p=strtok(NULL,"\t \n\r"))
            {
                int i=atoi(p);
                cur.PB(i);
                N=max(N,i);

				//check if overlapping
                if (tcid.find(i)!=tcid.end()) isPartition=false;
                tcid[i]=curid;
            }
            if (cur.size()>0) 
			{
				id.PB(cur);
			}
        }
        fclose(fin);

		//not overlapping
        if (isPartition)
        {
            cid.clear();
            cid.PB(0);
            for (int i=1; i<=N; i++)
                cid.PB(tcid[i]);
        }

        leftover.clear();
        NC=id.size()-1;
    }
    void attach(vector<int> t)
    {
		if (t.size()!=0)
		{        
			id.PB(t);
        	NC++;
		}
    }
    void pop()
    {
        id.pop_back();
        NC--;
    }
    void saveGeneral(string filename)
    {
		sync_id_size();
        //if (!testPartition()) cout<<"ERR"<<" "<<filename<<endl;
        FILE* fout=fopen(filename.c_str(),"w");
		if (config.find("Attach_Leftover")!=config.end())
		{
			if (config["Attach_Leftover"]=="TRUE")
				attach(leftover);
		}
		vector<int> sorted_index = sort_indexes(id_size);
		#ifdef DEBUG
			//assert(sorted_index.size()==id_size.size());
			//assert(comm.id.size()==id_size.size());
			cout<<endl;
			for (int i=0; i<id.size(); i++){
				cout<<sorted_index[i]<<" : "<<id_size[sorted_index[i]]<<endl;
			}
		#endif
		int sorted_i=0;
        for (int index=0; index<=NC; index++){
			sorted_i = sorted_index[index];
			if (id[sorted_i].size())
            {
                for (int j=0; j<id[sorted_i].size(); j++)
				{
                   	fprintf(fout,"%d\t",id[sorted_i][j]);
				}
                fprintf(fout,"\n");
            }
        	//pop();
		}

        //for (int i=0;i<leftover.size();i++) fprintf(fout,"%d\n",leftover[i]);
        fclose(fout);
    }
    void saveSucheta(FILE* fout,vector<int> rid)
    {
        /*calcid();
        for (int i=1;i<id.size();i++){
        	for (int j=0;j<id[i].size();j++)
        		fprintf(fout,"%d\t",rid[id[i][j]]);
        	if (id[i].size())fprintf(fout,"\n");
        }*/
    }
    void loadModularity(string filename)
    {
        FILE* fin=fopen(filename.c_str(),"r");
        cid.clear();
        for (int i,j,k=0; fscanf(fin,"%d%d",&i,&j)==2;)
        {
            k+=(i==0);
            if (k==2) break;
            cid.PB(j);
        }
        N=cid.size()-1;
        for (int layer=1;; layer++)
        {
            map<int,int> rid;
            rid.clear();
            for (int i,j; fscanf(fin,"%d%d",&i,&j)==2;)
            {
                if (i==0) break;
                rid[i]=j;
            }
            if (rid.size()==0) break;
            for (int i=0; i<cid.size(); i++)
                cid[i]=rid[cid[i]];
        }
        fclose(fin);
        leftover.clear();
        reId();
        calcid();
    }
    void loadInfomap(string filename)
    {
        FILE* fin=fopen(filename.c_str(),"r");
        cid.clear();
        cid.PB(0);
        fscanf(fin,"%*s%*d");
        for (int i; fscanf(fin,"%d",&i)==1;)
            cid.PB(i);
        fclose(fin);
        N=cid.size()-1;
        isPartition=true;

        reId();
        leftover.clear();
        calcid();
    }
    void loadOSLOM(string filename)
    {
        FILE* fin=fopen(filename.c_str(),"r");
        map<int,int> tcid;
        tcid.clear();
        char s[1000000];
        vector<int> cur;
        cur.clear();
        id.clear();
        id.PB(cur);
        isPartition=true;
        int curid=0;
        N=0;
        int linenumber=0;
        for (; fgets(s,1000000,fin);)
        {
            linenumber++;
            if (linenumber%2) continue;
            curid++;
            cur.clear();
            for (char* p=strtok(s,"\t \n\r"); p; p=strtok(NULL,"\t \n\r"))
            {
                int i=atoi(p);
                cur.PB(i);
                N=max(N,i);
                if (tcid.find(i)!=tcid.end()) isPartition=false;
                tcid[i]=curid;
            }
            if (cur.size()>0) 
			{
				id.PB(cur);
			}
        }
        fclose(fin);
        if (isPartition)
        {
            cid.clear();
            cid.PB(0);
            for (int i=1; i<=N; i++)
                cid.PB(tcid[i]);
        }
        leftover.clear();
        NC=id.size()-1;
    }
    void reId()
    {
        map<int,int> rid;
        rid.clear();
        for (int i=1,j=1; i<=N; i++)
        {
            if (rid.find(cid[i])==rid.end())
                rid[cid[i]]=j++;
            cid[i]=rid[cid[i]];
        }
    }
    void removeSmallComms(int Thres)
    {
        for (int i=1; i<=NC; i++)
            if (id[i].size()<Thres)
            {
				#ifdef DEBUG
					cout<<"id["<<i<<"].size()="<<id[i].size()<<"\tThres="<<Thres<<endl;
				#endif
                for (int z=0; z<id[i].size(); z++)
				{
                    leftover.PB(id[i][z]);				
				}                
				id[i].clear();
				while ((id[NC].size()<Thres)&&(NC>i))
				{
					#ifdef DEBUG
					cout<<"id["<<NC<<"].size()="<<id[NC].size()<<endl;
					#endif
					NC--;
					id.pop_back();
				}
                id[i] = id[NC];
                NC--;
                id.pop_back();
                if (NC+1 != id.size())
                {
					#ifdef DEBUG
                    	fprintf(stderr, "Function removeSmallComms in Community.h error");
					#endif
                    exit(-1);
                }
            }
    }

	//return res, i belongs to community res[i]
    vector<int> getPartitionID()
    {
        vector<int> res;
        res.clear();
        //if (!isPartition) return res;
        res.resize(N+1);
        for (int i=0; i<=N; i++) res[i]=-1;
        for (int i=1; i<=NC; i++)
            for (int j=0; j<id[i].size(); j++)
                res[id[i][j]]=i;
        //int cur=NC;
        //for (int i=0;i<=N;i++) if (res[i]==-1) res[i]=++NC;
        return res;
    }
    vector<vector<int> > getPartitionIDOver()
    {
        vector<vector<int> > res;
        for (int i=0; i<=N; i++)
        {
            vector<int> tmp;
            res.PB(tmp);
        }
        //cout<<"aaaaaaaaaa"<<res.size()<<" NC:"<<NC<<endl;
        //cout<<id.size()<<" "<<id[1].size()<<" "<<id[2].size()<<" "<<id[1][0]<<endl;
        //res.resize(N+1);
        //for (int i=0;i<=N;i++) res[i]=-1;
        /*for (int i=1;i<=NC;i++){
        	for (int j=0;j<id[i].size();j++){
        		cout<<id[i][j]<<" ";
        	}
        	cout<<endl;
        }*/
        for (int i=1; i<=NC; i++)
        {
            for (int j=0; j<id[i].size(); j++)
            {
                //cout<<"s1 ";
                //cout<<id[i][j]<<" ";
                res[id[i][j]].PB(i);
            }
            //cout<<endl;
        }
        //cout<<"ssssssggggggg"<<endl;
        //int cur=NC;
        //for (int i=0;i<=N;i++) if (res[i]==-1) res[i]=++NC;
        return res;
    }
    void loadLinkCommunity(string filename)
    {
        FILE* fin=fopen(filename.c_str(),"r");
        map<int,int> tcid;
        tcid.clear();
        char s[1000000];
        vector<int> cur;
        cur.clear();
        id.clear();
        id.PB(cur);
        isPartition=true;
        int curid=0;
        N=0;
        for (; fgets(s,1000000,fin);)
        {
            curid++;
            cur.clear();
            set<int> tmp;
            tmp.clear();
            for (char* p=strtok(s,"\t \n\r,"); p; p=strtok(NULL,"\t \n\r,"))
            {
                int i=atoi(p);
                if (tmp.find(i)!=tmp.end()) continue;
                tmp.insert(i);
                cur.PB(i);
                N=max(N,i);
                if (tcid.find(i)!=tcid.end()) isPartition=false;
                tcid[i]=curid;
            }
            if (cur.size()>0) id.PB(cur);
        }
        fclose(fin);
        if (isPartition)
        {
            cid.clear();
            cid.PB(0);
            for (int i=1; i<=N; i++)
                cid.PB(tcid[i]);
        }
        leftover.clear();
        NC=id.size()-1;
    }
    void sub(map<int,int> rid)
    {
        for (int i=1; i<=NC; i++)
            for (int j=0; j<id[i].size();)
            {
                if (rid.find(id[i][j])==rid.end())
                {
                    swap(id[i][j],id[i][id[i].size()-1]);
                    id[i].pop_back();
                }
                else
                {
                    id[i][j]=rid[id[i][j]];
                    j++;
                }
            }
        N=rid.size();
    }
    

};
