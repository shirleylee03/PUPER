//

// Created by leo on 22-6-4.
//
#include "unfolding.h"

extern string tid_str,ba_thr;
extern const TID_t init_tid = "\'main\'";

vector<String_t> merge_vec_unf(vector<String_t> vec1, vector<String_t> vec2) {
    //因ba的控制库所的初始token与pdn网内控制库所的token不同，故需特殊处理
    if (vec1.empty())
        return vec2;
    if (vec2.empty()) {
        vec1.clear();
        return vec1;
    }
    //ba+
//    if (*vec1.begin() == "'ba_thr'")
//        return vec2;
//    else if (count(vec2.begin(), vec2.end(), "'ba_thr'"))
//        return vec1;
    //ba-
    //    bool finded_flag = false;
    for (auto iter = vec1.begin(); iter != vec1.end();) {
        if (exist_in(vec2, *iter))
            iter++;
        else
            iter = vec1.erase(iter);
    }
    return vec1;
}

Binding_unf *bindingcid_unf(Product_t cid, SORTID sid, condition_tree_node *tokennode) {
    condition_tree_node *node = tokennode->left;
    int offset = 0;
    Binding_unf *newbinding, *result;
    result = new Binding_unf;
    result->next = NULL;
    auto ps = sorttable.find_productsort(sid);
    auto sortid = ps.get_sortid();
    while (node->right) {
        if (node->left->node_type == variable) {
            if (node->node_name != tid_str) {
                newbinding = new Binding_unf;
                newbinding->variable = node->left->value;
                if (sortid[offset].tid == Integer)
                    newbinding->value = (token) (new IntegerSortValue);
                else if (sortid[offset].tid == Real)
                    newbinding->value = (token) (new RealSortValue);
                else if (sortid[offset].tid == String)
                    newbinding->value = (token) (new StringSortValue);
                else {
                    cerr << "ERROR!variable can not be a productsort!" << endl;
                    throw "ERROR!variable can not be a productsort!";
                    //exit(-1);
                }
                Bucket bkt;
                cid[offset]->getcolor(bkt);
                newbinding->value->setcolor(bkt);
                //                color_copy(sortid[offset].tid, sortid[offset].sid, cid[offset], newbinding->value);
                newbinding->next = result->next;
                result->next = newbinding;
            }
        }
        offset++;
        node = node->right;
    }
    if (node->node_type == variable) {
        if (node->node_name != tid_str) {
            newbinding = new Binding_unf;
            newbinding->variable = node->value;
            if (sortid[offset].tid == Integer)
                newbinding->value = (token) (new IntegerSortValue);
            else if (sortid[offset].tid == Real)
                newbinding->value = (token) (new RealSortValue);
            else if (sortid[offset].tid == String)
                newbinding->value = (token) (new StringSortValue);
            else {
                cerr << "ERROR!variable can not be a productsort!" << endl;
                throw "ERROR!variable can not be a productsort!";
                //exit(-1);
            }
            Bucket bkt;
            cid[offset]->getcolor(bkt);
            newbinding->value->setcolor(bkt);
            //            color_copy(sortid[offset].tid, sortid[offset].sid, cid[offset], newbinding->value);
            newbinding->next = result->next;
            result->next = newbinding;
        }
    }
    return result;
}

Binding_unf *bindingToken(condition_tree_node *node, CUTINFO *multiset, TID_t tid) {

    SORTID sid;
    bool hasindex, hastid;
    Binding_unf *result, *tmpbinding;
    result = new Binding_unf;
    result->next = NULL;

    if (multiset->maptoken.size() == 0)
        return result;

    //binding Integer, for alloc and mutex cond
    if (multiset->tid == Integer) {
        if (node->left->node_name[0] == '_' || isalpha(node->left->node_name[0])) {
            result->next = new Binding_unf;
            result->next->next = NULL;
            result->next->variable = node->left->node_name;
            result->next->value = (token) (new IntegerSortValue);
            Bucket bkt;
            auto token = multiset->getonlytoken();
            token->second->getcolor(bkt);
            result->next->value->setcolor(bkt);
//            color_copy(Integer, 0, tokens->color, result->next->value);
            return result;
        } else
            return result;
    }

    sid = multiset->sid;
    auto ps = sorttable.find_productsort(sid);
    hasindex = ps.get_hasindex();
    hastid = ps.get_hastid();

    int offset = 0;
    token cid;
    cid = (token) (new ProductSortValue(sid));//fpro.generateSortValue(sid);
    Integer_t index;
    condition_tree_node *indexnode, *tidnode;
    indexnode = tidnode = node->left;

    auto tokens = multiset->maptoken;
    if (hasindex && hastid) {
        tidnode = tidnode->right->right;
        while (tidnode->right) {
            indexnode = indexnode->right;
            tidnode = tidnode->right;
            offset++;
        }
        index = atoi(indexnode->left->value.c_str());
        auto token = tokens.begin();
        while (token != tokens.end()) {
            Bucket cid_bkt, tid_bkt, index_bkt;
            token->second->getcolor(cid_bkt);
            cid->setcolor(cid_bkt);
            TID_t sub_tid;
            Integer_t sub_index;
            cid_bkt.pro[offset - 1 + 1]->getcolor(index_bkt);
            cid_bkt.pro[offset - 1 + 3]->getcolor(tid_bkt);
            sub_index = index_bkt.integer;
            sub_tid = tid_bkt.str;
            if (sub_index == index && tid == sub_tid) {
                tmpbinding = bindingcid_unf(cid_bkt.pro, sid, node);
                Binding_unf *end = tmpbinding;
                while (end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if (token == tokens.end())
            throw "ERROR!can't binding correctly!";
    } else if (hasindex) {
        tidnode = tidnode->right->right;
        while (tidnode) {
            indexnode = indexnode->right;
            tidnode = tidnode->right;
            offset++;
        }
        index = atoi(indexnode->left->value.c_str());
        auto token = tokens.begin();
        while (token != tokens.end()) {
            Bucket cid_bkt, index_bkt;
            token->second->getcolor(cid_bkt);
            Integer_t sub_index;
            cid_bkt.pro[offset - 1 + 1]->getcolor(index_bkt);
            sub_index = index_bkt.integer;
            if (sub_index == index) {
                tmpbinding = bindingcid_unf(cid_bkt.pro, sid, node);
                Binding_unf *end = tmpbinding;
                while (end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if (token == tokens.end())
            throw "ERROR!can't binding correctly!";
    } else if (hastid) {
        tidnode = tidnode->right;
        while (tidnode->right) {
            tidnode = tidnode->right;
            offset++;
        }
        auto token = tokens.begin();
        while (token != tokens.end()) {
            Bucket cid_bkt, tid_bkt;
            token->second->getcolor(cid_bkt);
            TID_t sub_tid;
            cid_bkt.pro[offset - 1 + 2]->getcolor(tid_bkt);
            sub_tid = tid_bkt.str;
            if (sub_tid == tid) {
                tmpbinding = bindingcid_unf(cid_bkt.pro, sid, node);
                Binding_unf *end = tmpbinding;
                while (end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if (token == tokens.end())
            throw "ERROR!can't binding correctly!";
    } else {
        tidnode = tidnode->right;
        while (tidnode) {
            tidnode = tidnode->right;
            offset++;
        }
        auto token = tokens.begin();
        while (token != tokens.end()) {
            Bucket cid_bkt;
            token->second->getcolor(cid_bkt);
            {
                tmpbinding = bindingcid_unf(cid_bkt.pro, sid, node);
                Binding_unf *end = tmpbinding;
                while (end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            token++;
        }
        if (token == tokens.end())
            throw "ERROR!can't binding correctly!";
    }
    //    delete[] cid;
    return result;
}

vector<Binding_unf *> get_bindings(CPN *cpn, CTransition *transition, CUT *cut, UNFPDN *unfpdn, SYNCH *synch) {
    //Binding control places first
    //transition's happen or not depends on control places

    vector<Binding_unf *> bindings;
    Binding_unf *binding, *tmpbinding;

    vector<TID_t> possible, tmp_vec;
    auto t_idx = cpn->mapTransition.find(transition->getid())->second;
    auto T_producer = transition->get_producer();

    bool all_ba= true;//检查前集是否都是ba库所
    for (int i = 0; i < T_producer.size(); i++) {
        index_t idx = T_producer[i].idx;
        if (idx >= synch->q_begin_idx ) {
            //如果cut中对应BA库所不存在token，则绑定失败。
            if (cut->cuttable[idx].maptoken.empty())
                return bindings;
            continue;
        } else{
            all_ba=false;
        }

        condition_tree_node *root = T_producer[i].arc_exp.getroot();//transition->producer[i].arc_exp.root;
        auto pp = cpn->findP_byindex(idx);
        if (pp->getiscontrolP()) {
            if (pp->gettid() == TID_colorset) {
                if (root->node_type == Token) {
                    if (root->left->node_type == variable) {
                        // arc_exp is 1'tid
                        string var = root->left->value;
                        //                        auto vv = cpn->findVar_byname(var);
                        //                        auto iter = cpn->mapVariable.find(var);
                        //                        if(iter != cpn->mapVariable.end()){
                        TID_t cid;
                        //                            Tokens *token = marking.mss[idx].tokenQ->next;
                        auto tokens = cut->cuttable[idx].maptoken;
                        auto token = tokens.begin();
                        tmp_vec.clear();
//                        for (auto j = cut->b_idx_vec.begin(); j != cut->b_idx_vec.end(); j++) {
//                            auto b_idx = *j;
//                            if (unfpdn->place[b_idx].cpn_index != idx)
//                                continue;
//                            auto token = unfpdn->place[b_idx].token_record.getonlytoken();
//                            Bucket bkt;
//                            token->getcolor(bkt);
//                            if (TID_colorset != String)
//                                throw "here we assume TID_colorset is String!";
//                            tmp_vec.push_back(bkt.str);
//                        }
                        while (token != tokens.end()) {
                            Bucket bkt;
                            token->second->getcolor(bkt);
                            if (TID_colorset != String)
                                throw "here we assume TID_colorset is String!";
                            tmp_vec.push_back(bkt.str);
                            token++;
                        }
                        possible = merge_vec_unf(possible, tmp_vec);
                        if (possible.empty()) {
                            return bindings;
                        }
                        //                        else {
                        //                            cout << "cpn_rg.cpp can't find variable" << endl;
                        //                            exit(-1);
                        //                        }
                    } else {
                        // arc_exp is color or case
                        //                        TID_t cid1,cid2;
                        //                        Tokens *token = marking.mss[idx].tokenQ->next;
                        MultiSet tmp_ms;
                        bool finded = false;
                        //                        tmp_ms.tid = TID_colorset;
                        //                        tmp_ms.sid = 0;
                        auto tokens = cut->cuttable[idx].maptoken;
                        auto token = tokens.begin();

                        cpn->CT2MS(T_producer[i].arc_exp, tmp_ms, TID_colorset, 0);
                        while (token != tokens.end()) {
                            Bucket bkt1, bkt2;
                            token->second->getcolor(bkt1);
//                            tmp_ms.Exp2MS(cpn,root,0,0,false);
//                            tmp_ms.tokenQ->next->color->getColor(cid2);
                            tmp_ms.getonlytoken()->getcolor(bkt2);
                            if (bkt1 == bkt2) {
                                finded = true;
                                break;
                            }
                            token++;
                        }
                        if (finded)
                            continue;
                        else
                            return bindings;
                    }
                } else
                    throw "ERROR!control places' read arc just have one token!";
            } else if (pp->gettid() == Integer) {
                //mutex cond
                //Tokens *token = marking.mss[idx].tokenQ->next;
                auto tokens = cut->cuttable[idx].maptoken;
                auto token = tokens.begin();
                Integer_t cid_arc = atoi(root->left->node_name.c_str());
                Bucket bkt_arc;
                bkt_arc.tid = Integer;
                bkt_arc.integer = cid_arc;
                while (token != tokens.end()) {
                    Bucket bkt_place;
                    token->second->getcolor(bkt_place);
                    if (bkt_arc == bkt_place)
                        break;
                    token++;
                }
                if (token == tokens.end())
                    return bindings;//empty return
            }
        }
    }
    if(all_ba){
        possible.emplace_back(ba_thr);
    }

    for (int i = 0; i < possible.size(); i++) {
        Bucket bkt_tid;
        String_t tid = possible[i];
        bkt_tid.tid = String;
        bkt_tid.str = tid;
        binding = new Binding_unf;
        binding->next = new Binding_unf;
        binding->next->variable = tid_str;
        binding->next->value = (token) (new StringSortValue);
        binding->next->value->setcolor(bkt_tid);
        binding->next->next = NULL;
        auto T_producer = transition->get_producer();
        for (unsigned int j = 0; j < T_producer.size(); j++) {
            index_t idx = T_producer[j].idx;
            auto pp = cpn->findP_byindex(idx);
            if (!pp->getiscontrolP()) {
                condition_tree_node *root = T_producer[j].arc_exp.getroot();
                while (root) {
                    if (root->node_type == CaseOperator && !root->right) {
                        root = root->left;
                        continue;
                    }
                    if (root->node_type == Token) {

                        tmpbinding = bindingToken(root, &cut->cuttable[idx], tid);
                        if (tmpbinding->next) {
                            Binding_unf *end = tmpbinding->next;
                            while (end->next)
                                end = end->next;
                            end->next = binding->next;
                            binding->next = tmpbinding->next;
                            delete tmpbinding;
                        }
                    }
                    if (root->left->node_type == Token) {

                        tmpbinding = bindingToken(root->left, &cut->cuttable[idx], tid);
                        if (tmpbinding->next) {
                            Binding_unf *end = tmpbinding->next;
                            while (end->next)
                                end = end->next;
                            end->next = binding->next;
                            binding->next = tmpbinding->next;
                            delete tmpbinding;
                        }
                    }
                    root = root->right;
                }
            }
        }
        bindings.push_back(binding);
    }
    return bindings;
}

void BindingVariable(const Binding_unf *binding, CPN *cpn) {
    Binding_unf *tmp = binding->next;
    while (tmp) {
        auto vv = cpn->findVar_byname(tmp->variable);
        //        auto viter = cpn->mapVariable.find(tmp->variable);
        //        if(viter == cpn->mapVariable.end()){
        //            cerr<<"ERROR!BindingVariable failed!"<<endl;
        //            exit(-1);
        //        }
        //        color_copy(cpn->vartable[viter->second].tid,cpn->vartable[viter->second].sid,tmp->value,cpn->vartable[viter->second].value);
        Bucket bkt;
        tmp->value->getcolor(bkt);
        vv->setvcolor(bkt);
        tmp = tmp->next;
    }
}

set<index_t> get_x(CTransition *t, CUT *cut, Binding_unf *binding, CPN *cpn) {
    set<index_t> x;
    BindingVariable(binding, cpn);

    auto T_producer = t->get_producer();
    for (unsigned int i = 0; i < T_producer.size(); i++) {
        MultiSet ms;
        int up;
        index_t idx = T_producer[i].idx;
        auto pp = cpn->findP_byindex(idx);
        cpn->CT2MS(T_producer[i].arc_exp, ms, pp->gettid(), pp->getsid());
        auto mapTokens = ms.getmapTokens();
        if (!mapTokens.empty()) {
            for (auto j = mapTokens.begin(); j != mapTokens.end(); j++) {
                int count = j->second;
                Bucket bkt1, bkt2;
                j->first->getcolor(bkt1);
                auto tokens = cut->cuttable[idx].maptoken;
                auto token = tokens.begin();
                while (token != tokens.end()) {
                    token->second->getcolor(bkt2);
                    if (bkt1 == bkt2) {
                        x.emplace(token->first);
                        count--;
                        if (count < 1)
                            break;
                    }
                        //ba+
                    else if (bkt2.str == "'ba_thr'") {
                        x.emplace(token->first);
                        count--;
                        if (count < 1)
                            break;
                    }
                    //ba-
                    token++;
                }
            }
        }
    }
    return x;
}

Cex_A *cexA_sub(Cex_A *cexA, type_T2Call t2call) {
    return cexA;

//    Cex_A *cexA_minus=new Cex_A ;
//    *cexA_minus=*cexA;
//    if(cexA_minus->empty())
//        return cexA_minus;
//    for (auto i = cexA_minus->back().path.begin(); i != cexA_minus->back().path.end();) {
//        if (i->first == t2call) {
//            if(i->second>1){
//                i->second--;
//                i++;
//            } else {
//                i = cexA_minus->back().path.erase(i);
//            }
//        }
//        else i++;
//    }
//    return cexA_minus;
}

//type_e_set vec_add(type_e_set D, index_t e) {
//    type_e_set D_add = D;
//    if (!exist_in(D_add, e))
//        D_add.push_back(e);
//    return D_add;
//}

bool CUTINFO::operator==(const CUTINFO &cuttable) {
    if (maptoken.size() != cuttable.maptoken.size())
        return false;
    auto tokens = cuttable.maptoken;
    auto token = tokens.begin();
    while (token != tokens.end()) {
        Bucket bkt1, bkt2;
        token->second->getcolor(bkt1);
        auto iter = maptoken.begin();
        for (; iter != maptoken.end(); iter++) {
            iter->second->getcolor(bkt2);
            if (bkt1 == bkt2)
                break;
        }
        if (iter == maptoken.end())
            return false;
        token++;
    }
    return true;
}

index_t CUT::hash(vector<UCondition> condition) {
    index_t hv = 0;
    for (auto i = 0; i < placecount; i++) {
        for (auto j = cuttable[i].maptoken.begin(); j != cuttable[i].maptoken.end(); j++)
            hv += condition[j->first].hashvalue;
    }
    return hv;
}

bool CUT::exist_condition(NUM_t c_idx, NUM_t p_idx) {
    if (cuttable[p_idx].maptoken.find(c_idx) != cuttable[p_idx].maptoken.end())
        return true;
    else return false;
}

bool CUT::add_condition(NUM_t c_idx, NUM_t p_idx, token token) {
    if (exist_condition(c_idx, p_idx))
        return false;
    cuttable[p_idx].maptoken.emplace(c_idx, token);
    conditioncount++;
    return true;
}

bool CUT::sub_condition(NUM_t c_idx, NUM_t p_idx, token token) {
    if (!exist_condition(c_idx, p_idx))
        return false;
    auto iter = cuttable[p_idx].maptoken.find(c_idx);
    cuttable[p_idx].maptoken.erase(iter);
    conditioncount--;
    return true;
}

void CUT::plus_cuttable(const CUTINFO *cuttable) {
    for (auto i = 0; i < placecount; i++) {
        auto tokens = cuttable[i].maptoken;
        auto token = tokens.begin();
        while (token != tokens.end()) {
            add_condition(token->first, i, token->second);//自带去重
            token++;
        }
    }
}

void CUT::minus_cuttable(const CUTINFO *cuttable) {
    for (auto i = 0; i < placecount; i++) {
        auto tokens = cuttable[i].maptoken;
        auto token = tokens.begin();
        while (token != tokens.end()) {
            sub_condition(token->first, i, token->second);//自带去重
            token++;
        }
    }
}

bool CUT::operator==(const CUT &CUT) {
    if (conditioncount != CUT.conditioncount)
        return false;
    for (int i = 0; i < placecount; i++) {
        if (!(cuttable[i] == CUT.cuttable[i]))
            return false;
    }
    return true;
}

void CUT::operator=(const CUT &CUT) {
//    conditioncount = CUT.conditioncount;
    plus_cuttable(CUT.cuttable);
}

CUT::CUT(CPN *cpn) {
    conditioncount = 0;
    placecount = cpn->get_placecount();
    cuttable = new CUTINFO[placecount];
    for (index_t i = 0; i < placecount; i++) {
        cuttable[i].tid = cpn->findP_byindex(i)->gettid();
        cuttable[i].sid = cpn->findP_byindex(i)->getsid();
    }
}

void Configuration::operator=(const Configuration &C) {
    e_Vec = C.e_Vec;
    is_accept = C.is_accept;
    fired_T = C.fired_T;
    q_cur = C.q_cur;
}

bool Configuration::containT(index_t t_idx) {
    if (fired_T.find(t_idx) != fired_T.end())
        return true;
    else return false;
}

bool Configuration::fireable(string s) {
    if (fireable_T.find(s) != fireable_T.end())
        return true;
    return false;
}

void UNFPDN::init(CPN *cpn) {
    conditioncount = eventcount = arccount = 0;
    min_o = new CUT(cpn);
    success = NONE;
}


void UNFPDN::add_Condition(string id, index_t cpn_index, token token) {
    UCondition b;
    b.id = id;
    b.cpn_index = cpn_index;
    b.multiSet.generateFromToken(token);
    b.hashvalue = cpn_index * b.multiSet.Hash() + cpn_index;
    condition.push_back(b);
    conditioncount++;
}

void UNFPDN::cal_Matrix_Cob(X x) {
    vector<int> cob;
    for (auto col = 0; col < matrix_Cob.size(); col++) {
        bool result = 1;
        if (!condition[col].producer.empty()) {//若输入条件相同，则为并发关系
            if (*condition[col].producer.begin() == eventcount)
                result = 1;
        } else {
            for (auto row = x.begin(); row != x.end(); row++) {
                result = result && matrix_Cob[*row][col];
            }
        }
        matrix_Cob[col].push_back(result);
        cob.push_back(result);
    }
    cob.push_back(0);
    matrix_Cob.push_back(cob);
}

void UNFPDN::cal_Config(X x, set<index_t> &xPro) {
    Config *config = new Config;
    for (auto i = x.begin(); i != x.end(); i++) {
        if (!condition[*i].producer.empty())
            xPro.emplace(*condition[*i].producer.begin());
    }
    vector<bool> e_vec;
    if (!xPro.empty()) {
        for (auto col = 0; col < table_config.size(); col++) {
            int result = 0;
            for (auto row = xPro.begin(); row != xPro.end(); row++) {
                //下三角保存配置集“矩阵”
                if (*row < col)
                    continue;
                result = result || table_config[*row]->e_Vec[col];
            }
            e_vec.push_back(result);
        }
        e_vec.push_back(1);
        config->e_Vec = e_vec;
        table_config.push_back(config);
    } else {
        for (auto col = 0; col < table_config.size(); col++) {
            e_vec.push_back(0);
        }
        e_vec.push_back(1);
        config->e_Vec = e_vec;
        table_config.push_back(config);
    }
}

void UNFPDN::cal_context(CPN *cpn, UEvent e, set<index_t> xPro, Cex_A *cexA) {
    CUT *cut, *pre, *con;
    cut = new CUT(cpn);
    pre = new CUT(cpn);
    con = new CUT(cpn);
    cut->e_idx = eventcount;

    switch (xPro.size()) {
        case 0:
            cut->plus_cuttable(min_o->cuttable);
            for (auto i = e.consumer.begin(); i != e.consumer.end(); i++) {
                auto p_idx = condition[*i].cpn_index;
                auto token = condition[*i].multiSet.getonlytoken();
                cut->add_condition(*i, p_idx, token);
                con->add_condition(*i, p_idx, token);
            }
            for (auto i = e.producer.begin(); i != e.producer.end(); i++) {
                auto p_idx = condition[*i].cpn_index;
                auto token = condition[*i].multiSet.getonlytoken();
                cut->sub_condition(*i, p_idx, token);
                pre->add_condition(*i, p_idx, token);
            }
            break;
        case 1:
            cut->plus_cuttable(table_config[*xPro.begin()]->cut->cuttable);
            pre->plus_cuttable(table_config[*xPro.begin()]->conf_pre->cuttable);
            con->plus_cuttable(table_config[*xPro.begin()]->conf_con->cuttable);
            for (auto i = e.consumer.begin(); i != e.consumer.end(); i++) {
                auto p_idx = condition[*i].cpn_index;
                auto token = condition[*i].multiSet.getonlytoken();
                cut->add_condition(*i, p_idx, token);
                con->add_condition(*i, p_idx, token);
            }
            for (auto i = e.producer.begin(); i != e.producer.end(); i++) {
                auto p_idx = condition[*i].cpn_index;
                auto token = condition[*i].multiSet.getonlytoken();
                cut->sub_condition(*i, p_idx, token);
                pre->add_condition(*i, p_idx, token);
            }
            break;
        default:
            cut->plus_cuttable(min_o->cuttable);
            for (auto i = e.consumer.begin(); i != e.consumer.end(); i++) {
                auto p_idx = condition[*i].cpn_index;
                auto token = condition[*i].multiSet.getonlytoken();
                cut->add_condition(*i, p_idx, token);
                con->add_condition(*i, p_idx, token);
            }
            for (auto i = xPro.begin(); i != xPro.end(); i++) {
                cut->plus_cuttable(table_config[*i]->conf_con->cuttable);
                con->plus_cuttable(table_config[*i]->conf_con->cuttable);
            }
            for (auto i = e.producer.begin(); i != e.producer.end(); i++) {
                auto p_idx = condition[*i].cpn_index;
                auto token = condition[*i].multiSet.getonlytoken();
                cut->sub_condition(*i, p_idx, token);
                pre->add_condition(*i, p_idx, token);
            }
            for (auto i = xPro.begin(); i != xPro.end(); i++) {
                cut->minus_cuttable(table_config[*i]->conf_pre->cuttable);
                pre->plus_cuttable(table_config[*i]->conf_pre->cuttable);
            }
    }
    auto config = table_config.back();
    config->conf_pre = pre;
    config->conf_con = con;
    config->cut = cut;
}

void UNFPDN::cal_call(CPN *cpn,SYNCH *synch, PE *pe) {
    //cal_xPro
    set<index_t> xPro;
    enum {
        none, thread_begin, thread_enter
    } thr_enter;
    vector<string> thr_enterT;
    auto t = cpn->findT_byindex(pe->t_idx);
    auto t_id = t->getid();

    bool ismain = true;
    for (auto i = pe->x.begin(); i != pe->x.end(); i++) {

        if (condition[*i].producer.empty())
            continue;
        ismain = false;
        xPro.emplace(*condition[*i].producer.begin());

        //线程变迁需特殊处理，若x中包含thread_begin库所，xPro中包含thread_enter变迁，
        //则pe的call为thread_enter变迁，pe的call_stack中压入thread_enter变迁；
        if (thr_enter != thread_enter) {
            thr_enter = none;
            auto p = cpn->findP_byindex(condition[*i].cpn_index);
            for (auto ithr = cpn->map_thread2beginP_endP.begin(); ithr != cpn->map_thread2beginP_endP.end(); ithr++) {
                if (ithr->second.front() == p->getid())
                    thr_enter = thread_begin;
            }
            if (thr_enter == none)
                continue;
            auto t = cpn->findT_byindex(event[*condition[*i].producer.begin()].cpn_index);
            auto iT = cpn->set_thread_enterT.find(t->getid());
            if (iT != cpn->set_thread_enterT.end()) {
                thr_enter = thread_enter;
                thr_enterT.push_back(*iT);
            }
        }
    }
    if (ismain) {
        thr_enter = thread_enter;
        thr_enterT.push_back(mainCall);
    }

    //若是线程创建处，则栈中只有线程创建变迁
    if (thr_enter == thread_enter) {
//        pe->call = thr_enterT;
        pe->call_stack.push_back(thr_enterT);
    } else {
        set<index_t> t_exePro, e_exePro;//e的非变量前集库所前集变迁；e的非变量前集条件前集事件
        //call的计算中，e的前集条件应排除变量条件，因为可能导致call计算错乱；
        for (auto i = t->get_producer().begin(); i != t->get_producer().end(); i++) {
            auto p_tPro = cpn->findP_byindex(i->idx);
            if (cpn->is_varP(i->idx))
                continue;
            for (auto j = p_tPro->get_producer().begin(); j != p_tPro->get_producer().end(); j++) {
                if (j->arcType == control || j->arcType == executed ||
                    j->arcType == call_enter || j->arcType == call_exit) {
                    t_exePro.emplace(j->idx);
                }
            }
        }
        for (auto i = xPro.begin(); i != xPro.end(); i++) {
            auto t_idx_xPro = event[*i].cpn_index;
            if (t_exePro.find(t_idx_xPro) != t_exePro.end()) {
                e_exePro.emplace(*i);
            }
        }

        for (auto i = e_exePro.begin(); i != e_exePro.end(); i++) {
            auto ePro = event[*i];
            auto t_idx=ePro.cpn_index;
            if(t_idx>=synch->ba_t_begin_idx)
                continue;
            if (pe->call_stack.size() < ePro.call_stack.size()) {
                pe->call_stack = ePro.call_stack;
            }
        }
        for (auto i = e_exePro.begin(); i != e_exePro.end(); i++) {
            //若t是函数enter变迁,在call栈中压入t变迁;
            auto t_pro = cpn->findT_byindex(event[*i].cpn_index)->getid();
            if (cpn->map_call_enterT2exitP.find(t_pro) != cpn->map_call_enterT2exitP.end()) {
                vector<string> call;
                call.push_back(t_pro);
                pe->call_stack.push_back(call);
                break;
            }

                //若t是函数exit变迁，在call栈中弹出t对应的call变迁;
            else if (cpn->map_call_exitT2enterT.find(t_id) != cpn->map_call_exitT2enterT.end()) {
                pe->call_stack.pop_back();
                break;
            }
        }
    }
}

bool UNFPDN::is_cut_off(index_t &e_same_cut) {
    if (map_cut_all.empty())
        return false;

    auto hv = table_config.back()->cut->hash(condition);
    auto cut = table_config.back()->cut;
    auto cute_range = map_cut_all.equal_range(hv);
    if (cute_range.first == cute_range.second)
        return false;
    for (auto i = cute_range.first; i != cute_range.second; i++) {
        //[e′] ≺LTL [e] holds
        if (!table_config.back()->e_Vec[i->second->e_idx])
            continue;

        auto cut_exist = i->second;
        if (cut_exist->conditioncount != cut->conditioncount)
            continue;

        if (*cut_exist == *cut) {
            event.back().is_cutoff = true;
            e_same_cut = cut_exist->e_idx;
            return true;
        }
    }
    return false;
}

//A包含，要求从外层开始前缀相同。
bool coverA(Cex_A *cexA1, Cex_A *cexA2) {
    if (cexA1->size() < cexA2->size())
        return false;
    auto dis = cexA1->size() - cexA2->size();
    for (auto i1 = cexA1->begin() + dis, i2 = cexA2->begin(); i1 != cexA1->end(); i1++, i2++) {
        if (!(*i1 == *i2)) {
            return false;
        }
    }
    return true;
}

bool UNFPDN::cal_is_cutoff_C(Config *C, Cex_A *cexA) {
//    if(map_cut_C.empty())
//        return false;

    auto hv = C->cut->hash(condition);
    auto cut = C->cut;
    auto cut_range = map_cut_C.equal_range(hv);
    if (cut_range.first == cut_range.second) {
        map_cut_C.emplace(hv, make_pair(cut, cexA));
        return false;
    }
    for (auto i = cut_range.first; i != cut_range.second; i++) {
        auto cut_exist = i->second.first;
        if (cut_exist->conditioncount != cut->conditioncount)
            continue;

        if (*cut_exist == *cut) {
            if (coverA(cexA, i->second.second)) {
                C->is_cutoff = true;
                return true;
            }
        }
    }
    map_cut_C.emplace(hv, make_pair(cut, cexA));
    return false;
}

bool UNFPDN::is_counterexample(SYNCH *synch, Config *C, index_t e_cutoff) {
    /*case(I)*/
    if (!table_config.back()->e_Vec[e_cutoff])
        return false;
    set<int> e_loop;//保存[e]\[e′]
    for (auto i = 0; i < table_config.size(); i++) {
        if (table_config.back()->e_Vec[i] && !table_config[e_cutoff]->e_Vec[i]) {//查表得[e]\[e']
            e_loop.insert(i);
        }
    }
    //if[e]\[e′] contains an I-event then belong to case(I)
    if (table_config.back()->e_Vec[e_cutoff]) {//if e′< e   如果e'是e的祖先
        for (auto i = e_loop.begin(); i != e_loop.end(); i++) {
            if (synch->I_idx.find(event[*i].cpn_index) != synch->I_idx.end()) {
                success = CASE1;
                return true;
            }
        }
    }

    /*case(II)*/
    if (C->is_accept) {
        success = CASE2;
        return true;
    }
    return false;
}

bool UNFPDN::is_counterexample_endpoint(Config *C) {
    if (C->is_accept) {
        success = CASE2;
        return true;
    }
    return false;
}

bool UNFPDN::is_conc(CPN *cpn, index_t e1, index_t e2) {
    //若是因果关系，则不是并发
    if (is_causal(e1, e2))
        return false;

    for (auto i = 0; i < event[e1].producer.size(); i++) {
        auto row = *event[e1].producer.begin();
        for (auto j = 0; j < event[e2].producer.size(); j++) {
            auto col = *event[e2].producer.begin();
            if (matrix_Cob[row][col] == 0)
                return false;
        }
    }

    return true;
}

bool UNFPDN::is_causal(index_t e1, index_t e2) {
    if (e1 > e2 ? table_config[e1]->e_Vec[e2] : table_config[e2]->e_Vec[e1])
        return true;
    else return false;
}

//bool UNFPDN::add_Event_L(CPN *cpn, SYNCH *synch,Config *C,index_t t_idx, X x, Binding_unf *binding) {
//    //判断L变迁能否发生，并获取x,y集合
//    Y y;
//    if (!is_fireable_L(cpn, synch, x, y, binding))
//        return false;
//
//    if (had_extend(cpn, C, t_idx, x))
//        return false;
//
//    auto tmp_arccount = arccount;
//    BindingVariable(binding, cpn);
//
//    //新建事件e
//    UEvent e;
//    e.id = "E" + to_string(eventcount);
//    e.cpn_index = t_idx;
//
//    //建立变迁前弧，输入库所后弧
//    for (auto k = x.begin(); k != x.end(); k++) {
//        index_t b_idx = *k;
//        //前弧
//        e.producer.emplace(b_idx);
//        //后弧
//        condition[b_idx].consumer.emplace(eventcount);
//        //建弧
//        add_Arc(condition[b_idx].id, e.id);
//    }
//    //建立变迁后弧，建立输出库所（条件），建立输出库所前弧
//
//    //若是L变迁，则输出库所由y集合提供，且与前集对应条件保持相同token
//    for (auto i = y.begin(); i != y.end(); i++) {
//        auto id = "B" + to_string(conditioncount);
//        auto p_idx = condition[*i].cpn_index;
//        auto token = condition[*i].multiSet.getonlytoken();
//        add_Condition(id, p_idx, token);
//        //产生后弧
//        e.consumer.emplace(conditioncount - 1);
//
//        //建立前弧
//        condition.back().producer.emplace(eventcount);
//
//        //建弧
//        add_Arc(e.id, condition.back().id);
//
//        //concurrency matrix
//        cal_Matrix_Cob(x);
//    }
//    event.push_back(e);
//
//    //configuration matrices+
//    set<index_t> xPro;
//    cal_Config(x, xPro);
//    //configuration matrices-
//
//    //·[e]、[e]·、cut([e])+
//    cal_Heuristic(cpn, e, xPro);
//    //·[e]、[e]·、cut([e])-
//
//    index_t cutoff_e_idx;
//    if (is_cut_off(cutoff_e_idx)) {
//        is_counterexample(synch, cutoff_e_idx);
//    } else {
//        C->en.emplace(C->en.begin(),eventcount);
//        map_cut_all.emplace(make_pair(table_config.back()->cut->hash(condition), table_config.back()->cut));
//    }
//    eventcount++;
//    return true;
//}
set<index_t> UNFPDN::delete_VarInX(PE *pe, CPN *cpn, set<index_t> readP) {
    set<index_t> delete_B;
    for (auto i = pe->x.begin(); i != pe->x.end();) {
        auto p_idx = condition[*i].cpn_index;
        if (readP.find(p_idx) != readP.end()) {
            delete_B.emplace(*i);
            i = pe->x.erase(i);
        } else
            i++;
    }
    return delete_B;
}

bool UNFPDN::add_Event(CPN *cpn, SYNCH *synch, Nod *nod, PE *pe) {
    //特殊处理，若是读变迁，将x中的变量条件删除。
    set<index_t> readP;
    bool is_readT = cpn->is_readT(pe->t_idx, readP);
    set<index_t> read_varB;
    if (is_readT)
        read_varB = delete_VarInX(pe, cpn, readP);

    //判断事件是否已经存在
    if (is_readT) {
        if (had_extend(cpn, nod->C, pe, true, read_varB))
            return false;
    } else {
        if (had_extend(cpn, nod->C, pe))
            return false;
    }

    auto tmp_arccount = arccount;
    BindingVariable(pe->binding, cpn);

    //新建事件e
    UEvent e;
    e.id = "E" + to_string(eventcount);
    e.cpn_index = pe->t_idx;
    if (is_readT) {
//        e.is_read = true;
        e.read_varB = read_varB;
    }

    //建立变迁前弧，输入库所后弧
    for (auto k = pe->x.begin(); k != pe->x.end(); k++) {
        index_t b_idx = *k;
        //前弧
        e.producer.emplace(b_idx);
        //后弧
        condition[b_idx].consumer.emplace(eventcount);
        //建弧
        add_Arc(condition[b_idx].id, e.id);
    }

    //建立变迁后弧，建立输出库所（条件），建立输出库所前弧
    auto t = cpn->findT_byindex(pe->t_idx);
    auto T_producer = t->get_producer();
    auto T_consumer = t->get_consumer();
    auto tmp_conditioncount = conditioncount;

    for (unsigned int j = 0; j < T_consumer.size(); j++) {
        //特殊处理，若是readT，则不将变量条件加入输出条件；
        if (is_readT) {
            auto p_idx = T_consumer[j].idx;
            if (readP.find(p_idx) != readP.end())
                continue;
        }

        MultiSet ms;
        index_t idx = T_consumer[j].idx;
        auto pp = cpn->findP_byindex(idx);

        //BA库所的token特殊处理，保持为ba_thr
        if(idx>=synch->q_begin_idx){
            UCondition b;
            auto id = "B" + to_string(conditioncount);
            auto p_idx = T_consumer[j].idx;
            //加token
            auto colorset = "String";
            condition_tree ct;
            MultiSet ms;
            auto msi = sorttable.find_typename(colorset);
            ct.construct("1`'ba_thr'");//初始token
            cpn->CT2MS(ct, ms, msi.tid, msi.sid);
            auto token=ms.getonlytoken();

            add_Condition(id, p_idx, token);

            //产生后弧
            e.consumer.emplace(conditioncount - 1);

            //建立前弧
            condition.back().producer.emplace(eventcount);

            //建弧
            add_Arc(e.id, condition.back().id);
            //concurrency matrix
            cal_Matrix_Cob(pe->x);
            continue;
        }


        cpn->CT2MS(T_consumer[j].arc_exp, ms, pp->gettid(), pp->getsid());
        //CT2MS may have 0 count token
        //ms.merge();
        //产生输出库所（条件）

        //仅有allocwrite写弧的数据库所需添加一条读弧，防止CUT出现两个值（特殊处理+）
        if (T_consumer[j].arcType == allocwrite) {
            auto iter_pro = T_producer.begin();
            for (; iter_pro != T_producer.end(); iter_pro++) {
                if (iter_pro->idx == idx)
                    break;
            }
            if (iter_pro == T_producer.end()) {
                auto tokens = nod->C->cut->cuttable[idx].maptoken;
                auto token = tokens.begin();
                while (token != tokens.end()) {
                    index_t b_idx = token->first;
                    //前弧
                    e.producer.emplace(b_idx);
                    //后弧
                    condition[b_idx].consumer.emplace(eventcount);
                    //建弧
                    add_Arc(condition[b_idx].id, e.id);
                    token++;
                }
            }
        }
        //特殊处理-
        auto mapTokens = ms.getmapTokens();
        if (!mapTokens.empty()) {
            for (auto k = mapTokens.begin(); k != mapTokens.end(); k++) {
                int count = k->second;
                while (count) {
                    UCondition b;
                    auto id = "B" + to_string(conditioncount);
                    auto p_idx = T_consumer[j].idx;
                    auto token = k->first;
                    add_Condition(id, p_idx, token);

                    //产生后弧
                    e.consumer.emplace(conditioncount - 1);

                    //建立前弧
                    condition.back().producer.emplace(eventcount);

                    //建弧
                    add_Arc(e.id, condition.back().id);
                    //concurrency matrix
                    cal_Matrix_Cob(pe->x);

                    count--;
                    if (count < 1)
                        break;
                }
            }
        }
    }

    //configuration matrices+
    set<index_t> xPro;
    cal_Config(pe->x, xPro);
    //configuration matrices-

    //·[e]、[e]·、cut([e])+
    cal_context(cpn, e, xPro, nod->cexA);
    //·[e]、[e]·、cut([e])-

    //call+
//    e.call = pe->call;
    e.call_stack = pe->call_stack;
    //call-

    event.push_back(e);//要在计算is_cut_off之前；

    index_t cutoff_e_idx;
    if (is_cut_off(cutoff_e_idx)) {
        is_counterexample(synch, nod->C, cutoff_e_idx);
    } else {
        map_cut_all.emplace(table_config.back()->cut->hash(condition), table_config.back()->cut);
    }
    pe->e_idx = eventcount;
    pe->had_extend = true;
    eventcount++;
    return true;
}

void UNFPDN::add_Arc(string source_id, string target_id) {
    UArc arc;
    arc.source_id = source_id;
    arc.target_id = target_id;
    this->arc.push_back(arc);
    arccount++;
}

bool UNFPDN::is_fireable_L(CPN *cpn, SYNCH *synch, X &x, Y &y, Binding_unf *binding) {
    //判断L变迁能否发生，并获取x,y集合
    //增量计算[e],不添加进configuration matrices
    set<int> prep_set;
    for (auto i = x.begin(); i != x.end(); i++) {
        if (!condition[*i].producer.empty())
            prep_set.insert(*condition[*i].producer.begin());
    }
    vector<int> config;
    if (!prep_set.empty()) {
        for (auto col = 0; col < table_config.size(); col++) {
            int result = 0;
            for (auto row = prep_set.begin(); row != prep_set.end(); row++) {
                result = result || table_config[*row]->e_Vec[col];
            }
            config.push_back(result);
        }
    } else {
        for (auto col = 0; col < table_config.size(); col++) {
            config.push_back(0);
        }
    }

    //计算M:=Mark([e]\{e}); /*The marking M= (q, sf, O, H)*/
    CUT *M = new CUT(cpn);
    if (prep_set.size() == 1) {
        *M = *table_config[*prep_set.begin()]->cut;
    } else {
        M->plus_cuttable(min_o->cuttable);
        for (auto i = prep_set.begin(); i != prep_set.end(); i++) {
            M->plus_cuttable(table_config[*i]->conf_con->cuttable);
        }
        for (auto i = prep_set.begin(); i != prep_set.end(); i++) {
            M->minus_cuttable(table_config[*i]->conf_pre->cuttable);
        }
    }
    //q:=M∩Q; /*Extract the Büchi state q*/
    //自动机状态对应的库所在place数组末尾;
    int q;
    for (auto i = synch->q_begin_idx; i < cpn->get_placecount(); i++) {
        if (!M->cuttable[i].maptoken.empty()) {
            q = M->cuttable[i].getonlytoken()->first;
            //if Aq¬φ= (Γ, Q, q, ρ, F) does not accept Oω
            //如果q不是可接受状态，则自动机不接受序列Oω，L不发生
            if (synch->state_accepted.find(i) == synch->state_accepted.end()) {
                return false;
            }
        }
    }

    //X:=Cut([e]\{e});/*Extend the preset to also remove tokens from H*/
    //Y:=(M∩InvisPre); /*Project M on invisible transition presets*/
    x.clear();
    auto init_p_count = synch->q_begin_idx;
    for (auto i = 0; i < cpn->get_placecount(); i++) {
        auto tokens = M->cuttable[i].maptoken;
        auto token = tokens.begin();
        bool is_visible_T = false;
        if (tokens.size() != 0) {
            if (i < init_p_count) {//筛选可视库所+不可视库所，排除B，S，Q
                if (synch->visible_P.find(i) != synch->visible_P.end()) {//筛选可视库所
                    is_visible_T = true;
                }
            }
        }
        while (token != tokens.end()) {
            //X
            x.emplace(token->first);
            //Y
            if (is_visible_T) {
                y.emplace(token->first);
            }
            token++;
        }
    }
    delete M;
    return true;
}

//bool UNFPDN::had_extend_T(Config *C, set<index_t> x) {
//    if (C->e_Vec.empty())
//        return false;
//    auto e = event[C->e_last];
//
//    auto i = e.consumer.begin();
//    auto j = x.begin();
//    while (i != e.consumer.end() && j != x.end()) {
//        if (*i < *j)
//            i++;
//        else if (*i > *j)
//            j++;
//        else return false;
//    }
//    return true;
//}

bool UNFPDN::had_extend(CPN *cpn, Config *C, PE *pe, bool is_readT, set<index_t> read_varB) {
    index_t b_control;
    auto t_idx = pe->t_idx;
    auto x = pe->x;
    bool has_control = 0;
    for (auto i = x.begin(); i != x.end(); i++) {
        if (cpn->findP_byindex(condition[*i].cpn_index)->getiscontrolP()) {
            b_control = *i;
            has_control = 1;
            break;
        }
    }
    if (!has_control) throw "ERROR,NO CONTROL PLACE";
    auto b = condition[b_control];
    auto b_con = b.consumer;
    if (b_con.empty())
        return false;
    for (auto i = b_con.begin(); i != b_con.end(); i++) {
        if (event[*i].cpn_index == t_idx && event[*i].call_stack == pe->call_stack) {
            auto e_pro = event[*i].producer;
            set<index_t> x_e;
            for (auto j = e_pro.begin(); j != e_pro.end(); j++) {
                x_e.emplace(*j);
            }
            if (x == x_e) {
                if (is_readT) {
                    if (event[*i].read_varB == read_varB) {
                        pe->e_idx = *i;
                        pe->had_extend = true;
                        return true;
                    }
                } else {
                    pe->e_idx = *i;
                    pe->had_extend = true;
                    return true;
                }
            }
        }
    }
    return false;
}

void UNFPDN::print_UNF(string filename, CPN *cpn) {
    ofstream out;
    out.open(filename + ".txt", ios::out);
    //out << "Place:" << endl;
    //out << "-----------------------------------" << endl;

    int P_width, T_width, T_height, font_size = 30;
    P_width = T_width = T_height = 1;

    string fillcolor = "chartreuse";
    string fillcolor1 = "lightblue";


    for (int i = 0; i < conditioncount; i++) {
        if (cpn->findP_byindex(condition[i].cpn_index)->getiscontrolP() == false)
            out << "subgraph cluster_" << condition[i].id << "{" << "fontsize = " << to_string(font_size) << ";label=\""
                <<
                cpn->findP_byindex(condition[i].cpn_index)->getExp() << ";"
                << cpn->findP_byindex(condition[i].cpn_index)->getid() << "\";color=\"white\"" << condition[i].id <<
                "[shape=circle" << ",fontsize = " << to_string(font_size) << ",width=" << to_string(P_width)
                << ",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor << "\"]}" << endl;
        else {
            //            out << condition[i].id << "[shape=circle," << "label=\"" << condition[i].expression << "\"]" << endl;
            out << "subgraph cluster_" << condition[i].id << "{" << "fontsize = " << to_string(font_size) << ";label=\""
                <<
                cpn->findP_byindex(condition[i].cpn_index)->getExp() << ";"
                << cpn->findP_byindex(condition[i].cpn_index)->getid() << "\";color=\"white\"" << condition[i].id <<
                "[shape=circle" << ",fontsize = " << to_string(font_size) << ",width=" << to_string(P_width)
                << ",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor1 << "\"]}" << endl;
        }
    }
    //out << "-----------------------------------" << endl;
    //out << "Transition:" << endl;
    //out << "-----------------------------------" << endl;
    for (int i = 0; i < eventcount; i++) {
        out << "subgraph cluster_" << event[i].id << "{" << "fontsize = " << to_string(font_size)
            << ";label=\"" <<
            cpn->findT_byindex(event[i].cpn_index)->getid() << "\";color=\"white\"" << event[i].id <<
            "[shape=box" << ",fontsize = " << to_string(font_size) << ",width=" << to_string(T_width) << ",height="
            << to_string(T_height) << "]}" << endl;

        //        out << event[i].id << "[shape=box"<<",fontsize = "<<to_string(font_size)<<",width="<<to_string(T_width)<<",height="<<to_string(T_height)
        //        <<";label=\"" <<cpn->findT_byindex(event[i].cpn_index)->getid() <<";"
        //        <<"]" << endl;
    }
    //out << "-----------------------------------" << endl;
    //out << "Arc:" << endl;
    //out << "-----------------------------------" << endl;

    for (int i = 0; i < arccount; i++) {
        //        if(arc[i].getdeleted())
        //            continue;
        //        string tmp_exp = arc[i].getexp();
        //        int pos = 0;
        //        pos = tmp_exp.find('\"');
        //        while(pos != string::npos){
        //            tmp_exp = tmp_exp.substr(0,pos) + "\\" + tmp_exp.substr(pos);
        //            pos = tmp_exp.find('\"',pos + 2);
        //        }
        //        if (arc[i].getArctype() == executed)
        //            out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"red\",label=\""<< tmp_exp<<"\"]" << endl;
        //        else if (arc[i].getArctype() == write)
        //            ;//out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        //            else if (arc[i].getArctype() == data)
        //                ;//out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        //                else if (arc[i].getArctype() == remain)
        //                    ;//out << arc[i].getsrc() << "->" << arc[i].gettgt() << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        //                    else
        out << arc[i].source_id << "->" << arc[i].target_id << "[label=\"" << "\"]" << endl;
    }
    out.close();

    readGraph(filename + ".txt", filename + ".dot");
    makeGraph(filename + ".dot", filename + ".png");
}

void UNFOLDING::getsynch(SYNCH *synch) {
    this->synch = synch;
    cpn = synch->cpn_synch;
}

void UNFOLDING::init() {
//    cpn->unf_init();
    elapse_begin=clock();
    unfpdn = new UNFPDN;
    unfpdn->init(cpn);

    //00-CPN的初始标识，库所p中有颜色为c的token，将含初始标识的库所转化为条件b，并用(p,c)标记；
    MultiSet multiset;
    Token_map tokens;
    auto place = cpn->getplacearr();
    for (int i = 0; i < cpn->get_placecount(); i++) {
        multiset = place[i].getMultiSet();
        tokens = multiset.getmapTokens();
        auto token = tokens.begin();
        while (token != tokens.end()) {
            for (auto j = 0; j < token->second; j++) {
                auto id = "B" + to_string(unfpdn->conditioncount);
                unfpdn->min_o->add_condition(unfpdn->conditioncount, i, token->first);
                unfpdn->add_Condition(id, i, token->first);
            }
            token++;
        }
    }
    //concurrency matrix
    for (auto i = 0; i < unfpdn->conditioncount; i++) {
        vector<int> tmp_cob;
        for (auto j = 0; j < unfpdn->conditioncount; j++) {
            if (i == j)
                tmp_cob.push_back(0);
            else
                tmp_cob.push_back(1);
        }
        unfpdn->matrix_Cob.push_back(tmp_cob);
    }
}

bool UNFOLDING::handleLTLF(string s, Config *C) {
    if (s[0] == '!') //前面带有'!'的is-fireable{}
    {
        /*!{t1 || t2 || t3}：
         * true：t1不可发生 并且 t2不可发生 并且 t3不可发生
         * false： 只要有一个能发生
         * */
        s = s.substr(2, s.length() - 2); //去掉“!{”
        while (1) {
            int pos = s.find_first_of(",");
            if (pos < 0)
                break;
            string subs = s.substr(0, pos);            //取出一个变迁

            auto idx = cpn->mapTransition.find(subs)->second;
            auto t_CP = synch->map_T2CP.find(idx)->second;
            vector<Binding_unf *> bindings;
            if (is_firable(t_CP, C, bindings))
                return false;

            s = s.substr(pos + 1, s.length() - pos);
        }
        return true;

    } else {         //单纯的is-fireable{}原子命题
        /*{t1 || t2 || t3}:
	     * true: 只要有一个能发生
	     * false: 都不能发生
	     * */
        s = s.substr(1, s.length() - 1);//去掉‘{’

        while (1) {
            int pos = s.find_first_of(",");
            if (pos < 0)
                break;
            string subs = s.substr(0, pos);  //取出一个变迁

            auto idx = cpn->mapTransition.find(subs)->second;
            auto t_CP = synch->map_T2CP.find(idx)->second;
            vector<Binding_unf *> bindings;
            if (is_firable(t_CP, C, bindings))
                return true;

            //走到这里表示取出来的变迁不可发生
            s = s.substr(pos + 1, s.length() - pos);
        }
        //走到这里表示所有的变迁都不能发生
        return false;
    }
}

bool UNFOLDING::handleLTLC(string s, Config *C) {
    short int front_sum, latter_sum;     //前半部分和 后半部分和
    if (s[0] == '!') {
        /*!(front <= latter)
	     * true:front > latter
	     * false:front <= latter
	     * */
        s = s.substr(2, s.length() - 2);   //去除"!{"
        handleLTLCstep(front_sum, latter_sum, s, C);
        if (front_sum <= latter_sum)
            return false;
        else
            return true;
    } else {
        /*(front <= latter)
         * true:front <= latter
         * false:front > latter
         * */
        s = s.substr(1, s.length() - 1);   //去除"{"
        handleLTLCstep(front_sum, latter_sum, s, C);
        if (front_sum <= latter_sum)
            return true;
        else
            return false;
    }
}

void UNFOLDING::handleLTLCstep(short int &front_sum, short int &latter_sum, string s, Config *C) {
    if (s[0] == 't')   //前半部分是token-count的形式
    {
        int pos = s.find_first_of("<=");           //定位到<=前
        string s_token = s.substr(12, pos - 13);   //去除"token-count(" ")"  ֻ只剩p1,p2,
        front_sum = sumtoken(s_token, C);      //计算token和

        //计算后半部分的和
        s = s.substr(pos + 2, s.length() - pos - 2); //取得后半部分，若是常数 则是常数+}

        if (s[0] == 't')   //后半部分是token-count
        {
            string s_token = s.substr(12, s.length() - 14);
            latter_sum = sumtoken(s_token, C);
        } else {             //后半部分是常数
            s = s.substr(0, s.length() - 1);        //去除后面的}
            latter_sum = atoi(s.c_str());
        }
    } else {             //前半部分是常数，那后半部分肯定是token-count
        //处理前半部分  现有的串 是从常数开始的
        int pos = s.find_first_of("<=");//定位到<=,取其前半部分
        string num = s.substr(0, pos);//取出常数
        front_sum = atoi(num.c_str());

        //处理后半部分
        s = s.substr(pos + 14, s.length() - pos - 16);

        latter_sum = sumtoken(s, C);
    }
}

bool UNFOLDING::handleLTLV(string s, Config *C) {
    int pos = s.find("==");
    if (pos != string::npos) {
        //equality
        if (s[0] == '!') {
            /*!(front == latter)
             * true:front != latter
             * false:front == latter
             * */
            s = s.substr(2, s.length() - 3);   //去除"!{}"
            int p = s.find_first_of("==");
            string front = s.substr(0, p);
            string latter = s.substr(p + 2);
            bucket front_color, latter_color;
            if (!FetchColor(front, C, front_color))
                return false;
            if (!FetchColor(latter, C, latter_color))
                return false;
            if (front_color == latter_color)
                return false;
            else
                return true;
        } else {
            /*(front == latter)
             * true:front == latter
             * false:front != latter
             * */
            s = s.substr(1, s.length() - 2);   //去除"{}"
            int p = s.find_first_of("==");
            string front = s.substr(0, p);
            string latter = s.substr(p + 2);
            bucket front_color, latter_color;
            if (!FetchColor(front, C, front_color))
                return false;
            if (!FetchColor(latter, C, latter_color))
                return false;
            if (front_color == latter_color)
                return true;
            else
                return false;
        }
    }

    pos = s.find("<=");
    if (pos != string::npos) {
        if (s[0] == '!') {
            /*!(front <= latter)
             * true:front !<= latter
             * false:front <= latter
             * */
            s = s.substr(2, s.length() - 3);   //去除"!{}"
            int p = s.find_first_of("<=");
            string front = s.substr(0, p);
            string latter = s.substr(p + 2);
            bucket front_color, latter_color;
            if (!FetchColor(front, C, front_color))
                return false;
            if (!FetchColor(latter, C, latter_color))
                return false;
            if (front_color <= latter_color)
                return false;
            else
                return true;
        } else {
            /*(front <= latter)
             * true:front <= latter
             * false:front !<= latter
             * */
            s = s.substr(1, s.length() - 2);   //去除"{}"
            int p = s.find_first_of("<=");
            string front = s.substr(0, p);
            string latter = s.substr(p + 2);
            bucket front_color, latter_color;
            if (!FetchColor(front, C, front_color))
                return false;
            if (!FetchColor(latter, C, latter_color))
                return false;
            if (front_color <= latter_color)
                return true;
            else
                return false;
        }
    }

    if (s[0] == '!') {
        /*!(front < latter)
         * true:front !< latter
         * false:front < latter
         * */
        s = s.substr(2, s.length() - 3);   //去除"!{}"
        int p = s.find_first_of("<");
        string front = s.substr(0, p);
        string latter = s.substr(p + 1);
        bucket front_color, latter_color;
        if (!FetchColor(front, C, front_color))
            return false;
        if (!FetchColor(latter, C, latter_color))
            return false;
        if (front_color < latter_color)
            return false;
        else
            return true;
    } else {
        /*(front < latter)
         * true:front < latter
         * false:front !< latter
         * */
        s = s.substr(1, s.length() - 2);   //去除"{}"
        int p = s.find_first_of("<");
        string front = s.substr(0, p);
        string latter = s.substr(p + 1);
        bucket front_color, latter_color;
        if (!FetchColor(front, C, front_color))
            return false;
        if (!FetchColor(latter, C, latter_color))
            return false;
        if (front_color < latter_color)
            return true;
        else
            return false;
    }
}

bool UNFOLDING::FetchColor(string s, Config *C, bucket &color) {
    if (s[0] == 't') {
        auto cpn = synch->cpn_synch;
        string value = s.substr(12, s.length() - 13);
        auto valueVec = split(value, "#");
        if (valueVec.size() != 3)
            throw "token-value转换为字符串后的内容应该由三部分组成!";
        string P_name = valueVec[0], index = valueVec[1], thread = valueVec[2];
        auto iter = cpn->mapPlace.find(P_name);
        if (iter == cpn->mapPlace.end())
            throw "找不到库所" + P_name + " in product.cpp!";
        CPlace &pp = cpn->getplacearr()[iter->second];
//        auto tks = state->marking.mss[iter->second].getmapTokens();
        auto tks = C->cut->cuttable[iter->second].maptoken;
        auto tkiter = tks.begin();
        while (tkiter != tks.end()) {
//            auto tmp_tk = tkiter->first;
            auto tmp_tk = tkiter->second;
            SORTID sid = tmp_tk->getsid();
            auto ps = sorttable.find_productsort(sid);
            if (index == "NULL" ^ ps.get_hasindex() && thread == "NULL" ^ ps.get_hastid()) {
                if (index == "NULL" && thread == "NULL") {
                    //全局普通变量
                    if (tks.size() != 1)
                        throw "FetchColor时全局普通变量出现异常：token数大于1";
                    Bucket tmp_bkt, value_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.getVarValue(value_bkt);
                    color.generateFromBucket(value_bkt);
                    return true;
                } else if (index != "NULL" && thread == "NULL") {
                    //全局数组变量
                    Bucket tmp_bkt, value_bkt, index_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size() - 2]->getcolor(index_bkt);
                    if (index_bkt == atoi(index.c_str())) {
                        tmp_bkt.getVarValue(value_bkt);
                        color.generateFromBucket(value_bkt);
                        return true;
                    }
                } else if (index == "NULL" && thread != "NULL") {
                    //局部普通变量
                    Bucket tmp_bkt, value_bkt, tid_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size() - 1]->getcolor(tid_bkt);
                    if (tid_bkt == thread) {
                        tmp_bkt.getVarValue(value_bkt);
                        color.generateFromBucket(value_bkt);
                        return true;
                    }
                } else if (index != "NULL" && thread != "NULL") {
                    //局部数组变量
                    Bucket tmp_bkt, value_bkt, index_bkt, tid_bkt;
                    tmp_tk->getcolor(tmp_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size() - 3]->getcolor(index_bkt);
                    tmp_bkt.pro[tmp_bkt.pro.size() - 1]->getcolor(tid_bkt);
                    if (index_bkt == atoi(index.c_str()) && tid_bkt == thread) {
                        tmp_bkt.getVarValue(value_bkt);
                        color.generateFromBucket(value_bkt);
                        return true;
                    }
                }
            } else
                throw "库所" + P_name + "的index或tid和检测性质不匹配!";
            tkiter++;
        }
        return false;
    } else if (s[0] == 'i') {
        string num = s.substr(13, s.length() - 14);
        color.icolor = atoi(num.c_str());
    } else if (s[0] == 'r') {
        string num = s.substr(14, s.length() - 15);
        color.rcolor = atof(num.c_str());
    } else if (s[0] == 's') {
        color.scolor = s.substr(16, s.length() - 17);
    }
    return true;
}

short int UNFOLDING::sumtoken(string s, Config *C) {
    short int sum = 0;
    while (1) {
        int pos = s.find_first_of(",");
        if (pos == string::npos)
            break;
        string subs = s.substr(0, pos);        //取得一个p1

        map<string, index_t>::iterator piter;
        piter = cpn->mapPlace.find(subs);

        auto tokens = C->cut->cuttable[piter->second].maptoken;
        sum += tokens.size();

        //将前面的用过的P1去除 从p2开始作为新的s串
        s = s.substr(pos + 1, s.length() - pos);
    }
    return sum;
}

bool UNFOLDING::is_firable(index_t idx, Config *C, vector<Binding_unf *> &bindings) {
    CTransition *t = cpn->findT_byindex(idx);

    //get_binding
    bindings = get_bindings(cpn, t, C->cut, unfpdn, synch);
    if (bindings.empty())
        return false;

    //judge guard
    if (t->get_hasguard()) {
        for (auto iter = bindings.begin(); iter != bindings.end();) {
            Integer_t res;
            MultiSet ms;

            //            ms.tid = Integer;
            //            ms.sid = 0;
            BindingVariable(*iter, cpn);
            //            ms.Exp2MS(cpn,transition->guard.root,0,0,false);
            //            ms.tokenQ->next->color->getColor(res);
            cpn->CT2MS(t->get_guard(), ms, Integer, 0);
            Bucket bkt;
            ms.getonlytoken()->getcolor(bkt);
            res = bkt.integer;
            if (res == 0) {
                iter = bindings.erase(iter);
            } else {
                iter++;
            }
        }
    }
    //如果是自动机变迁并有原子命题，则单独处理
    auto iter = synch->map_t2guard.find(idx);
    if (iter != synch->map_t2guard.end()) {
        for (auto i = iter->second.begin(); i != iter->second.end(); i++) {
            switch (synch->judgeF(*i)) {
                case LTLF: {
                    if (!handleLTLF(*i, C))
                        return false;
                    break;
                }
                case LTLV: {
                    if (!handleLTLV(*i, C))
                        return false;
                    break;
                }
            }
        }
    }

    if (bindings.empty())
        return false;
    return true;
}

Config *UNFOLDING::get_Config_add(Config *C, index_t e_idx, Cex_A *cexA) {
    Config *config = new Config;
    auto e = unfpdn->event[e_idx];
    *config = *C;
    config->e_last = e_idx;
    if (config->e_Vec.size() <= e_idx) {
        for (auto i = config->e_Vec.size(); i < e_idx; i++)
            config->e_Vec.push_back(false);
        config->e_Vec.push_back(true);
    } else {
        config->e_Vec[e_idx] = true;
    }
    config->fired_T.emplace(e.cpn_index);
    auto t_idx = unfpdn->event[e_idx].cpn_index;
    if (t_idx >= synch->ba_t_begin_idx && t_idx < synch->ba_t_begin_idx + synch->ba_t_count) {
        if (synch->I_idx.find(t_idx) != synch->I_idx.end())
            config->is_accept = true;
        else config->is_accept = false;
    }
    //context
    //配置集只需要cut，不用conf_pre和conf_con
    config->cut = new CUT(cpn);
    *config->cut = *C->cut;
    for (auto i = e.consumer.begin(); i != e.consumer.end(); i++) {
        auto p_idx = unfpdn->condition[*i].cpn_index;
        auto token = unfpdn->condition[*i].multiSet.getonlytoken();
        config->cut->add_condition(*i, p_idx, token);
    }
    for (auto i = e.producer.begin(); i != e.producer.end(); i++) {
        auto p_idx = unfpdn->condition[*i].cpn_index;
        auto token = unfpdn->condition[*i].multiSet.getonlytoken();
        config->cut->sub_condition(*i, p_idx, token);
    }

    unfpdn->cal_is_cutoff_C(config, cexA);

    return config;
}

bool UNFOLDING::imm_conflict_var(UEvent e1, index_t t_idx) {
    auto t1 = cpn->findT_byindex(e1.cpn_index);
    auto t2 = cpn->findT_byindex(t_idx);
    set<index_t> p_var1, p_var2;
    auto t_pre1 = t1->get_producer();
    auto t_pre2 = t2->get_producer();
    for (auto i = t_pre1.begin(); i != t_pre1.end(); i++) {
        if (cpn->findP_byindex(i->idx)->getiscontrolP() || cpn->findP_byindex(i->idx)->getisexecuted())
            continue;
        p_var1.emplace(i->idx);
    }
    for (auto i = t_pre2.begin(); i != t_pre2.end(); i++) {
        if (cpn->findP_byindex(i->idx)->getiscontrolP() || cpn->findP_byindex(i->idx)->getisexecuted())
            continue;
        p_var2.emplace(i->idx);
    }

    auto i = p_var1.begin();
    auto j = p_var2.begin();
    while (i != p_var1.end() && j != p_var2.end()) {
        if (*i < *j)
            i++;
        else if (*i > *j)
            j++;
        else return true;
    }
    return false;
}

bool UNFOLDING::imm_conflict(UEvent e1, PE *pe) {
    auto i = e1.producer.begin();
    auto j = pe->x.begin();
    while (i != e1.producer.end() && j != pe->x.end()) {
        if (*i < *j)
            i++;
        else if (*i > *j)
            j++;
        else return true;
    }

    if (imm_conflict_var(e1, pe->t_idx))
        return true;
    else return false;
}

//bool UNFOLDING::Alt0(Config *C, type_e_set D, index_t &e_alt) {
//    //Function Alt(X,Y ) returns all alternatives (in U) to Y after X
//    /*
//     * J应当在C下使能，与D直接冲突
//     */
//    for (auto i = C->en.begin(); i != C->en.end(); i++) {
//        if ((*i)->had_extend) {
//            if (exist_in(D, (*i)->e_idx))
//                continue;
//        }
//        auto j = D.begin();
//        for (; j != D.end(); j++) {
//            if (!imm_conflict(unfpdn->event[*j], *i))
//                break;
//        }
//        if (j == D.end()) {
//            if (!unfpdn->add_Event(cpn, synch, C, *i))
//                continue;
//            e_alt = (*i)->e_idx;
//            return true;
//        }
//    }
//    return false;
//}

set<type_T2Call> get_en_T2Call(vector<PE *> en) {
    set<type_T2Call> en_T;
    for (auto i = en.begin(); i != en.end(); i++) {
        en_T.emplace((*i)->t_idx, (*i)->call_stack);
    }
    return en_T;
}


bool is_earlier_cft(type_multimapCall2T cftTs, type_T2Call t2call_cur, type_Call2T call2t_conflict) {
    //若t_cur是当前冲突变迁，返回false
    if (call2t_conflict.second.find(t2call_cur.first) != call2t_conflict.second.end()) {
        if (call2t_conflict.first == t2call_cur.second)
            return false;
    }
    //若t_cur是更早冲突变迁，返回true
    for (auto i = cftTs.begin(); i != cftTs.end(); i++) {
        if (i->first == t2call_cur.second) {
            if (i->second.find(t2call_cur.first) != i->second.end())
                return true;
        }
    }
    return false;
}

bool Cex_texture::operator==(const Cex_texture &cexTexture) {
    if (t2Call == cexTexture.t2Call && Call == cexTexture.Call && conflictT == cexTexture.conflictT) {
        return true;
    }
    return false;
}

bool
Cft_Tpath::searched_Add(type_Call2T call2T, vector<type_T2Call> &searched, index_t t_idx, type_callStack call_cur) {
    //若变迁是冲突原变迁，则不加入searched；
    if (t_idx == t2cftT.first.first && call_cur == t2cftT.first.second) {
        return false;
    }
    //若冲突路径包含另一个与当前变迁冲突的变迁，则无需分岔，优先计算更早的冲突变迁；不加入searched;
    if (is_earlier_cft(t2cftT.second, make_pair(t_idx, call_cur), call2T)) {
        return false;
    }

    if (!exist_in(searched, make_pair(t_idx, call_cur))) {
        searched.push_back(make_pair(t_idx, call_cur));
        return true;
    }
    return true;
}

type_cftPath Cft_Tpath::get_t_path_step(type_Call2T call2T) {
    vector<type_T2Call> searched;
    index_t endT_idx = *call2T.second.begin();
    searched.push_back(make_pair(endT_idx, call2T.first));

    for (unsigned int i = 0; i < searched.size(); i++) {
        auto idx = searched[i].first;
        CTransition *tran = cpn->findT_byindex(idx);
        if (en_t2Call.find(searched[i]) != en_t2Call.end())//若为en_T，则不再继续向前查找
            continue;
        //当找执行弧到达当前线程开头或函数入口，需要找一次控制弧跳到线程创建处或函数调用处。
        bool control_permit = true;//路径到达函数入口或线程创建处
//        bool is_beginningT = true;//路径如果找到初始库所，则该路径无法发生;
        for (unsigned int j = 0; j < tran->get_producer().size(); j++) {
            CPlace *place = &cpn->getplacearr()[tran->get_producer()[j].idx];
//            if (is_beginningT && (!place->get_producer().empty()))
//                is_beginningT = false;
            for (unsigned int k = 0; k < place->get_producer().size(); k++) {
                auto arcType = place->get_producer()[k].arcType;
                if (arcType == executed || arcType == call_exit) {
                    auto callStk = searched[i].second;
                    auto t = cpn->findT_byindex(place->get_producer()[k].idx);
//                    if (t->isabort) {
                    auto exitT = cpn->map_call_exitT2enterT.find(tran->getid());
                    if (exitT != cpn->map_call_exitT2enterT.end()) {
                        auto enterT = cpn->map_call_exitT2enterT.find(tran->getid())->second;
                        vector<string> enterTs;
                        enterTs.push_back(enterT);
                        callStk.push_back(enterTs);
                    }
                    if (searched_Add(call2T, searched, place->get_producer()[k].idx, callStk)) {
                        control_permit = false;
                    }
                }
//                if (arcType == call_exit) {//需要添加call_stack
//                    auto enterT = cpn->map_call_exitT2enterT.find(tran->getid())->second;
//                    auto callStk = searched[i].second;
//                    vector<string> enterTs;
//                    enterTs.push_back(enterT);
//                    callStk.push_back(enterTs);
//                    if (searched_Add(call2T, searched, place->get_producer()[k].idx, callStk)) {
//                        control_permit = false;
//                    }
//                }
            }
        }
//        if (is_beginningT) {
//            return t_path;
//        }
        //若call指针超出栈范围，则已经找到最早的线程创建处，不需要再往前找控制弧
        if (control_permit && searched[i].second.size() > 0) {
            //找到函数入口需匹配冲突变迁对应的call变迁;
//            if (searched[i].second && call2T.first != notCall) {//若call不是notCall，则找到的不是线程创建处
//                first_call_enter= false;
            for (unsigned int j = 0; j < tran->get_producer().size(); j++) {
                CPlace *place = &cpn->getplacearr()[tran->get_producer()[j].idx];
                for (unsigned int k = 0; k < place->get_producer().size(); k++) {
                    auto arcType = place->get_producer()[k].arcType;
                    auto t_idx = place->get_producer()[k].idx;
                    auto t_id = cpn->findT_byindex(t_idx)->getid();
                    if ((arcType == control || arcType == call_enter) && exist_in(searched[i].second.back(), t_id)) {
                        auto callStk = searched[i].second;//用于匹配调用变迁的callStack
                        callStk.pop_back();
                        if (auto itCall =
                                cpn->map_t_call2thread.find(make_pair(t_idx, callStk)) != cpn->map_t_call2thread.end())
                            searched_Add(call2T, searched, t_idx, callStk);
//                        for (auto it = cpn->map_t_call2thread.begin(); it != cpn->map_t_call2thread.end(); it++) {
//                            if (it->first.first == place->get_producer()[k].idx) {
//                                searched_Add(call2T, searched, place->get_producer()[k].idx, it->first.second);
//                            }
//                        }
                    }
                }
            }
        }
    }
    //将search加入t_path
    bool enabled = false;
    for (auto i = searched.begin(); i != searched.end(); i++) {
        //若是en_T2Call,则路径可发生;
        if (en_t2Call.find(*i) != en_t2Call.end()) {
            enabled = true;
        }
        t_path.emplace(*i);
        //若路径上存在A中原变迁，则该路径不可发生
        for (auto j = cexA->begin(); j != cexA->end(); j++) {
            if (j->t2Call == *i) {
                t_path.clear();
                return t_path;
            }
        }
    }
    if (!enabled) {
        t_path.clear();
        return t_path;
    }
    //添加call2T中的变迁
    for (auto i = call2T.second.begin(); i != call2T.second.end(); i++) {
        t_path.emplace(*i, call2T.first);
    }
    return t_path;
}

//type_cftPath Cft_Tpath::get_t_path_step(type_Call2T call2T) {
//    vector<pair<CTransition *, bool>> searched;//pair<t,是否到达过第一个call_enter处>
//    index_t endT_idx = *call2T.second.begin();
//    auto endT = cpn->findT_byindex(endT_idx);
//    searched.push_back(endT, true);
//
////    bool first_call_enter= true;//第一次到达程序入口处需找对应call;
//
//    for (unsigned int i = 0; i < searched.size(); i++) {
//        CTransition *tran = searched[i].first;
//        auto idx = cpn->mapTransition.find(tran->getid())->second;
//        if (en_t.find(idx) != en_t.end())
//            continue;
//        //当找执行弧到达当前线程开头，需要找一次控制弧跳到线程创建处。
//        bool control_permit = true;//路径到达程序入口或线程创建处
//        bool is_beginningT = true;//路径如果找到初始库所，则该路径无法发生;
//        for (unsigned int j = 0; j < tran->get_producer().size(); j++) {
//            CPlace *place = &cpn->getplacearr()[tran->get_producer()[j].idx];
//            if (is_beginningT && (!place->get_producer().empty()))
//                is_beginningT = false;
//            for (unsigned int k = 0; k < place->get_producer().size(); k++) {
//                auto arcType = place->get_producer()[k].arcType;
//                if (arcType == executed || arcType == call_exit) {
//                    if(searched_Add(call2T,searched,place->get_producer()[k].idx)){
//                        control_permit= false;
//                    }
////                    if (!exist_in(searched, make_pair(&cpn->gettransarr()[place->get_producer()[k].idx], true))
////                        && !exist_in(searched, make_pair(&cpn->gettransarr()[place->get_producer()[k].idx], false))) {
////                        searched.push_back(&cpn->gettransarr()[place->get_producer()[k].idx], searched[i].second);
////                        control_permit = false;
//////                        if(first_call_enter){
//////                            t_fixCall.emplace(place->get_producer()[k].idx);
//////                        }
////                    }
//                }
//            }
//        }
//        if (is_beginningT) {
//            return t_path;
//        }
//        if (control_permit) {
//            //第一次找到程序入口需匹配冲突变迁对应的call变迁;
//            if (searched[i].second && call2T.first != notCall) {//若call不是notCall，则找到的不是线程创建处
////                first_call_enter= false;
//                for (unsigned int j = 0; j < tran->get_producer().size(); j++) {
//                    CPlace *place = &cpn->getplacearr()[tran->get_producer()[j].idx];
//                    for (unsigned int k = 0; k < place->get_producer().size(); k++) {
//                        auto arcType = place->get_producer()[k].arcType;
//                        auto t_id = cpn->findT_byindex(place->get_producer()[k].idx)->getid();
//                        if (arcType == call_enter && t_id == call2T.first) {
//                            searched_Add(call2T,searched,place->get_producer()[k].idx, false);
////                            if (!exist_in(searched, make_pair(&cpn->gettransarr()[place->get_producer()[k].idx], true))
////                                && !exist_in(searched,
////                                             make_pair(&cpn->gettransarr()[place->get_producer()[k].idx], false))) {
////                                searched.push_back(&cpn->gettransarr()[place->get_producer()[k].idx], false);
////                            }
//                        }
//                    }
//                }
//            } else {
//                for (unsigned int j = 0; j < tran->get_producer().size(); j++) {
//                    CPlace *place = &cpn->getplacearr()[tran->get_producer()[j].idx];
//                    for (unsigned int k = 0; k < place->get_producer().size(); k++) {
//                        auto arcType = place->get_producer()[k].arcType;
//                        if (arcType == control || arcType == call_enter) {
//                            searched_Add(call2T,searched,place->get_producer()[k].idx, searched[i].second);
////                            if (!exist_in(searched, make_pair(&cpn->gettransarr()[place->get_producer()[k].idx], true))
////                                && !exist_in(searched,
////                                             make_pair(&cpn->gettransarr()[place->get_producer()[k].idx], false))) {
////                                searched.push_back(&cpn->gettransarr()[place->get_producer()[k].idx],
////                                                      searched[i].second);
////                                if(first_call_enter){
////                                    t_fixCall.emplace(place->get_producer()[k].idx);
////                                }
////                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//    //将search加入t_path
//    for (auto i = searched.begin(); i != searched.end(); i++) {
//        auto t_idx = cpn->mapTransition.find(i->first->getid())->second;
//        if (i->second) {//冲突路径上的变迁有确定的call，即向前查找时,在遇到第一个call_enter变迁之前的变迁；
////            //若找到的冲突路径起点是当前变迁，则路径无需分岔；
////            if (t_idx == t2cftT.first.first && call2T.first == t2cftT.first.second) {
////                t_path.clear();
////                return t_path;
////            }
////            //若冲突路径包含另一个与当前变迁冲突的变迁，则无需分岔，优先计算更早的冲突变迁；
////            if (has_earlier_cft(t2cftT.second, make_pair(t_idx, call2T.first), call2T)) {
////                t_path.clear();
////                return t_path;
////            }
//
//            t_path.emplace(t_idx, call2T.first);
//        } else {//变迁没有确定的call
//            bool allPass = true;//变迁t对应的全部call都无法发生;
//            for (auto j = cpn->map_t_call2thread.begin(); j != cpn->map_t_call2thread.end(); j++) {
//                if (j->first.first != t_idx)
//                    continue;
//                //若冲突路径包含另一个与当前变迁冲突的变迁，则不添加该（变迁，call），若该变迁所有call都无法添加，则路径无法发生
//                if (has_earlier_cft(t2cftT.second, j->first, call2T)) {
//                    continue;
//                }
//                //若j->(t,call)与原变迁(t,call)相同，即冲突路径需从原变迁出发，则路径无需分岔
//                if (j->first == t2cftT.first)
//                    continue;
//
//                t_path.emplace(t_idx, j->first.second);
//                allPass= false;
//            }
//            if(allPass){
//                t_path.clear();
//               return t_path;
//            }
//        }
//    }
//    //添加call2T中的变迁
//    for (auto i = call2T.second.begin(); i != call2T.second.end(); i++) {
//        t_path.emplace(*i, call2T.first);
//    }
//    return t_path;
//}

bool not_join_cex(UNFPDN *unfpdn, type_D D, Cex_A *cexA, type_T2Call t_cur, type_Call2T call2cftT) {
    //t'与D中的t相同;
    for (auto i = D.begin(); i != D.end(); i++) {
        auto e_idx = *i;
        auto e = unfpdn->event[e_idx];
        if ((call2cftT.second.find(e.cpn_index) != call2cftT.second.end()) && call2cftT.first == e.call_stack)
            return true;
    }
    //A中已经存在（t_cur,conflictT);
    for (auto i = cexA->begin(); i != cexA->end(); i++) {
        if (i->t2Call == t_cur && i->Call == call2cftT.first && i->conflictT == call2cftT.second)
            return true;
    }

    return false;
}

type_cftPath
UNFOLDING::get_t_path(type_D D, Cex_A *cexA, pair<type_T2Call, type_multimapCall2T> t2cftT, set<type_T2Call> en_t2Call,
                      type_Call2T &call2T) {
    Cft_Tpath *cft_Tpath = new Cft_Tpath(cpn, t2cftT, en_t2Call, cexA);
    auto t_cur = t2cftT.first;
    auto call2Ts = t2cftT.second;
    for (auto i_call2T = call2Ts.begin(); i_call2T != call2Ts.end(); i_call2T++) {
        if (not_join_cex(unfpdn, D, cexA, t_cur, *i_call2T))
            continue;
        call2T = *i_call2T;

        cft_Tpath->get_t_path_step(call2T);
        if (cft_Tpath->t_path.empty())
            continue;
        else return cft_Tpath->t_path;
    }
    return cft_Tpath->t_path;
}

type_D get_en_D(Config *C, type_D D) {
    type_D en_D;
    for (auto ien = C->en.begin(); ien != C->en.end(); ien++) {
        auto pe = *ien;
        if (pe->had_extend) {
            if (exist_in(D, pe->e_idx))
                en_D.emplace(en_D.begin(), pe->e_idx);
        }
    }
    return en_D;
}

bool UNFOLDING::Alt(Config *C, type_D D, Cex_A *cexA_plus, Cex_A *cex_A) {
    //D ∩ en(C)
    type_D en_D = get_en_D(C, D);

    auto en_T2Call = get_en_T2Call(C->en);
    for (auto ien_D = en_D.begin(); ien_D != en_D.end(); ien_D++) {
        auto e = unfpdn->event[*ien_D];
        index_t t_idx = e.cpn_index;
        auto call = e.call_stack;
        auto i_t2conflictT = cpn->map_t2conflictT.find(make_pair(t_idx, call));
        if (i_t2conflictT == cpn->map_t2conflictT.end())
            return false;

        //同一语句对应的变迁，只需取其中之一查找路径，因为共同路径相同;
        type_Call2T call2T;
        auto t_path = get_t_path(D, cex_A, *i_t2conflictT, en_T2Call, call2T);
        if (t_path.empty())
            continue;

        (*cexA_plus) = (*cex_A);
        Cex_texture cex_texture(make_pair(t_idx, call), call2T, t_path);
        cexA_plus->push_back(cex_texture);
        C->firstStep = false;
        return true;
//        for (auto i_row2T = i_t2conflictT->second.begin(); i_row2T != i_t2conflictT->second.end(); i_row2T++) {
//            if (not_join_cex(C, *i_row2T, cex_A, cpn))
//                continue;
//
//            //同一语句对应的变迁，只需取其中之一查找路径，因为共同路径相同;
//            auto t_path = get_t_cex(cpn, i_row2T->second, en_T);
//            (*cexA_plus) = (*cex_A);
//            Cex_texture cex_texture(t_idx, i_row2T->second, t_path);
//            cexA_plus->push_back(cex_texture);
//            return true;
//        }
    }
    return false;
}

void UNFOLDING::print_ExploreTree(std::string filename) {
    ofstream out;
    out.open(filename + ".txt", ios::out);
    //out << "Place:" << endl;
    //out << "-----------------------------------" << endl;

    int P_width, T_width, T_height, font_size = 30;
    P_width = T_width = T_height = 1;

    string fillcolor = "chartreuse";
    string fillcolor1 = "lightblue";

    for (int i = 0; i < nodsCount; i++) {
        if (nods[i]->e_idx == 65535) {
            if (!nods[i]->has_Cft) {
                out << "subgraph cluster_" << nods[i]->id << "{" << "fontsize = " << to_string(font_size) << ";label=\""
                    << "no_e" << "\";color=\"white\"" << nods[i]->id <<
                    "[shape=circle" << ",fontsize = " << to_string(font_size) << ",width=" << to_string(P_width)
                    << ",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor1 << "\"]}" << endl;
            } else {
                out << "subgraph cluster_" << nods[i]->id << "{" << "fontsize = " << to_string(font_size) << ";label=\""
                    << "no_e" << " Cft:" << nods[i]->cftRow << "\";color=\"white\"" << nods[i]->id <<
                    "[shape=circle" << ",fontsize = " << to_string(font_size) << ",width=" << to_string(P_width)
                    << ",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor1 << "\"]}" << endl;
            }
        } else {
            auto e = unfpdn->event[nods[i]->e_idx];
            auto t = cpn->findT_byindex(e.cpn_index);
            auto t_Row = t->getRow();
            if (t_Row == 65535) {
                auto iter = cpn->map_NoneRow.find(t->getid());
                if (iter != cpn->map_NoneRow.end())
                    t_Row = iter->second;
            }
            if (!nods[i]->has_Cft) {
                out << "subgraph cluster_" << nods[i]->id << "{" << "fontsize = " << to_string(font_size) << ";label=\""
                    << t_Row << "\";color=\"white\"" << nods[i]->id <<
                    "[shape=circle" << ",fontsize = " << to_string(font_size) << ",width=" << to_string(P_width)
                    << ",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor1 << "\"]}" << endl;
            } else {
                out << "subgraph cluster_" << nods[i]->id << "{" << "fontsize = " << to_string(font_size) << ";label=\""
                    << t_Row << " Cft:" << nods[i]->cftRow << "\";color=\"white\"" << nods[i]->id <<
                    "[shape=circle" << ",fontsize = " << to_string(font_size) << ",width=" << to_string(P_width)
                    << ",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor1 << "\"]}" << endl;
            }
        }
    }

    for (int i = 0; i < nods.size(); i++) {
        if (nods[i]->LChild)
            out << nods[i]->id << "->" << nods[i]->LChild->id << "[label=\"" << "\"]" << endl;
        if (nods[i]->RChild)
            out << nods[i]->id << "->" << nods[i]->RChild->id << "[label=\"" << "\"]" << endl;
    }
    out.close();

    readGraph(filename + ".txt", filename + ".dot");
    makeGraph(filename + ".dot", filename + ".png");
}


Nod *UNFOLDING::createNod(Config *C, type_D D, Cex_A *cexA, NodChild isChild, Nod *PNod) {
    Nod *nod = new Nod(C, D, cexA);
    nod->id = "N" + to_string(nodsCount);
    for (auto i = D.begin(); i != D.end(); i++) {
        auto e = unfpdn->event[*i];
        nod->D_t2call.emplace(e.cpn_index, e.call_stack);
    }

    nods.push_back(nod);
    nodsCount++;
    switch (isChild) {
        case RootNod: {
            break;
        }
        case LChildNod: {
            PNod->LChild = nod;
            break;
        }
        case RChildNod: {
            PNod->RChild = nod;
            break;
        }
    }
    return nod;
}

bool conflictT_fired(Config *C, Cex_A *cexA, UNFPDN *unfpdn) {
    if (C->e_Vec.empty() || cexA->empty())
        return false;
    auto t_idx = unfpdn->event[C->e_last].cpn_index;
    auto t_call = unfpdn->event[C->e_last].call_stack;
    auto conflictT = cexA->back().conflictT;
    auto conflictT_call = cexA->back().Call;
    for (auto i = conflictT.begin(); i != conflictT.end(); i++) {
        if (t_idx == *i && t_call == conflictT_call)
            return true;
    }
    return false;
}

bool UNFOLDING::needNo_Explore(Nod *nod, PE *pe) {
    if (unfpdn->event[pe->e_idx].is_cutoff) {
//        auto t = cpn->findT_byindex(unfpdn->event[pe->e_idx].cpn_index);
//        cout << pe->e_idx << " " << t->getid() << "  call:" << pe->call_stack.back().front() << "  row:" << t->getRow()
//             << "  cutoff" << endl;
        return true;
    }
//    if(nod->D_t2call.find(make_pair(pe->t_idx,pe->call_stack))!=nod->D_t2call.end()){
//        return true;
//    }
    return false;
}

void UNFOLDING::Remove(index_t e, Config *C, type_D D) {
    set<index_t> Q;//Q(C,D,U)
    for (auto i = D.begin(); i != D.end(); i++) {
        Q.emplace(*i);
        for (auto j = C->en.begin(); j != C->en.end(); j++) {
            if (imm_conflict(unfpdn->event[*i], *j))
                Q.emplace((*j)->e_idx);
        }
    }

    if (Q.find(e) != Q.end())
        unfpdn->G.emplace(e);

}

//void UNFOLDING::ExtendFire(Config *C, PE *pe) {
//    auto t = cpn->findT_byindex(pe->t_idx);
//    auto binding = pe->binding;
//    auto x = get_x(t, C->cut, binding, cpn);
//    unfpdn->add_Event(cpn, synch, , pe);
//}

void UNFOLDING::Extend(Config *C) {
    if (!C->en.empty())//如果扩展过，则跳过扩展
        return;
    if (C->e_Vec.empty() && C->cut == NULL) {//根节点
        CUT *cut = new CUT(cpn);
        *cut = *unfpdn->min_o;
        C->cut = cut;
        //heuristic
        MultiSet multiset;
        Token_map tokens;
        auto place = cpn->getplacearr();
        for (auto i = synch->q_begin_idx; i < synch->q_begin_idx + synch->q_count; i++) {
            multiset = place[i].getMultiSet();
            tokens = multiset.getmapTokens();
            auto token = tokens.begin();
            if (token != tokens.end()) {
                C->q_cur = i;
            }
        }
    }

    //获取可能扩展;
    for (index_t i = 0; i < cpn->get_transcount(); i++) {

        if (synch->CP_tran.find(i) != synch->CP_tran.end())//CP是LTLF公式涉及变迁的复制变迁，用于可使能判断，不可发生
            continue;
        if (cpn->findT_byindex(i)->isabort)
            continue;
        vector<Binding_unf *> bindings;
        if (is_firable(i, C, bindings)) {
//            cout << "new:" << cpn->findT_byindex(i)->getid() << endl;
            for (auto binding = bindings.begin(); binding != bindings.end(); binding++) {
                PE *pe = new PE(i, *binding);
                auto t = cpn->findT_byindex(i);
                pe->x = get_x(t, C->cut, *binding, cpn);
                unfpdn->cal_call(cpn,synch, pe);

                //检测是否扩展过。
                set<index_t> readP;
                bool is_readT = cpn->is_readT(i, readP);
                if (is_readT) {
                    set<index_t> read_varB;
                    for (auto j = pe->x.begin(); j != pe->x.end(); j++) {
                        if (readP.find(unfpdn->condition[*j].cpn_index) != readP.end()) {
                            read_varB.emplace(*j);
                        }
                    }
                    unfpdn->had_extend(cpn, C, pe, true, read_varB);
                } else {
                    unfpdn->had_extend(cpn, C, pe);
                }
                C->en.emplace(C->en.begin(), pe);
            }
        }
    }
}

void filter_A(Cex_A *cexA) {
    auto t2Call_Ori = cexA->back().t2Call;
    auto tFireds = cexA->back().conflictT;
    for (auto i = cexA->begin(); i != cexA->end(); i++) {
        if (i->t2Call == t2Call_Ori) {
            cexA->erase(i, cexA->end());
            break;
        }
    }
}

void handle_conflictT_fired(Config *C, type_D &D, Cex_A *cexA, UNFPDN *unfpdn) {
    //conflict(t-->t'),t'发生后
    //将D中对应t的事件删除。因为后续发生的t对应的事件不会是D中的事件
    auto t2Call_last = cexA->back().t2Call;
    for (auto i = D.begin(); i != D.end();) {
        if (unfpdn->event[*i].cpn_index == t2Call_last.first && unfpdn->event[*i].call_stack == t2Call_last.second)
            i = D.erase(i);
        else
            i++;
    }
    //如果最后一个变迁是冲突变迁，则删除cexA中的路径优先;
    filter_A(cexA);
}

void UNFOLDING::Explore(Nod *nod) {
    elapse_end=clock();
    if((elapse_end-elapse_begin)/1000000.0>MaxTime) {
        timeup= true;
        return;
    }

    auto C = nod->C;
    auto cexA = nod->cexA;
    auto D = nod->D;
    if (C->firstStep && conflictT_fired(C, cexA, unfpdn)) {
        handle_conflictT_fired(C, D, cexA, unfpdn);
    }
    if (C->is_cutoff)
        return;

    Extend(C);

    if (C->en.empty()) {
        //到达程序终点，默认存在自循环，判断是否是case2
        unfpdn->is_counterexample_endpoint(C);
        return;
    }
    PE *pe;
    index_t e;

    C->firstStep = true;
    bool has_LChild = true;
    if (!cexA->empty()) {
        auto ie = C->en.begin();
        for (; ie != C->en.end(); ie++) {
            pe = *ie;
            auto ipath = cexA->back().path.begin();
            for (; ipath != cexA->back().path.end(); ipath++) {
                if (pe->t_idx == ipath->first && pe->call_stack == ipath->second) {
                    unfpdn->add_Event(cpn, synch, nod, pe);
                    if (unfpdn->success)
                        return;
                    if (needNo_Explore(nod, pe))
                        continue;
                    e = pe->e_idx;
                    break;
                }
            }
            if (ipath != cexA->back().path.end())
                break;
        }
        if (ie == C->en.end())
            has_LChild = false;
    } else {
        auto i = C->en.begin();
        for (; i != C->en.end(); i++) {
            pe = *i;
            for (auto j = i; j != C->en.end(); j++) {
                if (synch->I_idx.find((*j)->t_idx) != synch->I_idx.end())
                    pe = *j;
            }
            unfpdn->add_Event(cpn, synch, nod, pe);
            if (unfpdn->success)
                return;
            if (needNo_Explore(nod, pe))
                continue;
            e = pe->e_idx;
            break;
        }
        if (i == C->en.end())
            has_LChild = false;
    }
    if (has_LChild) {
//        //debug
//        auto t=cpn->findT_byindex(unfpdn->event[e].cpn_index);
//        cout<<unfpdn->eventcount<<"  "<<t->getid()<<" row:"<<t->getRow()<<endl;

        nod->e_idx = e;
        auto C_plus = get_Config_add(C, e, cexA);;//C ∪ {e}
        Cex_A *cexA_minus = new Cex_A;
        *cexA_minus = *cexA_sub(cexA, make_pair(unfpdn->event[e].cpn_index, unfpdn->event[e].call_stack));//A ∖ {e}
        Nod *LChild_nod = createNod(C_plus, D, cexA_minus, LChildNod, nod);
        Explore(LChild_nod);

        if (unfpdn->success)
            return;
    }

    auto D_plus = D;
    if (has_LChild)
        D_plus.push_back(e);

    Cex_A *cexA_plus = new Cex_A;

    if (Alt(C, D_plus, cexA_plus, cexA)) {
        Nod *RChild_nod = createNod(C, D_plus, cexA_plus, RChildNod, nod);
        auto cftT = cpn->findT_byindex(*cexA_plus->back().conflictT.begin());
        nod->has_Cft = true;
        if (cftT->getRow() != 65535)
            nod->cftRow = cftT->getRow();
        else {
            auto iter = cpn->map_NoneRow.find(cftT->getid());
            if (iter != cpn->map_NoneRow.end())
                nod->cftRow = iter->second;
            else nod->cftRow = NoneRow;
        }

        Explore(RChild_nod);
    }
}

void UNFOLDING::unfolding() {
    init();
    Config *C = new Config;
    type_D D;
    Cex_A *cexA = new Cex_A;//cex
    Nod *nod = createNod(C, D, cexA, RootNod);
    Explore(nod);
//    unfpdn->print_UNF("unfolding",cpn);
//    print_ExploreTree("ExploreTree");
    cout << endl << "UNFOLDING:" << endl;
    if(timeup){
        result="time out";
        cout<<"time out"<<endl;
    } else {
        switch (unfpdn->success) {
            case NONE:
                result = "True";
                cout << "TRUE" << endl;
                break;
            case CASE1:
                result = "False";
                cout << "FALSE" << endl;
                break;
            case CASE2:
                result = "False";
                cout << "FALSE" << endl;
                break;
        }
    }
    cout << "Condition Count:" << unfpdn->conditioncount << endl;
    cout << "Event Count:" << unfpdn->eventcount << endl;
    cout << "Nod Count:" << nodsCount << endl;
}
