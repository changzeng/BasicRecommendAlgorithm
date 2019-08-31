//
// Created by changzeng on 2019/8/31.
//

#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

typedef vector<float> Sample;

class LogisticRegression{
private:
    vector<float> w;
    vector<float> tmp_w;
    vector<Sample> sample_list;
    vector<float> labels;
    float b;
    float tmp_b=0;
    float alpha;
    int fea_num;
    int epoch_num;
public:
    explicit LogisticRegression(int fea_num, int epoch_num=10, float alpha=0.5):fea_num(fea_num),
    epoch_num(epoch_num),alpha(alpha){
        srand(time(nullptr));
        for(int i=0; i<fea_num; i++){
            w.push_back(random_float());
            tmp_w.push_back(0);
        }
        b=random_float();
        load_data();
    }
    static float random_float(){
        int base=500;
        return (float)((int)random()%(2*base+1)-base)/(float)base;
    }
    static float sigmoid(float x){
        return (float)1.0/(1+exp(-x));
    }
    void load_data(){
        ifstream reader("/home/changzeng/CLionProjects/LogisticRegression/feature.txt");
        int sample_ind=0;
        while(!reader.eof()){
            sample_list.emplace_back(fea_num, 0);
            labels.push_back(0);
            for(int i=0; i<fea_num; i++){
                reader>>sample_list[sample_ind][i];
            }
            reader>>labels[sample_ind];
            sample_ind++;
        }
    }
    void train(){
        for(int epoch=1; epoch<=epoch_num; epoch++){
            for(int i=0; i<fea_num; i++){
                tmp_w[i]=0;
            }
            int correct_num=0;
            for(int sample_ind=0; sample_ind<(int)sample_list.size(); sample_ind++){
                float p_val = 0;
                for(int fea_ind=0; fea_ind<fea_num; fea_ind++){
                    p_val += w[fea_ind]*sample_list[sample_ind][fea_ind];
                }
                p_val = sigmoid(p_val+b);
                if((p_val>=0.5?1:0) == (int)labels[sample_ind]){
                    correct_num++;
                }
                for(int fea_ind=0; fea_ind<fea_num; fea_ind++){
                    tmp_w[fea_ind] += (labels[sample_ind]-p_val)*sample_list[sample_ind][fea_ind];
                }
                tmp_b += labels[sample_ind]-p_val;
            }
            for(int i=0; i<fea_num; i++){
                w[i] += alpha*tmp_w[i]/(float)sample_list.size();
            }
            b += alpha*tmp_b/(float)sample_list.size();
            cout.precision(2);
            cout<<(float)correct_num/(float)sample_list.size()<<endl;
        }
    }
    void predict(vector<Sample>& fea_list, vector<float>& res){
        res.clear();
        for(const Sample& fea:fea_list){
            float p_val=0;
            for(int fea_ind=0; fea_ind<fea_num; fea_ind++){
                p_val += fea[fea_ind]*w[fea_ind];
            }
            res.push_back(sigmoid(p_val+b));
        }
    }
    void print_size(){
        cout<<sample_list.size()<<" "<<labels.size()<<endl;
    }
};

int main(){
    LogisticRegression lr(50);
    lr.train();
    return 0;
}
