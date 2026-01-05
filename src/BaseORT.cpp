#include "BaseORT.h"
#include "utils.h"
using namespace std;


// std::shared_ptr<Ort::Session> BaseORT::init_session(const wchar_t* model_path_) {
    
//     Ort::SessionOptions session_options;
    
//     session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
//     session_options.SetInterOpNumThreads(2);
//     session_options.SetIntraOpNumThreads(6);
//     // session_options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);
//     // this->input_node_names[0] = "input";
//     // this->output_node_names[0] = "output"
//     session_options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
//     // wstring model_path = s2ws(model_path_);
//     //this->model_path = model_path.c_str();
//     this->env = std::make_shared<Ort::Env>(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, "Default");
//     return std::make_shared<Ort::Session>(*this->env, model_path_, session_options);
// }

// Ort::Value BaseORT::make_input(vector<float>& res,vector<vector<float>>& data,size_t input_data_size,vector<int64_t> input_shape) {
    
    
//     for (int i = 0; i < data.size(); i++) {
//         for (int j = 0; j < data[0].size(); j++) {
//             // wcout<< data[i][j] << " ";
//             res.push_back(static_cast<float>((data[i][j])));
//         }
//     }
//     return Ort::Value::CreateTensor<float>(
//         this->memory_info,                     // 메모리 위치
//         res.data(),               // 데이터 포인터
//         res.size(),               // 데이터 총 크기 (바이트가 아닌 요소 개수)
//         input_shape.data(),              // 모양 포인터
//         input_shape.size()              // 모양 배열 크기
//     );
// }
