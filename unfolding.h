//
// Created by leo on 22-6-4.
//

#ifndef PUNF_UNFOLDING_H
#define PUNF_UNFOLDING_H

#include "cpn.h"
#include "iostream"
#include "vector"
#include <algorithm>
#include <iomanip>//控制cout输出宽度
#include "time.h"
#include "synchronization.h"
typedef vector<index_t> type_D;
typedef set<index_t> X,Y,type_t_set,type_e_set ;
typedef unordered_map<token, token_count_t, tokenHasher, tokenEqualto> Token_map;
typedef set<type_T2Call> type_cftPath;//conflictPath
enum case_t{NONE,CASE1,CASE2};//counterexample类型
enum NodChild{RootNod,LChildNod,RChildNod};


class Binding_unf {
public:
    string variable;
    token value;
    Binding_unf *next;

    Binding_unf() { value= NULL; }
};

//typedef struct UNF_Small_Arc {
//    index_t idx;
//} USArc;

typedef class UNF_Condition {
public:
    string id;
    int cpn_index;//映射在cpn中对应的库所
    MultiSet multiSet;
    index_t hashvalue;
    set<index_t >producer;
    set<index_t >consumer;
} UCondition;

typedef class UNF_Event {
public:
    string id;
    int cpn_index;
    set<index_t > producer;
    set<index_t > consumer;
    bool is_cutoff;
    index_t cutoff_e_idx;
//    string call;//记录e对应的调用变迁，用（e，call）划分e所属线程;
    type_callStack call_stack;//调用顺序由front到back;back是当前所在函数;

//    bool is_read=false;//读事件标识
    set<index_t> read_varB;//读事件输入变量条件；

    UNF_Event(){is_cutoff= false;}
} UEvent;

typedef class UNF_Arc {
public:
    string source_id;
    string target_id;
    condition_tree arc_exp;
    Arc_Type arcType;
} UArc;

class Cex_texture{
public:
    type_T2Call t2Call;
    type_callStack Call;//conflictT的Call
    set<index_t > conflictT;
    type_cftPath path;//[冲突变迁]\C   <<t_idx,call>,可发生次数>

//    set<type_T2Call> cutoff_fired;//若A非空，则cutoff事件应当允许发生一次；

    Cex_texture(type_T2Call t2Call,type_Call2T call2T,type_cftPath path){
        this->t2Call=t2Call;this->Call=call2T.first;this->conflictT=call2T.second;this->path=path;}
//    void operator=(const Cex_texture cexA_texture);
    bool operator==(const Cex_texture &cexTexture);
};

typedef vector<Cex_texture> Cex_A;//下标越大的路径优先权越高。

class Cft_Tpath{
public:
    CPN *cpn;
    type_cftPath t_path;//冲突路径
    pair<type_T2Call, type_multimapCall2T> t2cftT;//原变迁-->冲突变迁
    set<type_T2Call >en_t2Call;
    Cex_A *cexA;

    Cft_Tpath(CPN *cpn,pair<type_T2Call, type_multimapCall2T> t2cftT,set<type_T2Call >en_t2Call,Cex_A *cexA){
        this->cpn=cpn;this->t2cftT=t2cftT;this->en_t2Call=en_t2Call;this->cexA=cexA;
    }
    bool searched_Add(type_Call2T call2T,vector<type_T2Call> &searched,index_t t_idx,type_callStack call_cur);
    type_cftPath get_t_path_step(type_Call2T call2T);
};

class CUTINFO{
public:
    type tid;
    SORTID sid;
    unordered_multimap<index_t,token> maptoken;//<c_idx,token>
    unordered_multimap<unsigned int,std::shared_ptr<SortValue>>::iterator getonlytoken(){if(maptoken.size() != 1) throw "getonlytoken must have one token";return maptoken.begin();}
    bool operator==(const CUTINFO &cuttable);
};
class CUT{
public:
    int e_idx;
    NUM_t placecount;
    NUM_t conditioncount;
    CUTINFO * cuttable;//数组按顺序映射每个CPN库所

    index_t hash(vector<UCondition> condition);
    bool exist_condition(NUM_t c_idx,NUM_t p_idx);
    bool add_condition(NUM_t c_idx,NUM_t p_idx,token token);//c_idx : conditon_idx,p_idx : cpn_place_idx
    bool sub_condition(NUM_t c_idx,NUM_t p_idx,token token);//不存在返回ture，删除失败返回false;
    void plus_cuttable(const CUTINFO *cuttable);
    void minus_cuttable(const CUTINFO *cuttable);
    void operator=(const CUT &CUT);
    bool operator==(const CUT &CUT);
    CUT(CPN* cpn);
    ~CUT(){delete[] cuttable;}
};

typedef class Possible_Extend{
public:
    index_t t_idx;
    index_t e_idx;
    X x;
    Binding_unf *binding;
    bool had_extend;
    Possible_Extend(index_t t_idx,Binding_unf *binding){ this->t_idx=t_idx; this->binding=binding;had_extend= false;}
//    type_call call;//栈，记录e对应的调用变迁;从外层到内层调用从前到后排列；main函数变迁call为"main"
    type_callStack call_stack;//调用顺序由front到back;back是当前所在函数;
}PE;

typedef class Configuration{
public:
    vector<bool> e_Vec;//[e],大小为eventcount,在配置内为1,不在为0，用于增量计算,只用于局部配置
    set<string> fireable_T;
    vector<PE *> en;//enabled_t;
    index_t e_last;//最后加入配置的事件，新的扩展只需在该事件之后，其他都已经扩展过。
    bool is_accept;//BA状态是否为接受状态。
    bool is_cutoff= false;
    set<index_t > fired_T;
    //context
    index_t q_cur;//heuristic;
    CUT * conf_pre;//·[e]
    CUT * conf_con;//[e]·
    CUT * cut;//cut([e])
    //alt
    bool firstStep= true;//应对特殊情况：冲突路径还未发生，但C->e_last已经是冲突变迁，会立即返回；因此用该变量让冲突路径至少发生一个变迁再计算冲突路径探索完成

    Configuration(){is_accept= false;cut =NULL;conf_pre=NULL;conf_con=NULL;}
    void operator=(const Configuration &C);
    bool containT(index_t t_idx);
    bool fireable(string s);//根据C的可能扩展，判定Fireability命题（自动机变迁guard）
}Config;

class Nod{
public:
    string id;
    Config *C;
    type_D D;
    set<type_T2Call> D_t2call;
    Cex_A *cexA;
    index_t e_idx=65535;
    index_t cftRow;
    bool has_Cft= false;

    Nod(Config *C,type_D D,Cex_A *cexA){
        this->C=C;this->D=D;this->cexA=cexA;
    }
    Nod * LChild=NULL;
    Nod * RChild=NULL;
};

class UNFPDN{
public:
    vector<UCondition> condition;
    vector<UEvent> event;
    vector<UArc> arc;
    NUM_t conditioncount;
    NUM_t eventcount;
    NUM_t arccount;
    CUT* min_o;
    unordered_multimap<int ,CUT*> map_cut_all;//保存所有出现过的[e]的cut
    unordered_multimap<int ,pair<CUT*,Cex_A*>> map_cut_C;//保存所有C的cut；
    case_t success;//标记是否找到counterexample;
    set<index_t > G;//回收站

    //context
    vector<vector<int>> matrix_Cob;//concurrency matrix of conditions
    vector<Config *> table_config;//局部配置表

    void init(CPN *cpn);
    void add_Condition(string id,index_t cpn_index,token token);
    //bool add_Event_L(CPN *cpn,SYNCH *synch,Config *C,index_t t_idx, X x, Binding_unf *binding);//先判断L能否发生，可能不扩展
    bool add_Event(CPN *cpn,SYNCH *synch,Nod *nod,PE *pe);//扩展非L事件
    void add_Arc(string source_id,string target_id);
    void cal_Matrix_Cob(X x);//新增一个条件时，增量计算它与其他条件的并发关系，加入matrix_Cob
    void cal_Config(X x,set<index_t> &xPro);
    void cal_context(CPN* cpn,UEvent e,set<index_t> xPro,Cex_A *cexA);
    void cal_call(CPN *cpn,SYNCH *synch ,PE *pe);
    bool is_fireable_L(CPN *cpn,SYNCH *synch,X &x,Y &y, Binding_unf *binding);
    bool is_cut_off(index_t &e_same_cut);
    bool cal_is_cutoff_C(Config *C,Cex_A *cexA);
    bool is_counterexample(SYNCH *synch,Config *C, index_t e_cutoff);
    bool is_counterexample_endpoint(Config *C);
    bool is_conc(CPN *cpn,index_t e1,index_t e2);
    bool is_causal(index_t e1,index_t e2);
    //bool had_extend_T(Config *C,set<index_t> x);
    bool had_extend(CPN *cpn,Config *C,PE *pe,bool is_readT=false,set<index_t > read_varP={});
    set<index_t> delete_VarInX(PE *pe,CPN *cpn,set<index_t> readP);//返回删除的变量条件
    void print_UNF(string filename, CPN *cpn);
};
class UNFOLDING{
public:
    SYNCH *synch;
    UNFPDN *unfpdn;
    CPN *cpn;
    vector<Nod *> nods;
    index_t nodsCount=0;
    string result;

    //time
    double MaxTime=300;
    clock_t elapse_begin,elapse_end;
    bool timeup=false;

    void getsynch(SYNCH *synch);
    void init();
    void unfolding();
    Nod * createNod(Config *C,type_D D,Cex_A *cexA,NodChild isChild,Nod *PNod=NULL);
    void Extend(Config *C);//查找可能扩展，但不发生
    void Explore(Nod *nod);//A:alt   At:cex
    void ExtendFire(Config *C,PE *pe);//一个变迁对应的可能扩展;
    bool is_firable(index_t idx,Config *C,vector<Binding_unf *> &bindings);
    Config *get_Config_add(Config * C,index_t e,Cex_A *cexA);
    bool imm_conflict(UEvent e1,PE *pe);
    bool imm_conflict_var(UEvent e1,index_t t_idx);
    bool Alt0(Config *C,type_D D,index_t &e_alt);
    bool Alt(Config *C,type_D D,Cex_A *cexA_plus,Cex_A *cex_A);
    void Remove(index_t,Config *C,type_D D);

    bool handleLTLF(string s,Config *C);
    bool handleLTLC(string s, Config *C);
    bool handleLTLV(string s,Config *C);
    void handleLTLCstep(short int &front_sum, short int &latter_sum, string s, Config *C);
    short int sumtoken(string s, Config *C);   //计算s中所有库所的token和

    //Auxiliary
    bool needNo_Explore(Nod *nod,PE *pe);
    type_cftPath get_t_path(type_D D,Cex_A *cexA,pair<type_T2Call ,type_multimapCall2T> t2conflictT,set<type_T2Call> en_T2Call,type_Call2T &call2T);
    void print_ExploreTree(string filename);
    bool FetchColor(string s,Config *C,bucket &color);
};
#endif //PUNF_UNFOLDING_H
