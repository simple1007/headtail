#pragma once

#ifndef HTTokenizer2_H
#define HTTokenizer2_H

// #include <windows.h>
#include "utils.h"

// #include <cstdlib>
#include "onnxruntime_c_api.h"
#include "onnxruntime_cxx_api.h"
#include <regex>
#include "Matrix.h"
#include "tok_dict.pb.h"
#include "BaseORT.h"
#include <map>
#include <vector>
#include <fstream>
#include <string>


using namespace std;
using namespace httokenizer;

class TokenizerData {
public:
    TokenizerData(google::protobuf::Map<std::string,int64_t> dict);
    // 이 맵은 초기화 후에는 변경되지 않는다고 가정합니다.
    const google::protobuf::Map<std::string, int64_t>& get_word_to_id_map() const;

private:
    google::protobuf::Map<std::string, int64_t> word_to_id_map_; 
};


bool ReadUserPreferencesFromFileTOK(const std::string& filename, TokDict* message);

vector<float> w2i(const wstring& wstr,const shared_ptr<const TokenizerData>& dict_,int maxlen);

int get_maxlen(const vector<string>& data,vector<wstring>& inputtext);

// class TokenizerData {
// public:
//     // 이 맵은 초기화 후에는 변경되지 않는다고 가정합니다.
//     const google::protobuf::Map<std::string, int64_t>& get_word_to_id_map() const {
//         return word_to_id_map_; // 읽기 전용으로 제공
//     }

// private:
//     google::protobuf::Map<std::string, int64_t> word_to_id_map_; 
// };

class HTTokenizer2: public BaseORT {
public:
    std::shared_ptr<const TokenizerData> dict;
    // google::protobuf::Map<std::string,int64_t> dict;
    HTTokenizer2(const wchar_t* model_path,const string& dictpath, const shared_ptr<const TokenizerData>& dict);
    ~HTTokenizer2();
    HTTokenizer2(const HTTokenizer2&);
    vector<string> tokenizer(vector<string> data_);
    vector<string> make_tok_res(vector<int32_t> resprob,vector<wstring> text,int maxlen);
   
    vector<int32_t> run(const MatrixFloat& data);
};

#endif