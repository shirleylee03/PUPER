//
// Created by leo on 2021/11/3.
//

#ifndef PUNF_SYNCHRONIZATION_H
#define PUNF_SYNCHRONIZATION_H

#include <iostream>
#include <string>
#include "cpn.h"
#include "product.h"
#include "BA/buchi.h"
using namespace std;

//extern enum LTLCategory{LTLC,LTLF,LTLV};
//string ba_tokenvalue="ba_thr";//ba的thread不参与绑定
class SYNCH{
public:
    CPN* cpn_synch;

//    vector<index_t > P,T;//criteria 从公式中提取
    NUM_t q_begin_idx;
    NUM_t q_count;//不包含B，S库所,B、S在队尾
    NUM_t ba_t_begin_idx;
    NUM_t ba_t_count;
    NUM_t u_count;//自动机transition有三种u,I,L
    set<int> I_idx;//保存I变迁在synch_cpn中的idx
    set<int> L_idx;//保存L变迁在synch_cpn中的idx
    set<int> visible_P;
    set<int> visible_T;
    map<int,int> map_state2place;
    map<int,int> map_place2state;
    map<int,set<string>>map_t2predicates;//保存所有predicates
    map<int,set<string>>map_t2guard;//保存包含可视库所、可视变迁的predicates的BA_transition_idx_in_cpn和predicates
    set<int> state_accepted;
    set<index_t > CP_tran;//CP变迁集合
    map<index_t,index_t> map_T2CP;//原T变迁与对应CP变迁，便于快速查找
    void ba2cpn(LTLCategory type,int num,CPN* cpn);
    void synch();
    void clear_producer_consumer(CPN *cpn_synch);
    int judgeF(string s);
//    void GetLTL(ofstream &out,TiXmlElement *p);
//    void extract_targetPT(int number);
    set<index_t> get_observesLTLF(string s);
    set<index_t> get_observesLTLV(string s);
    void add_copy_obs(index_t BA_t,set<index_t > obs_T);//LTLF公式将guard中的T复制一份到BA变迁，用于使能判断，防止调度库所的存在导致I和T无法同时使能
    void Add_selfloop(int idx);
};
#endif //PUNF_SYNCHRONIZATION_H