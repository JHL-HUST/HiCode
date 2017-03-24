#include <assert.h>
double calcModularity_real(Graph graph,Community comms)
{
    double Q=0;
    vector<double> edge_in_in;
    edge_in_in.clear();
    vector<double> edge_in_out;
    edge_in_out.clear();
    double M=0;
    if (comms.leftover.size()!=0)
        comms.attach(comms.leftover);
    int nc = comms.NC;
    cout<<"N = "<<comms.N<<"NC = "<<comms.NC<<endl;
    vector<vector<int > > cid=comms.getPartitionIDOver();
    //cout<<"cid.sizes = "<<cid.size()<<endl;
    for (int i=0; i<=nc; i++)
    {
        edge_in_in.PB(0);
        edge_in_out.PB(0);
    }

    if (cid.size()==0)
    {
        if (comms.leftover.size()!=0)
            comms.pop();
        return log(0);
    }
    int h[cid.size()];
    for (int cid_i = 1; cid_i < cid.size(); cid_i++)
    {
        h[cid_i] = cid[cid_i].size();
    }
    vector<int> cap;//used to store intersection
    vector<int> diff_i_j;//used to store difference
    int v_i = 0;
    for (int edge_e=0; edge_e<graph.g.size(); edge_e++)
    {
        int i=graph.g[edge_e].i;
        int j=graph.g[edge_e].j;
        double a_ij=graph.g[edge_e].w;//a_ij 邻接矩阵
        //fprintf(stdout, "i = %d, j = %d, a_ij = %lf\n", i, j, a_ij);
        if ((cid[i].size()!=0) && (i<=comms.N))//若i,j不属于任何社团，它们自然不会对任何社团的e值产生贡献
        {
            if (j<=comms.N)//判断点j是否在运行时BASE算法中被除去
            {
                cap = intersect(cid[i], cid[j]);//i,j共同的社团集合
                //fprintf(stdout, "cid[%d].size = %d\tcap.size = %d\n", i, cid[i].size(), cap.size());
                if (cap.size() != 0) //如果i,j有共同的社团
                {
                    //copy (cap.begin(), cap.end(), ostream_iterator<int> (cout, " "));
                    //cout<<endl;
                    for (v_i = 0; v_i < cap.size(); v_i++)
                    {
                        assert(cap[v_i] <= nc);
                        edge_in_in[cap[v_i]] += 0.5*(1.0/h[i] + 1.0/h[j])*a_ij;//无向图里，每条边出现两次
                    }
                }
            }
            //cout<<"diff start"<<endl;
            if (j>comms.N)
                diff_i_j = cid[i];
            else
                diff_i_j = difference(cid[i], cid[j]);
            //cout<<"diff end"<<endl;
            //vector<int> diff_j_i = difference(cid[j], cid[i]);
            //sym_diff([1, 2, 3, 4], [2, 4, 5, 8]) = [1, 3, 5, 8]
            //diff([1, 2, 3, 4], [2, 4, 5, 8]) = [1, 3]
            if (diff_i_j.size() != 0) //i∈diff_i_j, j∉diff_i_j
            {
                for (v_i = 0; v_i < diff_i_j.size(); v_i++)
                {
                    assert(diff_i_j[v_i] <= nc);
                    edge_in_out[diff_i_j[v_i]] += 0.5*(1.0/h[i] + 1)*a_ij;//i∈diff_i_j
                }
            }
            //cout<<"e_i_o accumulate end"<<endl;
            /*if (diff_j_i.size() != 0) //j∈diff_i_j, i∉diff_i_j
            {
                for (v_i = 0; v_i < diff_j_i.size(); v_i++)
                {
                    edge_in_out[diff_i_j[v_i]] += 0.5*(1.0/h[j] + 1)*a_ij;//j∈diff_i_j
                }
            }*/
        }
        M+=a_ij;//累加a_ij的权值，无向图中a_ij=a_ji=1;
    }
    //cout<<"M accumulate end"<<endl;
    for (int i=1; i<=nc; i++)
    {
        Q+=edge_in_in[i]/M-sqr((edge_in_in[i]+edge_in_out[i])/(M));
        //fprintf(stdout, "inner_edge[%d] = %lf, outer_edge[%d] = %lf\n", i, edge_in_in[i]/2, i, edge_in_out[i]);
    }
    //edge_in_in是两倍内部边，edge_in_out是一杯的跨出去的边，M是两倍图中所有边。
    //因为这里用有向图表示。即无向图每条边会出现两次
    //
    if (comms.leftover.size()!=0)
        comms.pop();
    //cout<<"calcModularity Graph and comm DONE"<<endl;
    return Q;
}

double calcModularity(string graphFile,string commsFile)
{
    cout<<"Overlapping MODularity Start Calculating: "<<endl<<commsFile<<" in "<<graphFile<<endl;
    Graph graph;
    if (!graph.load(graphFile)) return log(0);
    Community comms;
    comms.load(commsFile);
    if (config.find("Metric_CommunitySizeThres")!=config.end())
        comms.removeSmallComms(atoi(config["Metric_CommunitySizeThres"].c_str()));
    return calcModularity_real(graph, comms);
}

