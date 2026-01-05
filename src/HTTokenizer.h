// #pragma once

// #ifndef HTTOKENIZER_H
// #define HTTOKENIZER_H

// #include "tok_dict.pb.h"
// #include "BaseORT.h"
// #include <map>
// #include <vector>
// #include <fstream>
// #include <string>


// using namespace std;
// using namespace httokenizer;


// bool ReadUserPreferencesFromFileTOK(const std::string& filename, TokDict* message);

// vector<float> w2i(const wstring& wstr,const google::protobuf::Map<std::string,int64_t>& dict,int maxlen);

// int get_maxlen(const vector<string>& data,vector<wstring>& inputtext);

// class HTTokenizer: public BaseORT {
// public:
//     google::protobuf::Map<std::string,int64_t> dict;
//     HTTokenizer(const wchar_t* model_path,const string& dictpath);
//     ~HTTokenizer();
//     vector<string> tokenizer(vector<string> data_);
//     vector<string> make_tok_res(vector<vector<int>> resprob,vector<wstring> text,int maxlen);
   
//     vector<vector<int>> run(vector<vector<float>> data);
//     vector<vector<int>> get_prob(vector<float> output_tensors,vector<int64_t> input_shape,int mode);
// };

// #endif