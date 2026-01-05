// #include "HTPostagger.h"
// #include "onnxruntime_c_api.h"
// #include "onnxruntime_cxx_api.h"
// #include <algorithm>
// #include <execution> // C++17 필수
// // #include <mutex>
// // std::mutex tokenizer_mutex;

// bool ReadUserPreferencesFromFilePOS(const std::string& filename, PosDict* message) {
//     std::fstream input(filename, std::ios::in | std::ios::binary);
//     if (!input) {
//         std::cerr << "File " << filename << " not found." << std::endl;
//         return false;
//     }

//     // 파일에서 데이터를 파싱(역직렬화)합니다.
//     if (!message->ParseFromIstream(&input)) {
//         std::cerr << "Failed to parse UserPreferences from file." << std::endl;
//         return false;
//     }
//     return true;
// }



// HTPostagger::HTPostagger(const wchar_t* model_path,const string& dictpath,const string& tokenizerpath) {
//     GOOGLE_PROTOBUF_VERIFY_VERSION;
//     // cout << "hi" << endl;
//     const std::string filename = dictpath;
//     PosDict prefs;
//     if (ReadUserPreferencesFromFilePOS(filename, &prefs)) {
//         std::cerr << "loadded" << std::endl;
//     }
//     // cout << L"HI" << endl;
//     this->dict = prefs.i2p();
//     // cout << L"HI2" << endl;
    
//     this->env = std::make_unique<Ort::Env>(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, "Default");
//     // cout << L"HI3" << endl;
    
//     this->session = this->init_session(model_path);
    
//     // auto blob = LoadBytesFromFile(tokenizerpath);
//     // this->tokenizer = FromBlobJSON(blob);
    
//     // cout << L"END" << endl;
// }

// HTPostagger::~HTPostagger() {
//     google::protobuf::ShutdownProtobufLibrary();
// }


// // vector<vector<int>> HTPostagger::encode_batch(const unique_ptr<HFTokenizer>& tokenizer, const vector<string>& data){
// //     // std::lock_guard<std::mutex> lock(tokenizer_mutex);
// //     vector<vector<int32_t>> res = move(tokenizer->EncodeBatch(data,true));
// //     vector<vector<int>> padding_tok = padding_input(tokenizer,res);

// //     return padding_tok;
// // }

// vector<vector<float>> HTPostagger::get_tokens(HFTokenizer* hftok,const vector<string>& data){
//     // vector<float> res;
//     // cout << typeid(this->tokenizer->EncodeBatch(data,true)).name() << endl;
//     // exit(0);
//     // vector<int> res = this->encode_batch(data,);
//     vector<vector<int32_t>> res = move(hftok->EncodeBatch(data,true));
//     vector<vector<int>> padding_tok = padding_input(hftok,res);
//     // vector<vector<int>> padding_tok = this->encode_batch(tokenizer,data);

//     vector<vector<float>> res_tok;
//     for(int i = 0;i<padding_tok.size();i++){
//         vector<float> padding_tok_tmp;

//         for(int j=0;j<padding_tok[i].size();j++){
//             // cout << static_cast<float>(padding_tok[i][j]) << L" ";
//             padding_tok_tmp.push_back(move(float(padding_tok[i][j])));
//         }
//         // cout<<endl;
//         // exit(0);
//         res_tok.push_back(move(padding_tok_tmp));
//         padding_tok_tmp.clear();
//     }

//     return res_tok;
// }
// vector<Morphs> HTPostagger::postagger(HFTokenizer* hftok,const vector<string>& data_){
//     //vector<vector<int>> data;
//     vector<Morphs> resmorph;
//     vector<vector<float>> tokens = this->get_tokens(hftok,data_);
//     vector<vector<string>> resrun;
    
//     // if 
//     try{
//         resrun = this->run(tokens);
//     }catch(const Ort::Exception& e){
//         cerr << e.what() << endl;
//         // exit(0);
//     }
//     try{
//         for (int i=0;i<tokens.size();i++){
//             vector<int> tmp;
//             for (int j = 0; j < tokens[i].size(); j++) {
//                 // cout << tokens[i][j] <<L" ";
//                 tmp.push_back(int(tokens[i][j]));
//             }
            
//             vector<vector<string>> res = split_eoj(hftok,tmp,resrun[i],data_[i]);
//             Morphs morph_tmp;
//             for (int j=0;j<res.size();j++){
//                 // cout << res[j].size() << L";
//                 morph_tmp.toks.push_back(res[j][0]);
//                 morph_tmp.poss.push_back(res[j][1]);
//                 // cout << s2ws(res[j][0]) << L"/" << s2ws(res[j][1]) << L" ";
//             }
//             resmorph.push_back(morph_tmp);
//         }
//         // cout << endl;
//     }
//     catch(const exception& e){
//         cout << e.what() << endl;
//     }
//     return resmorph;
// }
    
// vector<vector<string>> HTPostagger::run(vector<vector<float>> data) {
    
//     vector<int64_t> input_shape = { (int64_t)data.size(),(int64_t)data[0].size() };
    
//     size_t input_data_size = input_shape[0] * input_shape[1];
//     // cout << input_data_size << endl;
//     vector<float> restmp;
//     restmp.reserve(input_data_size);
//     // Ort::Value input_tensors = make_input(restmp,data,input_data_size,input_shape);
//      for (int i = 0; i < data.size(); i++) {
//         for (int j = 0; j < data[0].size(); j++) {
//             // wcout<< data[i][j] << " ";
//             restmp.push_back(float((data[i][j])));
//         }
//     }
//     Ort::Value input_tensors = Ort::Value::CreateTensor<float>(
//         this->memory_info,                     // 메모리 위치
//         restmp.data(),               // 데이터 포인터
//         restmp.size(),               // 데이터 총 크기 (바이트가 아닌 요소 개수)
//         input_shape.data(),              // 모양 포인터
//         input_shape.size()              // 모양 배열 크기
//     );
    
//     vector<Ort::Value> output_tensors;
//     // output_tensors.reserve(input_shape[0] * input_shape[1] * this);
//     // cout << L"Hello" << endl;
//     // float* output_prob;
//     try {
//         output_tensors = (*this->session).Run(Ort::RunOptions(nullptr), this->input_node_names,&input_tensors, 1, this->output_node_names, 1);
    
//     } catch (const Ort::Exception& e) {
//         std::cerr << "ONNX Runtime error: " << e.what() << std::endl;
//         // exit(0);
//     }
//     float* output_prob = output_tensors[0].GetTensorMutableData<float>();
//     std::vector<float> safe_results;
//     // 출력 텐서의 크기 계산 (Batch Size가 커지면 이 값도 큼)
//     auto type_info = output_tensors[0].GetTensorTypeAndShapeInfo();
//     size_t output_count = type_info.GetElementCount();

//     // 3. [핵심] 내 메모리 공간(std::vector)으로 값 복사 (Deep Copy)
//     // assign을 쓰면 벡터 크기 자동 조정 및 데이터 복사가 일어남
//     safe_results.assign(output_prob, output_prob + output_count);
//     return get_prob(safe_results,input_shape,1);
// }
// vector<vector<string>> HTPostagger::get_prob(vector<float> output_tensors,vector<int64_t> input_shape,int mode){
//     // std::locale::global(std::locale("")); 
//     float* foutput_tensors = output_tensors.data();
//     int cnt = 0;
//     vector<vector<string>> res;

//     vector<string> restmp;
//     // float* tmp = output_tensors
//     // cout << input_shape[1] * input_shape[0] << endl;
//     try{
//         for (int64_t j = 0; j < input_shape[1] * input_shape[0]; j++) {
//             // int maxi2 = -1;
//             // float maxv2 = -1.;
//             // int idx2 = 0;
//             // for (int i = j * this->dict.size(); i < (j * this->dict.size()) + this->dict.size();i++){
//             //     if (maxv2 < output_tensors[i]){
//             //         maxv2 = output_tensors[i];
//             //         maxi2 = idx2;
//             //     }
//             //     idx2++;
//             // }
//             // cout << maxi2 << " ";
//             // // for (int i = j * this->dict.size(); i < (j * this->dict.size()) + this->dict.size();i++){
//             // // for (size_t i = 0; i < batch_size; ++i) {
//             //     // 현재 배치의 점수 데이터 시작 포인터
//             int dictlen = this->dict.size();
//             // const float* batch_start = output_tensors + (j * dictlen);//* num_classes);
//             // // 현재 배치의 점수 데이터 끝 포인터
//             // const float* batch_end = batch_start + dictlen;//num_classes;

//             // std::max_element를 사용하여 가장 큰 값의 반복자(iterator) 찾기
//             auto max_it = std::max_element(std::execution::par_unseq,foutput_tensors+(j * dictlen), foutput_tensors+(j * dictlen)+dictlen);
            
//             // 포인터 간의 거리를 계산하여 인덱스(ArgMax) 구하기
//             int maxi = std::distance(foutput_tensors+(j * dictlen), max_it);
//             // cout << maxi << " " << this->dict[maxi] << " ";
//             // cout << maxi % this->dict.size() << s2ws(this->dict[int(maxi % this->dict.size())]) << endl;
//             // predictions.push_back(max_index);
//             // }
//             // cout << maxi % this->dict.size()<< L"\"" << s2ws(this->dict[maxi % this->dict.size()]) << L"\"" << endl;
//             restmp.push_back(this->dict[maxi]);
//             // cout << s2ws(this->dict[maxi]) << L" " << endl;;
            
//             cnt++;
//             if (cnt % input_shape[1] == 0){
//                 res.push_back(move(restmp));
//                 restmp.clear();
//                 // cout << cnt <<endl;
//                 // cout << L"get prob" << cnt << endl;
//             }
//         }
//     }catch(const std::exception e){
//         cerr << e.what() << endl;
//         // exit(0);
//     }
    
//     // }
//     return res;
// }