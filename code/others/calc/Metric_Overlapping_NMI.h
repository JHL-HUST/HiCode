double Entropy_h(double p)
/*
    require: p>=0
    return:
         { -plog(p),    if p>0
    h(p)=|
         {  0      ,    otherwise
*/
{
    double result = p>0?(-p*log(p)):0;
    //cout<<"Entropy_h("<<p<<") = "<<result<<endl;
    return result;
}

double Entropy_H(vector<int> Xi, int max_size)
/*
    calculate H(Xi)
    require: max_size>|Xi|>0
    return:  H(Xi) = h[P(Xi∈X)] + h[P(Xi∉X)]
*/

{
    double PXi_1 = (double)Xi.size()/(double)max_size; //P(Xi∈X)
    //cout<<"Xi.size = "<<Xi.size()<<endl;
    double PXi_0 = 1 - PXi_1;        //P(Xi∉X)
    //cout<<"PXi_0 = "<<PXi_0<<endl;
    //cout<<"PXi_1 = "<<PXi_1<<endl;
    return Entropy_h(PXi_1) + Entropy_h(PXi_0);
}




double each_right_Conditional_Entropy(vector<int> Xi,
                                      vector<int> Yj,
                                      int max_size)
/*
    Xi 就是 X_left_comm_vector， 同样的Yj 就是 Y_right_comm_vector
    calculate H(Xi|Yj)
    require:  max_size > |Xi| > 0
              max_size > |Yj| > 0
    return:
                         {   H(Xi,Yj) - H(Yj), if constraint satisfied
              H(Xi|Yj) = |
                         {   INF     , otherwise
*/
{
    //计算H(Xi,Yj) = h[P(1,1)] + h[P(1,0)] + h[P(0,1)] + h[P(0,0)]
    int intersectionXi_Yj = intersect_size(Xi, Yj);
    double intersection = (double)intersectionXi_Yj;
    double unionXi_Yj        = (double)Xi.size() - intersectionXi_Yj + (double)Yj.size();
    double P_1_1 = intersection/(double)max_size;//calculate P(1,1) = |Xi ∩ Yj|/n
    double P_1_0 = ( Xi.size() - intersection )/(double)max_size;// P(1,0) = (|Xi|-|Xi ∩ Yj|)/n
    double P_0_1 = ( Yj.size() - intersection )/(double)max_size;// P(0,1) = (|Yi|-|Xi ∩ Yj|)/n
    double P_0_0 = 1 - ( unionXi_Yj )/(double)max_size;             // P(0,0) = |Xi ∪ Yj|/n
    double h11 = Entropy_h(P_1_1);//h[P(1,1)]
    double h10 = Entropy_h(P_1_0);//h[P(1,0)]
    double h01 = Entropy_h(P_0_1);//h[P(0,1)]
    double h00 = Entropy_h(P_0_0);//h[P(0,0)]

    if ( (h11 + h00) > (h01 + h10) ) //(satisfaction check)
        //判断是否满足限制(constraint)h[P(1,1)] + h[P(1,1)] > h[P(0,1)] + h[P(1,0)]
        return h11 + h10 + h01 + h00 - Entropy_H(Yj, max_size);
    else
        //表示不满足该限制条件，设置为-log(0)=inf就可以在each_left_Conditional_Entropy中判断是不是每个j都不满足该限制条件
        return -log(0);

    //因为只有都不满足，each_left_Conditional_Entropy中的MinH_XiYj才会是inf无穷大

}

double each_left_Conditional_Entropy(vector<int> X_left_comm_vector,
                                     Community right_commsY,
                                     int max_size)
/*
    Y = right_commsY
    calculate H(Xi|Y)
    require: max_size > |Xi| > 0
         and right_commsY  ≠ ∅
    return:
                       {    min_{Yj∈Y}H(Xi|Yj), if min_{Yi∈Y}H(Xi|Yj) ≠ INF
             H(Xi|Y) = |
                       {    H(Xi)             , otherwise
*/
{
    int k = right_commsY.NC;
    vector<int> Y_right_comm_vector = right_commsY.id[1];
    double H_XiYj = 0;
    double MinH_XiYj = each_right_Conditional_Entropy(X_left_comm_vector,
                       Y_right_comm_vector,
                       max_size);
    for (int j = 2; j <= k; j++)
    {
        Y_right_comm_vector = right_commsY.id[j];
        H_XiYj = each_right_Conditional_Entropy(X_left_comm_vector,
                                                Y_right_comm_vector,
                                                max_size);

        if (MinH_XiYj > H_XiYj)
            MinH_XiYj = H_XiYj;
    }
    if (MinH_XiYj == -log(0))//表示对每个j，都不满足限制h[P(1,1)]+h[P(0,0)]>h[P(0,1)]+h[P(1,0)]
        MinH_XiYj = Entropy_H(X_left_comm_vector, max_size);//在这种情况下，赋值H(Xi)
    //cout<<"MinH_XiYj"<<MinH_XiYj<<endl;
    return MinH_XiYj;
}

double Conditional_Entropy(Community left_comms, Community right_comms)
/*
    calculate Conditional Entropy H(X|Y)
    require:    left_comms ≠ ∅ and right_comms ≠ ∅
    return:               1   _k_   H(Xi|Y)
                H(X|Y) = ---  \  ` ---------
                          k   /__,   H(Xi)
                              i=1
*/
{
    int k = left_comms.NC;
    int max_size = left_comms.N>right_comms.N?left_comms.N:right_comms.N;
    double sum, result;
    sum = 0;
    result = 0;
    vector<int> X_left_comm_vector;
    for (int i = 1; i <= k; i++)
    {
        X_left_comm_vector = left_comms.id[i];
        sum += each_left_Conditional_Entropy(X_left_comm_vector, right_comms, max_size)/Entropy_H(X_left_comm_vector, max_size);
        //H(Xi|Y)/H(Xi)
    }
    result = sum/k;
    //cout<<"Conditional_Entropy, K"<<result<<", "<<k<<endl;
    return result;
}

double calcNMI_real(Community commsX,Community commsY)
/*
    calculate Overlapping NMI
    require:?????????? 怎样的两个社团可以拿来比？需要做什么预处理。
            目前需要
            不管原来社团集合中是否允许存在不在任何社团中的点，传入的两个社团必须做到每个点至少在一个社团中，
            并且每个社团的size就是处理后的size。
    return:
            Overlapping NMI = 1 - 0.5*[H(X|Y) + H(Y|X)]
*/
{
    if (commsX.N <= 0 || commsY.N <= 0)//如果两个社团集合之一是空集，则认为它们完全不相似。
        //事实上，能做比较的两个社团集合所具有的顶点数应当相等
    {
        cout<<"info: one of the two communities set is empty"<<endl;
        return 0;
    }
    //else if (commsX.N != commsY.N)
    //{
    //    fprintf(stderr, "error: the vertex number of two community sets are not equal");
    //}
    else
    {
        double H_X_in_Y = Conditional_Entropy(commsX, commsY);
        double H_Y_in_X = Conditional_Entropy(commsY, commsX);
        return (1 - 0.5*(H_X_in_Y + H_Y_in_X));// 1 - 0.5*[H(X|Y) + H(Y|X)]}
    }
}

double calcNMI(string commsX_file,string commsY_file)
//对用户提供的计算overlapping NMI的接口
{
    //cout<<commsX_file<<' '<<commsY_file<<endl;
    if ( commsX_file == commsY_file )
        return 1.0;
    Community X;
    X.load(commsX_file);
    Community Y;
    Y.load(commsY_file);
    if (config.find("Metric_CommunitySizeThres")!=config.end())
    {
        X.removeSmallComms(atoi(config["Metric_CommunitySizeThres"].c_str()));
        Y.removeSmallComms(atoi(config["Metric_CommunitySizeThres"].c_str()));
    }
    double score=calcNMI_real(X,Y);//调用真正计算Overlapping NMI的接口,返回OverlappingNMI值
    cout<<commsX_file<<" vs "<<commsY_file<<" Overlapping NMI "<<score<<endl;
    return score;//返回OverlappingNMI值给用户
}

double calcEntropy(Community comm)//commnities set's Entropy
{
    double E=0;
    for (int i=1; i<=comm.NC; i++)
    {
        double p=comm.id[i].size()/(double)(comm.N);
        if (p>0) E-=p*log(p);
    }
    //cout<<"Entropy"<<E<<endl;
    return E;
}
