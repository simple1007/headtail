
// #pragma once
// #ifndef HTPOSTAGGER_H
// #define HTPOSTAGGER_H

// #include "BaseORT.h"
// #include "pos_dict.pb.h"
// #include "hftokenizer.h"
// #include <fstream>
// #include <iostream>
// #include <string>
// #include "utils.h"

// using namespace std;
// using namespace htpostagger;

// struct Morphs {
//     vector<string> toks;
//     vector<string> poss;
// };

// bool ReadUserPreferencesFromFilePOS(const std::string& filename, PosDict* message);

// class HTPostagger: public BaseORT {
// public:
//     google::protobuf::Map<int64_t,std::string> dict;
//     // shared_ptr<HFTokenizer> tokenizer;
//     HTPostagger(const wchar_t* model_path,const string& dictpath,const string& tokenizerpath);
//     ~HTPostagger();

//     // vector<vector<int>> encode_batch(const unique_ptr<HFTokenizer>& tokenizer, vector<string>& data);
//     vector<vector<float>> get_tokens(HFTokenizer* hftok,const vector<string>& data);
//     vector<Morphs> postagger(HFTokenizer* hftok,const vector<string>& data_);
    
//     vector<vector<string>> run(vector<vector<float>> data);
//     vector<vector<string>> get_prob(vector<float> output_tensors,vector<int64_t> input_shape,int mode);
// };

// #endif