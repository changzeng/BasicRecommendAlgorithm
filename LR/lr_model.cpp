#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
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
    float dev_frac;
    int fea_num;
    int epoch_num;
    int split_ind, sample_num;
    float early_stop;
public:
    explicit LogisticRegression(int fea_num=-1, int epoch_num=100, float alpha=1.0, float dev_frac=0.2,
            float early_stop=0.001):fea_num(fea_num),epoch_num(epoch_num),alpha(alpha),dev_frac(dev_frac),
            early_stop(early_stop){
        srand(time(nullptr));
        b=random_float();
        if(fea_num==-1){
            read_csv("imdb.csv");
            for(int i=0; i<this->fea_num; i++){
                w.push_back(random_float());
                tmp_w.push_back(0);
            }
        }else{
            for(int i=0; i<fea_num; i++){
                w.push_back(random_float());
                tmp_w.push_back(0);
            }
            load_data();
        }
        shuffle();
        sample_num=split_ind=(int)((float)sample_list.size()*(1-dev_frac));
    }
    // generate random float number between -1.0 and 1.0
    static float random_float(){
        int base=500;
        return (float)((int)random()%(2*base+1)-base)/(float)base;
    }
    static float sigmoid(float x){
        return (float)1.0/(1+exp(-x));
    }
    // load csv file
    void read_csv(const string& file_name="iris.csv"){
        cout<<"starting read csv file..."<<endl;
        string full_file_name = "/home/changzeng/CLionProjects/LogisticRegression/"+file_name;
        ifstream reader(full_file_name);
        string line_str;
        int line_num=1, col_num=0, col_ind=0;
        vector<float> tmp_fea;
        while(getline(reader, line_str)){
            // cout<<line_num<<endl;
            stringstream ss(line_str);
            string str;
            col_ind=0;
            tmp_fea.clear();
            while(getline(ss, str, ',')){
                if(line_num==1){
                    ++col_num;
                }else{
                    if(0<col_ind && col_ind<col_num){
                        if(col_ind==col_num-1){
                            labels.push_back(atof(str.c_str()));
                        }else{
                            tmp_fea.push_back(atof(str.c_str()));
                        }
                    }
                }
                ++col_ind;
            }
            if(line_num!=1) sample_list.push_back(tmp_fea);
            ++line_num;
        }
        fea_num=col_num-2;
        cout<<"read csv file done"<<endl;
    }
    // random shuffle data
    void shuffle(){
        int sample_num = sample_list.size();
        int shuffle_epoch=sample_num*2;
        int x, y;
        for(int i=0; i<shuffle_epoch; i++){
            x = (int)rand()%sample_num;
            y = (int)rand()%sample_num;
            swap(sample_list[x], sample_list[y]);
            swap(labels[x], labels[y]);
        }
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
        cout<<"start training..."<<endl;
        float cur_dev_accu=0, pre_dev_accu=0;
        for(int epoch=1; epoch<=epoch_num; epoch++){
            cur_dev_accu=dev();
            for(int i=0; i<fea_num; i++){
                tmp_w[i]=0;
            }
            int correct_num=0;
            for(int sample_ind=0; sample_ind<sample_num; sample_ind++){
                float p_val = predict_one(sample_list[sample_ind]);
                if((p_val>=0.5?1:0) == (int)labels[sample_ind]){
                    correct_num++;
                }
                for(int fea_ind=0; fea_ind<fea_num; fea_ind++){
                    tmp_w[fea_ind] += (labels[sample_ind]-p_val)*sample_list[sample_ind][fea_ind];
                }
                tmp_b += labels[sample_ind]-p_val;
            }
            for(int i=0; i<fea_num; i++){
                w[i] += alpha*tmp_w[i]/(float)sample_num;
            }
            b += alpha*tmp_b/(float)sample_list.size();
            cout.precision(2);
            float train_accu=(float)correct_num/(float)sample_num;
            cout<<"training accuracy: "<<train_accu<<", dev accuracy: "<<cur_dev_accu<<endl;
            if(abs(cur_dev_accu-pre_dev_accu)<=early_stop){
                cout<<"early stop"<<endl;
                break;
            }
            pre_dev_accu=cur_dev_accu;
        }
    }
    float dev(){
        int total=0, accu=0;
        for(int i=split_ind; i<(int)sample_list.size(); i++){
            if(predict_one(sample_list[i])>=0.5 && labels[i]>=1.0) accu++;
            if(predict_one(sample_list[i])<0.5 && labels[i]==0.0) accu++;
            ++total;
        }
        return (float)accu/(float)total;
    }
    float predict_one(const Sample& fea){
        float p_val=0;
        for(int fea_ind=0; fea_ind<fea_num; fea_ind++){
            p_val += fea[fea_ind]*w[fea_ind];
        }
        return sigmoid(p_val+b);
    }
    void predict(vector<Sample>& fea_list, vector<float>& res){
        res.clear();
        for(const Sample& fea:fea_list){
            res.push_back(predict_one(fea));
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
