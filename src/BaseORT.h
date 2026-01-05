#pragma once

#ifndef BASEORT_H
#define BASEORT_H

#include "onnxruntime_c_api.h"
#include "onnxruntime_cxx_api.h"
#include <iostream>
#include <vector>
#include <string>


using namespace std;

class BaseORT{
public:
    //wchar_t* model_path;
    // std::unique_ptr<Ort::Session> session;
    // 1. Ort::Env는 모든 세션이 공유하며, 세션보다 오래 살아남아야 하므로 shared_ptr로 관리합니다.
    std::shared_ptr<Ort::Env> env;
    
    // 2. Ort::Session 역시 공유 포인터로 관리하여 생명주기를 자동으로 관리합니다.
    std::shared_ptr<Ort::Session> session;
    // std::unique_ptr<Ort::Env> env;
    const char* input_node_names[1] = {"input"};
    const char* output_node_names[1] = {"output"};
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    // std::shared_ptr<Ort::Session> init_session(const wchar_t* model_path_);
    Ort::Value make_input(vector<float>& res,vector<vector<float>>& data,size_t input_data_size,vector<int64_t> input_shape);
};

#endif