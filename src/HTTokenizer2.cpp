// #include <Windows.h>
#include "HTTokenizer2.h"
#include <cstdlib>
// Protobuf 메시지를 파일에서 읽어오는 함수

bool ReadUserPreferencesFromFileTOK(const std::string& filename, TokDict* message) {
    std::fstream input(filename, std::ios::in | std::ios::binary);
    if (!input) {
        std::wcerr << L"File " << s2ws(filename) << L" not found." << std::endl;
        return false;
    }

    // 파일에서 데이터를 파싱(역직렬화)합니다.
    if (!message->ParseFromIstream(&input)) {
        std::wcerr << L"Failed to parse UserPreferences from file." << std::endl;
        return false;
    }

    //  make_shared<google::protobuf::Map<std::string,int64_t>> message->dict()>;
    // TokenizerData tokdata(message->dict()) 
    return true;
}

vector<float> w2i(const wstring& wstr,const shared_ptr<const TokenizerData>& dict_,int maxlen) {
    vector<float> res;
    // res.reserve(maxlen);
    // if (!dict_) { 
    //     cout << "httok2 null" << endl;
    //     exit(0);
    //     // NULL 포인터 접근 방지
    //     // return; // 또는 예외 처리
    // }
    // res.push_back(trunc(float(dict.find("[SOS]")->second)));
    // cout << "test" << endl;
    // exit(0);
    const auto& dict = dict_->get_word_to_id_map();
    // cout  << "dsaf"<< dict.size() << endl;
    // exit(0);
    // for(const auto& a:dict){
    //     cout << a.first << " " << a.second << endl;
    // }
    // exit(0);
    for (int i = 0; i < wstr.length(); i++) {
        // dict[str.substr(i, 1)];
        wstring weoj = wstr.substr(i,1);
        // cout << weoj << " ";
        string eoj = ws2s(weoj);
        auto it = dict.find(eoj);
        
        if (eoj.compare(" ") == 0){
            auto spaceit = dict.find("▁");
            // cout << eoj << static_cast<float>(spaceit -> second) << endl;
            res.push_back(static_cast<float>(spaceit->second));
            // res.push_back
        }
        else if (it != dict.end()){
            
            // cout << eoj << static_cast<float>(it -> second) << endl;
            res.push_back(static_cast<float>(it->second));
        }
        else{
            it = dict.find("[UNK]");
            // cout << eoj << static_cast<float>(it -> second) << endl;
            res.push_back(static_cast<float>((it->second)));
        }
        // tmp.push_back()
        // cout << str.substr(i,1) << endl;
    }
    // cout << endl;
    // res.push_back(static_cast<float>(dict.find("[EOS]")->second));
    if (int(res.size()) < maxlen){
        int curlen = maxlen-int(res.size());
        for(int j=0;j<curlen;j++){
            // cout << j <<endl;
            // cout << static_cast<float>(dict.find("[PAD]")->second) << endl;
            // cout << dict.find("[PAD]")->second << endl;
            res.push_back(static_cast<float>((dict.find("[PAD]")->second)));
        }
        // cout << maxlen-int(res.size()) << L" "<< res.size()<<endl;
        
    }
    // cout << wstr.size() << endl;
    // cout << res.size() << endl;
    return res;
}

int get_maxlen(const vector<string>& data,vector<wstring>& inputtext){
    int max = -1;
    for(int i=0;i<data.size();i++){
        wstring wstr = s2ws(data[i]);
        inputtext.push_back(wstr);
        // cout << (max < int(data[i].size()) + 2) << endl;
        if (max < int(wstr.length())){
            // cout << L"1111" << endl;
            max = int(wstr.length());
        }
    }

    // cout << max << endl;
    return max;
}

// Ort::Session tinit_session(wchar_t* model_path){
//     Ort::SessionOptions session_options;
//     session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
//     session_options.SetInterOpNumThreads(4);
//     session_options.SetIntraOpNumThreads(2);
//     // session_options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);
//     // this->input_node_names[0] = "input";
//     // this->output_node_names[0] = "output"
//     session_options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
//     // wstring model_path = s2ws(model_path_);
//     //this->model_path = model_path.c_str();
//     Ort::Env env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, "Default");
//     Ort::Session possenssion(env, model_path, session_options);
//     return possenssion;
// }

// Ort::Session tksession = tinit_session(L"htmorphs/tok-model-uint8.onnx");
// tksession
// BaseORT baset;
// std::shared_ptr<Ort::Session> tksession;// = baset.init_session(L"htmorphs/tok-model-uint8.onnx");
class TokenizerManager {
private:
    // 실제 공유할 세션 객체
    std::shared_ptr<Ort::Session> session_ptr;
    std::shared_ptr<Ort::Env> env_ptr;

    // 생성자를 private로 만들어 외부에서 직접 객체를 생성할 수 없도록 합니다.
    TokenizerManager() {
        std::wcout << L"TokenizerManager: Initializing Session...\n";
        // 1. Ort::Env 초기화
        env_ptr = std::make_shared<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "GlobalSession");

        // 2. Ort::Session 초기화 (모델 경로는 실제 경로로 변경 필요)
        Ort::SessionOptions options;
        // ... 옵션 설정 ...
        // options.SetInterOpNumThreads(1);
        options.SetIntraOpNumThreads(4);
        options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
        
        try {
            const char* env_var_name = "HTCPP"; // 얻고 싶은 환경변수 이름 (예: PATH)
            const char* env_value = std::getenv(env_var_name); // 환경변수 값 가져오기

            string htpath = env_value;
#ifdef _WIN32
            wstring htpath_w = s2ws(htpath+"/htmorphs/tok-model-uint8.onnx");
	    // 이 시점에서 shared_ptr<Ort::Session>이 초기화됩니다.
            session_ptr = std::make_shared<Ort::Session>(
                *env_ptr,
                htpath_w.c_str(), //  실제 모델 경로 사용
                options
            );
#else
	    string htpath_w = htpath+"/htmorphs/tok-model-uint8.onnx";
            // 이 시점에서 shared_ptr<Ort::Session>이 초기화됩니다.
            session_ptr = std::make_shared<Ort::Session>(
                *env_ptr, 
                htpath_w.c_str(), //  실제 모델 경로 사용
                options
            );
#endif	   
        } catch (const Ort::Exception& e) {
            std::wcerr << L"ONNX Session initialization failed: " << s2ws(e.what()) << std::endl;
            session_ptr = nullptr; // 초기화 실패 시 nullptr 명확히 지정
        }
    }

public:
    // 복사 및 이동 금지 (싱글톤 보장)
    TokenizerManager(const TokenizerManager&) = delete;
    TokenizerManager& operator=(const TokenizerManager&) = delete;

    // 객체에 접근하는 유일한 방법 (Magic Statics 활용)
    static TokenizerManager& get_instance() {
        // 이 줄이 실행될 때 단 한 번만 초기화가 보장됩니다 (스레드 안전).
        static TokenizerManager instance;
        return instance;
    }

    // 공유된 Ort::Session을 shared_ptr<T>& 형태로 안전하게 반환
    const std::shared_ptr<Ort::Session>& get_session() const {
        return session_ptr;
    }
};

// static TokenizerManager tokman;
TokenizerData::TokenizerData(google::protobuf::Map<std::string,int64_t> dict){
    this->word_to_id_map_ = dict;
}
    // 이 맵은 초기화 후에는 변경되지 않는다고 가정합니다.
const google::protobuf::Map<std::string, int64_t>& TokenizerData::get_word_to_id_map() const {
    return this->word_to_id_map_; // 읽기 전용으로 제공
}

HTTokenizer2::HTTokenizer2(const wchar_t* model_path,const string& dictpath,const shared_ptr<const TokenizerData>& dict) {
    // GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    this->dict = dict;

    // this->dict = make_shared<TokenizerData>(prefs.dict());
    // this->session = this->init_session(model_path);
    // if(!tksession){
    //     tksession = this->init_session(model_path);
    //     cout << "httk 초기화" << endl;
    // }
}

HTTokenizer2::HTTokenizer2(const HTTokenizer2&) {

    // this = HTTokenizer2;
}

HTTokenizer2::~HTTokenizer2() {
    google::protobuf::ShutdownProtobufLibrary();
}

vector<string> HTTokenizer2::tokenizer(vector<string> data_){
    vector<wstring> data;
    int maxlen = get_maxlen(data_,data);

    // vector<vector<float>> input;
    MatrixFloat input(data_.size(),maxlen);
    // input.reserve(data.size());
    for (int i=0;i<data.size();i++){
        vector<float> input_ = w2i(data[i],this->dict,maxlen);
        // input.push_back(input_);
        for(int j=0;j<maxlen;j++){
            input.at(i,j) = input_[j];
        }
    }
    // exit(0);
    vector<int32_t> resprob;
    try{
        resprob = this->run(input);
    }catch(const Ort::Exception& e){
        wcerr << s2ws(e.what()) << endl;
        // exit(0);
    }
    vector<string> res = this->make_tok_res(resprob,data,maxlen);

    return res;
}
vector<string> HTTokenizer2::make_tok_res(vector<int32_t> resprob,vector<wstring> text,int maxlen){
    vector<string> res;
    int strlen = 0;
    int num_string = text.size();
    int text_idx = 0;
    wstring httok;
    wstring w2istr = text[text_idx];
    for(int i = 0;i<resprob.size();i++){
        
        if (strlen <= w2istr.size()){
            if(resprob[i]==1){
                
                httok += L" + "+w2istr.substr(strlen,1);
            }
            else{
                httok += w2istr.substr(strlen,1);
            }
        }
        if ((i+1) % maxlen == 0 || (i+1) == resprob.size()){
            std::wregex pattern(L" +");
            httok = std::regex_replace(httok, pattern, L" ");
            httok = strip(httok);
            string httk_str = ws2s(httok);
            res.push_back(httk_str);
            strlen = -1;
            text_idx++;
            if (text_idx > text.size()-1)
                break;
            w2istr = text[text_idx];
            httok = L"";
        }
        strlen++;
    }

    return res;
}
    
vector<int32_t> HTTokenizer2::run(const MatrixFloat& data) {
    // cout <<"tokenizer::run"<< data.size() << endl;
    // data.cols
    vector<int64_t> input_shape = { (int64_t)data.rows,(int64_t)data.cols };
    size_t input_data_size = input_shape[0] * input_shape[1];
    
    vector<float> restmp;
    restmp.reserve(input_data_size);
    // restmp.insert(restmp.end(),data.data.begin(),data.data.end());
    for(auto d:data.data){
        restmp.push_back(d);
    }
    // restmp.reserve(input_data_size);
    // Ort::Value input_tensors = make_input(restmp,data,input_data_size,input_shape);
    // for (int i = 0; i < data.size(); i++) {
    //     for (int j = 0; j < data[0].size(); j++) {
    //         // wcout<< data[i][j] << " ";
    //         restmp.push_back(static_cast<float>((data[i][j])));
    //     }
    // }
    
    Ort::Value input_tensors = Ort::Value::CreateTensor<float>(
        this->memory_info,                     // 메모리 위치
        restmp.data(),               // 데이터 포인터
        restmp.size(),               // 데이터 총 크기 (바이트가 아닌 요소 개수)
        input_shape.data(),              // 모양 포인터
        input_shape.size()              // 모양 배열 크기
    );
    vector<Ort::Value> output_tensors;
    try {
        const auto& session_ = TokenizerManager::get_instance().get_session();
        output_tensors = session_->Run(Ort::RunOptions(nullptr), this->input_node_names,&input_tensors, 1, this->output_node_names, 1);

    } catch (const Ort::Exception& e) {
        std::wcerr << L"ONNX Runtime error: " << s2ws(e.what()) << std::endl;
        // exit(0);
    }

    int32_t* output_prob = output_tensors[0].GetTensorMutableData<int32_t>();

    std::vector<int32_t> safe_results;
    // 출력 텐서의 크기 계산 (Batch Size가 커지면 이 값도 큼)
    auto type_info = output_tensors[0].GetTensorTypeAndShapeInfo();
    size_t output_count = type_info.GetElementCount();
    // for(int i=0;i<output_count;i++){
    //     cout << static_cast<int>(output_prob[i]) << endl;
    // }
    // 3. [핵심] 내 메모리 공간(std::vector)으로 값 복사 (Deep Copy)
    // assign을 쓰면 벡터 크기 자동 조정 및 데이터 복사가 일어남
    safe_results.assign(output_prob, output_prob + output_count);
    return safe_results;//get_prob(safe_results,input_shape,1);
}

// int main(){
//     SetConsoleOutputCP(65001);
//     HTTokenizer2 tok(L"htmorphs/tok-model-uint8.onnx","htmorphs/httokenizer.bin");
//     vector<string> data;
//     data.push_back("나는 밥을 먹고 학교에 갔다");
//     data.push_back("통합보건교육은 이 대학만의 특화된 프로그램이다");
//     data.push_back("나는 밥을 먹고 학교에 갔다");
//     auto res = tok.tokenizer(data);
//     for (auto r: res){
//         cout <<"[" << r << "]"<< endl;
//     }
//     return 0;
// }