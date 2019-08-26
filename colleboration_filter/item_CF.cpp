//
// Created by Administrator on 22/08/2019.
//

#include <set>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <unordered_map>

using namespace std;

typedef pair<string, float> sim_pair;
typedef set<string> hash_set;
typedef unordered_map<string, hash_set> inverse_table;

struct cmp{
    bool operator()(const sim_pair& a, const sim_pair& b){
        return a.second>b.second;
    }
};

class ItemCf{
public:
    void load_data(){
        ifstream reader;
        reader.open("F:\\project\\BasicRecommendAlgorithm\\colleboration_filter\\user_act.txt", ios::in);
        string user, item;
        while(!reader.eof()){
            reader>>user>>item;
            if(u_to_item.find(user)==u_to_item.end()){
                u_to_item[user] = hash_set();
            }
            if(i_to_user.find(item)==i_to_user.end()){
                i_to_user[item] = hash_set();
            }
            u_to_item[user].insert(item);
            i_to_user[item].insert(user);
        }
        reader.close();
    }
    float cal_sim(string &a, string &b){
        auto res_a = i_to_user.find(a);
        auto res_b = i_to_user.find(b);
        if(res_a==i_to_user.end() || res_b==i_to_user.end())
            return 0.0;
        auto set_a = res_a->second;
        auto set_b = res_b->second;
        vector<string> u_inter, u_union;
        set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), inserter(u_inter, u_inter.begin()));
        set_union(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), inserter(u_union, u_union.begin()));
        return (float)u_inter.size()/u_union.size();
    }
    void cal_item_similarity(){
        vector<string> item_list;
        for(auto & iter : u_to_item){
            auto item_set = iter.second;
            for(const auto& item:item_set){
                item_list.push_back(item);
            }
            for(int i=0; i<(int)item_list.size(); i++){
                for(int j=i+1; j<(int)item_list.size(); j++){
                    string item_i = item_list[i];
                    string item_j = item_list[j];
                    string q_str_1 = item_i+"_"+item_j;
                    string q_str_2 = item_j+"_"+item_i;
                    if(item_sim.find(q_str_1)==item_sim.end() && item_sim.find(q_str_2)==item_sim.end()){
                        float sim = cal_sim(item_i, item_j);
                        item_sim[q_str_1] = sim;

                        vector<string> tmp_item_list({item_i, item_j});
                        for(int item_ind=0; item_ind<2; item_ind++){
                            string item=tmp_item_list[item_ind];
                            string item_o=tmp_item_list[1-item_ind];
                            auto sim_res = i_to_sim_i.find(item);
                            if(sim_res == i_to_sim_i.end()){
                                i_to_sim_i[item] = vector<sim_pair>();
                                sim_res = i_to_sim_i.find(item);
                            }
                            auto& sim_q = sim_res->second;
                            sim_q.emplace_back(item_o, sim);
                            push_heap(sim_q.begin(), sim_q.end(), cmp());
                            while(sim_q.size()>sim_item_top_k){
                                pop_heap(sim_q.begin(), sim_q.end(), cmp());
                                sim_q.pop_back();
                            }
                        }
                    }
                }
            }
            item_list.clear();
        }
    }
    float query(string &a, string &b){
        string q_str1 = a+"_"+b;
        string q_str2 = b+"_"+b;
        auto res1=item_sim.find(q_str1);
        if(res1 == item_sim.end()){
            auto res2=item_sim.find(q_str2);
            if(res2==item_sim.end()){
                return 0;
            }
            return res2->second;
        }
        return res1->second;
    }
    vector<sim_pair> recommend(const string& user){
        vector<sim_pair> res;
        auto u_to_item_res = u_to_item.find(user);
        if(u_to_item_res == u_to_item.end()){
            return res;
        }
        unordered_map<string, float> item_to_score;
        for(const auto& item:u_to_item_res->second){
            auto q_res = i_to_sim_i.find(item);
            if(q_res == i_to_sim_i.end()){
                continue;
            }
            for(const auto &sim_item:q_res->second){
                if(item_to_score.find(sim_item.first)==item_to_score.end()){
                    item_to_score[sim_item.first]=0;
                }
                item_to_score[sim_item.first]+=sim_item.second;
            }
        }
        hash_set user_visit_items;
        auto q_u_to_item = u_to_item.find(user);
        if(q_u_to_item != u_to_item.end()){
            user_visit_items = q_u_to_item->second;
        }
        for(const auto& iter:item_to_score){
            if(user_visit_items.find(iter.first)!=user_visit_items.end())
                continue;
            res.emplace_back(iter.first, iter.second);
            push_heap(res.begin(), res.end(), cmp());
            while(res.size()>recommend_top_k){
                pop_heap(res.begin(), res.end(), cmp());
                res.pop_back();
            }
        }

        return res;
    }
private:
    int sim_item_top_k=10;
    int recommend_top_k=20;
    inverse_table u_to_item;
    inverse_table i_to_user;
    unordered_map<string, float> item_sim;
    unordered_map<string, vector<sim_pair>> i_to_sim_i;
};

int main(){
    ItemCf item_cf;
    item_cf.load_data();
    item_cf.cal_item_similarity();
    string user_id="1";
    vector<sim_pair> res = item_cf.recommend(user_id);
    for(const auto& res_pair:res){
        cout<<res_pair.first<<"  "<<res_pair.second<<endl;
    }
    return 0;
}
