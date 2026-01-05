// #include <Windows.h>
#include "HTPostagger2.h"
#include "onnxruntime_c_api.h"
#include "onnxruntime_cxx_api.h"
#include <iostream>
#include <memory>
#include <cstdlib>
// #include <mutex>
// std::mutex tokenizer_mutex;

bool ReadUserPreferencesFromFilePOS(const std::string& filename, PosDict* message) {
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
    return true;
}

class PostaggerManager {
private:
    // 실제 공유할 세션 객체
    std::shared_ptr<Ort::Session> session_ptr;
    std::shared_ptr<Ort::Env> env_ptr;

    // 생성자를 private로 만들어 외부에서 직접 객체를 생성할 수 없도록 합니다.
    PostaggerManager() {
        std::wcout << L"PostaggerManager: Initializing Session...\n";
        // 1. Ort::Env 초기화
        env_ptr = std::make_shared<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "GlobalSession");

        // 2. Ort::Session 초기화 (모델 경로는 실제 경로로 변경 필요)
        Ort::SessionOptions options;
        // ... 옵션 설정 ...
        // options.SetInterOpNumThreads();
        options.SetIntraOpNumThreads(4);
        options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
        try {
            // 이 시점에서 shared_ptr<Ort::Session>이 초기화됩니다.
            const char* env_var_name = "HTCPP"; // 얻고 싶은 환경변수 이름 (예: PATH)
            const char* env_value = std::getenv(env_var_name); // 환경변수 값 가져오기

            string htpath = env_value;
            // wstring htpath_w = s2ws(htpath+"/htmorphs/pos-model-uint8.onnx");
            // session_ptr = std::make_shared<Ort::Session>(
            //     *env_ptr, 
            //     htpath_w.c_str(), // 실제 모델 경로 사용
            //     options
            // );
#ifdef _WIN32	  
            wstring htpath_w = s2ws(htpath+"/htmorphs/pos-model-uint8.onnx");
            session_ptr = std::make_shared<Ort::Session>(
                *env_ptr, 
                htpath_w.c_str(), // 실제 모델 경로 사용
                options
            );
#else
	   string htpath_w = htpath+"/htmorphs/pos-model-uint8.onnx";
           session_ptr = std::make_shared<Ort::Session>(
                *env_ptr,
                htpath_w.c_str(), // 실제 모델 경로 사용
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
    PostaggerManager(const PostaggerManager&) = delete;
    PostaggerManager& operator=(const PostaggerManager&) = delete;

    // 객체에 접근하는 유일한 방법 (Magic Statics 활용)
    static PostaggerManager& get_instance() {
        // 이 줄이 실행될 때 단 한 번만 초기화가 보장됩니다 (스레드 안전).
        static PostaggerManager instance;
        return instance;
    }

    // 공유된 Ort::Session을 shared_ptr<T>& 형태로 안전하게 반환
    const std::shared_ptr<Ort::Session>& get_session() const {
        return session_ptr;
    }
};

// BaseORT basep;
// std::unique_ptr<Ort::Session> possession = basep.init_session(L"htmorphs/pos-model-uint8.onnx");

// Ort::Session pinit_session(wchar_t* model_path){
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

PosTaggerData::PosTaggerData(google::protobuf::Map<int64_t,std::string> dict){
    this->word_to_id_map_=dict;
}
const google::protobuf::Map<int64_t, std::string>& PosTaggerData::get_word_to_id_map() const{
    return this->word_to_id_map_;
}


// this->session = this->init_session(model_path);
// BaseORT base;
// shared_ptr<Ort::Session> possession;
// TokenizerManager tokman;
HTPostagger2::HTPostagger2(const HTPostagger2 &){

}
HTPostagger2::HTPostagger2(const wchar_t* model_path,const string& dictpath,const string& tokenizerpath,const shared_ptr<const PosTaggerData> dict) {
    // GOOGLE_PROTOBUF_VERIFY_VERSION;
    // cout << "hi" << endl;
    // const std::string filename = dictpath;
    // PosDict prefs;
    // if (ReadUserPreferencesFromFilePOS(filename, &prefs)) {
    //     std::cerr << "loadded" << std::endl;
    // }
    this->dict = dict;
    // cout << L"HI" << endl;
    // this->dict = prefs.i2p();//make_shared<PosTaggerData>(prefs.i2p());
    // cout << L"HI2" << endl;
    
    // this->env = std::make_shared<Ort::Env>(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, "Default");
    // if(!possession){
    //     possession = this->init_session(model_path);
    //     cout << "htpos 초기화" << endl;
    // }
    // cout << L"HI3" << endl;
    // this->session = session_;
    // this->session = this->init_session(model_path);
    
    // auto blob = LoadBytesFromFile(tokenizerpath);
    // this->tokenizer = FromBlobJSON(blob);
    
    // cout << L"END" << endl;
}

HTPostagger2::~HTPostagger2() {
    // google::protobuf::ShutdownProtobufLibrary();
}


// vector<vector<int>> HTPostagger2::encode_batch(const unique_ptr<HFTokenizer>& tokenizer, const vector<string>& data){
//     // std::lock_guard<std::mutex> lock(tokenizer_mutex);
//     vector<vector<int32_t>> res = move(tokenizer->EncodeBatch(data,true));
//     vector<vector<int>> padding_tok = padding_input(tokenizer,res);

//     return padding_tok;
// }

// vector<vector<float>> HTPostagger2::get_tokens(HFTokenizer* hftok,const vector<string>& data){
MatrixFloat HTPostagger2::get_tokens(HFTokenizer* hftok,const vector<string>& data){

    vector<vector<int32_t>> res = hftok->EncodeBatch(data,true);

    MatrixInt padding_tok = padding_input(hftok,res);
    // vector<vector<int>> padding_tok = this->encode_batch(tokenizer,data);

    // vector<vector<float>> res_tok;
    // res_tok.reserve(data.size());
    // wcout << L"dsaf" << res.size() << endl;
    MatrixFloat res_tok(padding_tok.rows,padding_tok.cols);
    // for(int i = 0;i<padding_tok.size();i++){
    // res_tok.data.insert(res_tok.data.end(),padding_tok.data.begin(),padding_tok.data.end());//padding_tok_tmp(padding_tok[i].begin(),padding_tok[i].end());
    // res_tok.push_back(padding_tok_tmp);
        // padding_tok_tmp.clear();
    // }
    for(int i=0;i<padding_tok.rows;i++){
        for(int j = 0;j<padding_tok.cols;j++){
            res_tok.at(i,j) = static_cast<float>(padding_tok.at(i,j));
        }
    }
    return res_tok;
}

MatrixMorph HTPostagger2::postagger2(HFTokenizer* hftok,const vector<string>& data_){
    //vector<vector<int>> data;
    // vector<Morphs> resmorph;
    // array<Morph,350>
    // resmorph.reserve(data_.size());
    // resmorph.reserve(data_.size());
    MatrixMorph resmorph(data_.size(),400);

    MatrixFloat tokens = this->get_tokens(hftok,data_);
    // vector<vector<string>> resrun;
    MatrixString resrun(tokens.rows,tokens.cols);
    // cout << tokens.size() << " " << data_.size()<<endl;
    // exit(0);
    // if 
    try{
        resrun = this->run(tokens);
    }catch(const Ort::Exception& e){
        wcerr << L"HTPostagger2::postagger run: ";
        wcerr << s2ws(e.what()) << endl;
        // exit(0);
    }
    
    // const int reslen = 4;
    // array<vector<Morph>,reslen> res;
    Morph initv;
    for (int i=0;i<tokens.rows;i++){
        vector<int> tmp;
        // tmp.clear();
        vector<string> resrun_str;
        //cout << "------------------" << endl;
        for (int j = 0; j < tokens.cols; j++) {
            // cout << tokens[i][j] <<L" ";
            tmp.push_back(static_cast<int>(tokens.at(i,j)));
            // wcout << s2ws(resrun.at(i,j)) << L" ";
            // wcout << s2ws(resrun.at(i,j)) << endl;
            resrun_str.push_back(resrun.at(i,j));
        }
        // wcout << endl;
        // wcout << L"end " << tmp.size() << L" " << resrun_str.size() << endl;
        
        // tmp.insert(tmp.begin(),tokens.data.begin()+(i*tokens.rows),tokens.data.end()+((i*tokens.rows)+tokens.cols));
        // resrun_str.insert(resrun_str.begin(),resrun.data.begin()+(i*resrun.rows),resrun.data.end()+((i*tokens.rows)+resrun.cols));
        // cout << tmp
        // cout << "111111111111" << endl;
        /*vector<array<string,2>> res;
        Morphs morph_tmp = {};*/
        vector<Morph> res;
        
        // cout << "222222222222" << endl;
        try{
            // wcout << s2ws(data_[i]) <<endl;
            res = split_eoj_struct(hftok,tmp,resrun_str,data_[i]);
            // wcout << res.size() << endl;
            // for (int v=0;v<res.size();v++){
                // wcout<<L"++++" << res[v].isdata << L" " << res[v].istail << s2ws(res[v].head_tk) << endl;
                // if(res[v].istail){
                    // wcout << s2ws(res[v].tail_tk) << L" " << s2ws(res[v].tail_pos) << endl;
                // }
            // }
            // for (int j=0;j<res.poss.size();j++){
            //     wcout << s2ws(res.toks[j]) + L"+" + s2ws(res.poss[j]) << L" ";
            // }
            // wcout << endl;
        } catch(const exception& e){      
            wcerr << L"HTPostagger2::postagger split_eoj: " << s2ws(data_[i]) << endl;
            string err;
            for(auto r:resrun_str){
                // wcerr<< s2ws(r) << " ";
                err += r + " ";
            }

            int tmpidx = 0;
            for(int v: tmp){
                wstring tk = s2ws(hftok->IdToToken(v));
                wstring firsteum = tk.substr(0,1);
                if (firsteum.compare(L"▁") == 0){
                    tk = L"_" + tk.substr(1,tk.size());
                }
                if (tk.compare(L"[SEP]") == 0){
                    break;
                }
                wcout << tk + L"/" + s2ws(resrun_str[tmpidx]) << L" ";
                tmpidx++;
            }
            wcout << endl;
            // wcerr << L" " << endl;
            wcerr << s2ws(e.what()) << endl;
            Morph restmp;
            restmp.isdata=true;
            restmp.head_tk = "ERROR:"+data_[i];
            restmp.head_pos = err;
            res.push_back(restmp);
            // wcout << res.size() << endl;
            // res.toks.push_back("ERROR:"+data_[i]);
            // res.poss.push_back(err);
        }
        /*morph_tmp.poss.reserve(res.size());
        morph_tmp.toks.reserve(res.size());*/
        //for (int j=0;j<2;j++){
        //    // if (res[j].empty()) continue;
        //    // cout << res[j].size() << L";
        //    // cout << typeid(res[j][0]).name() << "/" << typeid(res[j][1]).name() << " ";
        //    // morph_tmp.toks.push_back(string(res[j][0]));
        //    // morph_tmp.poss.push_back(string(res[j][1]));
        //    // if (res[j].size() > 0) {
        //    // cout << "[" << res[j][0] << " " << res[j][1]  << "]"<< endl;
        //    // cout << res[j].size() << endl;
        //
        //    morph_tmp.toks.push_back(res[j][0]); // 굳이 string()으로 감쌀 필요 없음
        //    // } else {
        //    //     morph_tmp.toks.push_back(""); 
        //    // }

        //    // 2. 품사 넣기 (2번째 요소가 있는지 확인 필수!)
        //    // if (res[j].size() > 1) {
        //    morph_tmp.poss.push_back(res[j][1]); 
        //    // } else {
        //    //     // 품사 정보가 없으면 "Unknown"이나 빈 문자열 채우기
        //    //     morph_tmp.poss.push_back(""); 
        //    // }
        //    // cout << 
        //    // cout << "[" << res[j][0] << " " << res[j][1]  << "]"<< endl;
        //    
        //}
        // cout << endl;
        // cout << morph_tmp.toks.size() << " " << morph_tmp.poss.size() << endl;
        // wcout << res.size() <<endl;
        for(int j=0;j<res.size();j++){

            // wcout << s2ws(res[j].head_tk+"/"+res[j].head_pos) << endl;
            // if (res[j].istail){
            //     wcout << L"ffff" << s2ws(res[j].tail_tk+"/"+res[j].tail_pos) << endl;
            // }
            resmorph.at(i,j) = res[j];
            // wcout << L"end" << endl;
        }
        // wcout << L"end" << endl;
        // resmorph.push_back(res);
    }
        // exit(0);
        // cout << endl;
    

    // for (auto m : resmorph){
    //     for(int i=0;i<m.toks.size();i++){
    //         cout << m.toks[i] << "/" << m.poss[i] << " ";
    //     }
    //     cout << endl;
    // }
    return resmorph;
}


vector<Morphs> HTPostagger2::postagger(HFTokenizer* hftok,const vector<string>& data_){
    //vector<vector<int>> data;
    vector<Morphs> resmorph;
    // array<Morph,350>
    // resmorph.reserve(data_.size());
    // resmorph.reserve(data_.size());
    MatrixFloat tokens = this->get_tokens(hftok,data_);
    // vector<vector<string>> resrun;
    MatrixString resrun(tokens.rows,tokens.cols);
    // cout << tokens.size() << " " << data_.size()<<endl;
    // exit(0);
    // if 
    try{
        resrun = this->run(tokens);
    }catch(const Ort::Exception& e){
        wcerr << L"HTPostagger2::postagger run: ";
        wcerr << s2ws(e.what()) << endl;
        // exit(0);
    }
    
    
    for (int i=0;i<tokens.rows;i++){
        vector<int> tmp;
        // tmp.clear();
        vector<string> resrun_str;
        //cout << "------------------" << endl;
        for (int j = 0; j < tokens.cols; j++) {
            // cout << tokens[i][j] <<L" ";
            tmp.push_back(static_cast<int>(tokens.at(i,j)));
            // wcout << s2ws(resrun.at(i,j)) << L" ";
            resrun_str.push_back(resrun.at(i,j));
        }
        // wcout << endl;
        // wcout << L"end " << tmp.size() << L" " << resrun_str.size() << endl;
        
        // tmp.insert(tmp.begin(),tokens.data.begin()+(i*tokens.rows),tokens.data.end()+((i*tokens.rows)+tokens.cols));
        // resrun_str.insert(resrun_str.begin(),resrun.data.begin()+(i*resrun.rows),resrun.data.end()+((i*tokens.rows)+resrun.cols));
        // cout << tmp
        // cout << "111111111111" << endl;
        /*vector<array<string,2>> res;
        Morphs morph_tmp = {};*/
        Morphs res;
        // cout << "222222222222" << endl;
        try{
            res = split_eoj(hftok,tmp,resrun_str,data_[i]);
            // for (int j=0;j<res.poss.size();j++){
            //     wcout << s2ws(res.toks[j]) + L"+" + s2ws(res.poss[j]) << L" ";
            // }
            // wcout << endl;
        } catch(const exception& e){      
            wcerr << L"HTPostagger2::postagger split_eoj: " << s2ws(data_[i]) << endl;
            string err;
            for(auto r:resrun_str){
                // wcerr<< s2ws(r) << " ";
                err += r + " ";
            }

            int tmpidx = 0;
            for(int v: tmp){
                wstring tk = s2ws(hftok->IdToToken(v));
                wstring firsteum = tk.substr(0,1);
                if (firsteum.compare(L"▁") == 0){
                    tk = L"_" + tk.substr(1,tk.size());
                }
                if (tk.compare(L"[SEP]") == 0){
                    break;
                }
                wcout << tk + L"/" + s2ws(resrun_str[tmpidx]) << L" ";
                tmpidx++;
            }
            wcout << endl;
            // wcerr << L" " << endl;
            wcerr << s2ws(e.what()) << endl;
            res.toks.push_back("ERROR:"+data_[i]);
            res.poss.push_back(err);
        }
        /*morph_tmp.poss.reserve(res.size());
        morph_tmp.toks.reserve(res.size());*/
        //for (int j=0;j<2;j++){
        //    // if (res[j].empty()) continue;
        //    // cout << res[j].size() << L";
        //    // cout << typeid(res[j][0]).name() << "/" << typeid(res[j][1]).name() << " ";
        //    // morph_tmp.toks.push_back(string(res[j][0]));
        //    // morph_tmp.poss.push_back(string(res[j][1]));
        //    // if (res[j].size() > 0) {
        //    // cout << "[" << res[j][0] << " " << res[j][1]  << "]"<< endl;
        //    // cout << res[j].size() << endl;
        //
        //    morph_tmp.toks.push_back(res[j][0]); // 굳이 string()으로 감쌀 필요 없음
        //    // } else {
        //    //     morph_tmp.toks.push_back(""); 
        //    // }

        //    // 2. 품사 넣기 (2번째 요소가 있는지 확인 필수!)
        //    // if (res[j].size() > 1) {
        //    morph_tmp.poss.push_back(res[j][1]); 
        //    // } else {
        //    //     // 품사 정보가 없으면 "Unknown"이나 빈 문자열 채우기
        //    //     morph_tmp.poss.push_back(""); 
        //    // }
        //    // cout << 
        //    // cout << "[" << res[j][0] << " " << res[j][1]  << "]"<< endl;
        //    
        //}
        // cout << endl;
        // cout << morph_tmp.toks.size() << " " << morph_tmp.poss.size() << endl;
        resmorph.push_back(res);
    }
        // exit(0);
        // cout << endl;
    

    // for (auto m : resmorph){
    //     for(int i=0;i<m.toks.size();i++){
    //         cout << m.toks[i] << "/" << m.poss[i] << " ";
    //     }
    //     cout << endl;
    // }
    return resmorph;
}
    
MatrixString HTPostagger2::run(const MatrixFloat& data) {
    
    vector<int64_t> input_shape = { (int64_t)data.rows,(int64_t)data.cols };
    
    size_t input_data_size = input_shape[0] * input_shape[1];
    // cout << input_data_size << endl;
    vector<float> restmp;
    restmp.reserve(input_data_size);
    // cout << input_data_size << " " << "Matfloat size:" << data.data.size() << endl;
    // restmp.insert(restmp.end(),data.data.begin(),data.data.end());
    for (auto d:data.data){
        restmp.push_back(d);
    }
    // Ort::Value input_tensors = make_input(restmp,data,input_data_size,input_shape);
    //  for (int i = 0; i < data.size(); i++) {
    //     for (int j = 0; j < data[0].size(); j++) {
    //         // wcout<< data[i][j] << " ";
    //         restmp.push_back(float((data[i][j])));
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
    // output_tensors.reserve(input_shape[0] * input_shape[1] * this);
    // cout << L"Hello" << endl;
    // float* output_prob;
    try {
        const auto& session_ = PostaggerManager::get_instance().get_session();
        output_tensors = session_->Run(Ort::RunOptions(nullptr), this->input_node_names,&input_tensors, 1, this->output_node_names, 1);
    
    } catch (const Ort::Exception& e) {
        wcerr << L"POSTAGGER" << endl;
        std::wcerr << L"ONNX Runtime error: " << s2ws(e.what()) << std::endl;
        // exit(0);
    }
    int64_t* output_prob = output_tensors[0].GetTensorMutableData<int64_t>();
    std::vector<int64_t> safe_results;
    // 출력 텐서의 크기 계산 (Batch Size가 커지면 이 값도 큼)
    auto type_info = output_tensors[0].GetTensorTypeAndShapeInfo();
    size_t output_count = type_info.GetElementCount();

    // 3. [핵심] 내 메모리 공간(std::vector)으로 값 복사 (Deep Copy)
    // assign을 쓰면 벡터 크기 자동 조정 및 데이터 복사가 일어남
    safe_results.assign(output_prob, output_prob + output_count);
    MatrixString resrun(data.rows,data.cols);
    vector<string> tmp;
    // const auto& dict_ = this->dict//->get_word_to_id_map();
    int rowindex = 0;
    int colindex = 0;
    for(int i = 0; i<input_shape[0]*input_shape[1];i++){
        // cout << s << endl;
        int64_t s = safe_results[i];
        int64_t value = s;
        const auto& dict_ = this->dict->get_word_to_id_map();
        string tag = dict_.find(value)->second;
        // tmp.push_back(tag);
        resrun.at(rowindex,colindex) = tag;
        colindex++;
        if(colindex == input_shape[1]){
            colindex = 0;
            rowindex++;
        }
    }
    return resrun;//get_prob(safe_results,input_shape,1);
}

// int main(){
//     SetConsoleOutputCP(65001);
//     HTPostagger2 pos(L"htmorphs/pos-model-uint8.onnx","htmorphs/htpostagger.bin","htmorphs/tokenizer/tokenizer.json");
//     auto blob = LoadBytesFromFile("htmorphs/tokenizer/tokenizer.json");
//     unique_ptr<HFTokenizer> tokenizer = FromBlobJSON(blob);

//     vector<string> data;
//     data.push_back("나 + 는 밥 + 을 먹 + 고 학교 + 에 갔 + 다");
//     data.push_back("집 + 에 가 + 고 싶 + 어요");
//     data.push_back("배불러 터지 + 겠다");
//     vector<Morphs> res = pos.postagger(tokenizer.get(), data);
//     for(auto r: res){
//         // cout << res.size() << " " << r.poss.size() << " " << r.toks.size() << endl;
//         for(size_t i=0;i<r.poss.size();i++){
//             cout << r.toks[i] << "/" << r.poss[i] << " ";
//         }
//         cout << endl;
//     }
//     return 0;
// }