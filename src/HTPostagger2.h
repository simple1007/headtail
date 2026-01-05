#ifndef HTPOSTAGGER2_H
#define HTPOSTAGGER2_H
// #include <windows.h>
#include "BaseORT.h"
#include "pos_dict.pb.h"
#include "hftokenizer.h"
#include <fstream>
#include <iostream>
#include <string>
#include "utils.h"
#include "Matrix.h"

using namespace std;
using namespace htpostagger;

bool ReadUserPreferencesFromFilePOS(const std::string& filename, PosDict* message);

class PosTaggerData {
public:
    PosTaggerData(google::protobuf::Map<int64_t,std::string> dict);
    // 이 맵은 초기화 후에는 변경되지 않는다고 가정합니다.
    const google::protobuf::Map<int64_t, std::string>& get_word_to_id_map() const;

private:
    google::protobuf::Map<int64_t,std::string> word_to_id_map_; 
};


class HTPostagger2: public BaseORT {
public:
    // google::protobuf::Map<int64_t,std::string> dict;
    shared_ptr<const PosTaggerData> dict;
    HTPostagger2(const wchar_t* model_path,const string& dictpath,const string& tokenizerpath,const shared_ptr<const PosTaggerData> dict);
    ~HTPostagger2();
    HTPostagger2(const HTPostagger2 &);
    // vector<vector<int>> encode_batch(const unique_ptr<HFTokenizer>& tokenizer, vector<string>& data);
    MatrixFloat get_tokens(HFTokenizer* hftok,const vector<string>& data);
    vector<Morphs> postagger(HFTokenizer* hftok,const vector<string>& data_);
    MatrixMorph postagger2(HFTokenizer* hftok,const vector<string>& data_);
    
    MatrixString run(const MatrixFloat& data);
    // vector<vector<string>> get_prob(vector<float> output_tensors,vector<int64_t> input_shape,int mode);
};

#endif