// #include "HTTokenizer.h"
// #include "utils.h"

// #include "onnxruntime_c_api.h"
// #include "onnxruntime_cxx_api.h"
// #include <regex>
// // Protobuf 메시지를 파일에서 읽어오는 함수

// bool ReadUserPreferencesFromFileTOK(const std::string& filename, TokDict* message) {
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

// vector<float> w2i(const wstring& wstr,const google::protobuf::Map<std::string,int64_t>& dict,int maxlen) {
//     vector<float> res;
//     res.reserve(maxlen);
//     res.push_back(trunc(float(dict.find("[SOS]")->second)));
//     for (int i = 0; i < wstr.length(); i++) {
//         // dict[str.substr(i, 1)];
//         wstring weoj = wstr.substr(i,1);
//         // cout << weoj << " ";
//         string eoj = ws2s(weoj);
//         auto it = dict.find(eoj);
        
//         if (eoj.compare(" ") == 0){
//             auto spaceit = dict.find("▁");
//             // cout << weoj << static_cast<float>(spaceit -> second) << endl;
//             res.push_back(static_cast<float>(spaceit->second));
//             // res.push_back
//         }
//         else if (it != dict.end()){
            
//             // cout << weoj << static_cast<float>(it -> second) << endl;
//             res.push_back(static_cast<float>(it->second));
//         }
//         else{
//             it = dict.find("[UNK]");
//             // cout << weoj << static_cast<float>(it -> second) << endl;
//             res.push_back(static_cast<float>((it->second)));
//         }
//         // tmp.push_back()
//         // cout << str.substr(i,1) << endl;
//     }
//     // cout << endl;
//     res.push_back(static_cast<float>(dict.find("[EOS]")->second));
//     if (int(res.size()) < maxlen){
//         int curlen = maxlen-int(res.size());
//         for(int j=0;j<curlen;j++){
//             // cout << j <<endl;
//             // cout << static_cast<float>(dict.find("[PAD]")->second) << endl;
//             // cout << dict.find("[PAD]")->second << endl;
//             res.push_back(static_cast<float>((dict.find("[PAD]")->second)));
//         }
//         // cout << maxlen-int(res.size()) << L" "<< res.size()<<endl;
        
//     }
//     // cout << wstr.size() << endl;
//     // cout << res.size() << endl;
//     return res;
// }

// int get_maxlen(const vector<string>& data,vector<wstring>& inputtext){
//     int max = -1;
//     for(int i=0;i<data.size();i++){
//         wstring wstr = s2ws(data[i]);
//         inputtext.push_back(wstr);
//         // cout << (max < int(data[i].size()) + 2) << endl;
//         if (max < int(wstr.length())+2){
//             // cout << L"1111" << endl;
//             max = int(wstr.length())+2;
//         }
//     }

//     // cout << max << endl;
//     return max;
// }


// HTTokenizer::HTTokenizer(const wchar_t* model_path,const string& dictpath) {
//     GOOGLE_PROTOBUF_VERIFY_VERSION;
//     const std::string filename = dictpath;//"httokenizer.bin";
//     TokDict prefs;
//     if (ReadUserPreferencesFromFileTOK(filename, &prefs)) {
//         std::cout << "loadded" << std::endl;
//     }
//     this->dict = prefs.dict();
    
//     this->env = std::make_unique<Ort::Env>(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, "Default");
    
//     this->session = this->init_session(model_path);
// }

// HTTokenizer::~HTTokenizer() {
//     google::protobuf::ShutdownProtobufLibrary();
// }
// vector<string> HTTokenizer::tokenizer(vector<string> data_){
//     // setlocale(LC_ALL,"");
//     vector<wstring> data;
//     int maxlen = get_maxlen(data_,data);
//     // cout << L"maxlen: " << maxlen << endl;
//     // cout << L"------------" << endl;
//     vector<vector<float>> input;
//     for (int i=0;i<data.size();i++){
//         vector<float> input_ = w2i(data[i],this->dict,maxlen);
//         // cout << input_.size() << endl;
//         input.push_back(input_);
//         // cout << input_.size() << endl;
//     }
//     vector<vector<int>> resprob;
//     try{
//         resprob = this->run(input);
//     }catch(const Ort::Exception& e){
//         cerr << e.what() << endl;
//         // exit(0);
//     }
//     // vector<vector<int>> 
//     // cout << L"HI" << endl;
//     vector<string> res = this->make_tok_res(resprob,data,maxlen);
//     // for(auto w:res){
//     //     cout << s2ws(w) << endl;
//     // }
//     return res;
// }
// vector<string> HTTokenizer::make_tok_res(vector<vector<int>> resprob,vector<wstring> text,int maxlen){
//     vector<string> res;
//     for(int i = 0;i<resprob.size();i++){
//         wstring httok;
//         wstring w2istr = text[i];

//         for (int j=1;j<resprob[i].size();j++){
            
//             if(j<= w2istr.size()){
//                 //cout << resprob[i][j] << endl;
//                 if(resprob[i][j]==1){
//                     httok += L" + "+w2istr.substr(j-1,1);
//                 }
//                 else{
//                     httok += w2istr.substr(j-1,1);
//                 }
//             }
//             if (w2istr.size() == j){
//                 std::wregex pattern(L" +");
//                 httok = std::regex_replace(httok, pattern, L" ");
//                 httok = strip(httok);
//                 // wstring restr = ws2s(httok);
//                 res.push_back(ws2s(httok));
//                 // break;
//             }
//         }
//     }
//     return res;
// }
    
// vector<vector<int>> HTTokenizer::run(vector<vector<float>> data) {
//     vector<int64_t> input_shape = { (int64_t)data.size(),(int64_t)data[0].size() };
//     size_t input_data_size = input_shape[0] * input_shape[1];
//     vector<float> restmp;
//     restmp.reserve(input_data_size);
//     // Ort::Value input_tensors = make_input(restmp,data,input_data_size,input_shape);
//     for (int i = 0; i < data.size(); i++) {
//         for (int j = 0; j < data[0].size(); j++) {
//             // wcout<< data[i][j] << " ";
//             restmp.push_back(static_cast<float>((data[i][j])));
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
// vector<vector<int>> HTTokenizer::get_prob(vector<float> output_tensors,vector<int64_t> input_shape,int mode){
//     int cnt = 0;
//     vector<vector<int>> res;
//     vector<int> restmp;
//     for (int64_t j = 0; j < input_shape[0] * input_shape[1]; j++) {

//         if (output_tensors[j] >= 0.5)
//             restmp.push_back(1);
//         else
//             restmp.push_back(0);
//         // }
//         cnt += 1;
//         if (cnt % input_shape[1] == 0) {
//             res.push_back(std::move(restmp));
//             restmp = vector<int>();
//         }
//     }
//     // }
//     return res;
// }

