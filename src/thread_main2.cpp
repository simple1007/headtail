// cppthreadtest.cpp : 애플리케이션의 진입점을 정의합니다.
//
// #include <Windows.h>
#include "thread_main2.h"
using namespace std;
// namespace fs = std::filesystem;
// 환경 변수 설정 함수
void SetEnv(const std::string& name, const std::string& value) {
#ifdef _WIN32
    // Windows (Visual Studio)
    _putenv_s(name.c_str(), value.c_str());
#else
    // Linux / macOS
    // 세 번째 인자 1: 이미 있으면 덮어쓰기(Overwrite)
    setenv(name.c_str(), value.c_str(), 1);
#endif
}


vector<string> pos_(const shared_ptr<HTTokenizer2>& tok,const shared_ptr<HTPostagger2>& pos,const MatrixString& data_,HFTokenizer* hftoks) {
    vector<string> resultht;
    MatrixString data = data_;
    if (data.get_rows()>0){
        // for(vector<string> dv:data){
        for (int i = 0;i < data.get_rows();i++){
            vector<string> dv = data.at(i);
            if (i == data.get_rows()-1){
                vector<string> dv_;
                for(string dvv: dv){
                    if(dvv.compare("NULL") != 0){
                        // wcout << s2ws(dvv) << endl;
                        dv_.push_back(dvv);
                    }
                }
            //     // wcout << L"+++++++++++++++++++" << endl;
                dv = dv_;
            //     // wcout << dv.size() << endl;
            }
            // for(string ss: dv){
            //     wcout << s2ws(ss) << endl;
            // }
            // wcout << L"--------------" << endl;
            try{
                // wcout << dv[i] << endl;
                vector<string> toks = tok->tokenizer(dv);

                // for(string ss:dv){
                //     wcout << dv.size() << L" " << s2ws(ss) << endl;
                // }
                // wcout << L"------------------" << endl;
                // for(string tk:toks){
                    // wcout << toks.size() << endl;
                // }
                // wcout << L"================" <<endl;
                MatrixMorph poss = pos->postagger2(hftoks,toks);
                // wcout << dv.size() << L" " << poss.get_rows() << endl;
                // for(Morphs m:poss){
                for(int j=0;j<poss.get_rows();j++){
                //    wcout << s2ws(dv[j]) << endl;
                    
                //    wcout << L"end" << endl;
                    vector<Morph> m = poss.at(j);
                    
                    // bool prev_htsep = false;
                    vector<wstring> tmp;
                    for(int k =0;k<m.size();k++){
                        Morph morph = m[k];
                        // wcout << morph.isdata << L" " << morph.istail << endl;   
                        if (!morph.isdata){
                            break;
                        }
                        tmp.push_back(s2ws(morph.head_tk+"/"+morph.head_pos)+L" ");
                        if (morph.istail){
                            tmp.push_back(L"+/+ ");
                            tmp.push_back(s2ws(morph.tail_tk+"/"+morph.tail_pos)+L" ");
                        }
                    //    tmp.push_back(s2ws(m.toks[i]+"/"+m.poss[i])+L" ");
                    }
                    
                    wstring res_tk_pos = join(tmp);
                    // cout << ws2s(res_tk_pos) << endl;
                //    wcout << res_tk_pos << endl;
                    std::wregex re(L"\\s\\+/\\+\\s"); 

                    // 2. 치환 수행
                    // 원본 문자열, 정규식 객체, 바꿀 문자열
                    res_tk_pos = std::regex_replace(res_tk_pos, re, L"+");
                //    wcout << res_tk_pos << endl;
                //    wcout << L"endl" << endl;
                    resultht.push_back(ws2s(strip(res_tk_pos)));
                    
                }
            }catch(const std::exception& e){
                
                wcerr << s2ws(e.what()) << endl;
                exit(0);
            }
                        
        }
    }
    return resultht;
    // this->is_running = false;
}


string replace(const string& text){
    wstring text_ = strip(s2ws(text));

    std::wregex re(L"([^a-zA-Z\\s\\d가-힣]+)"); 

    // 2. 치환 수행
    // 원본 문자열, 정규식 객체, 바꿀 문자열
    std::wstring result = std::regex_replace(text_, re, L" ");
    
    std::wregex num_eng(L"([\\da-zA-Z]+)");
    result = std::regex_replace(result,num_eng,L" $1 ");
    
    
    std::wregex spacere(L" +"); 
    result = std::regex_replace(result, spacere, L" ");
    result = strip(result);

    if(result.length() > 398){
        result = result.substr(0,398);
    }
    if(strip(result).compare(L" ")==0){
        result = L"NoChar";
    }
    string res = ws2s(strip(result));
    return res;
}
MatrixString normalize(vector<string> data,int inferbs) {
    // if (data.size() < 24){
    //     MatrixString input(data.size(),1);
    //     for(int i=0;i<data.size();i++){
    //         input.at(i,0) = data[i];
    //     }
    // }
    if (data.size() < 24){
        MatrixString matrix(1,data.size());
        for(int i=0;i<data.size();i++){
            string d = replace(data[i]);
            matrix.at(0,i) = d;
        }
        return matrix;
    }
    vector<string> tmp;
    // vector<vector<string>> res;
    size_t rows = ceil(static_cast<double>(data.size()) / static_cast<double>(inferbs));
    MatrixString matrix(rows,inferbs);
    int idx = 0;
    for (string d : data) {
        string tmpd = d;
        // idx++;
        d = replace(d);
        tmp.push_back(d);
        if (tmp.size() == inferbs) {
            // res.push_back(tmp);
            // tmp.clear();
            for(int i = 0; i < inferbs; i++){
                matrix.at(idx,i) = tmp[i];
            }
            // cout << matrix.data.size() << endl;
            tmp.clear();
            idx++;
        }
        // wcout << tmp.size() << endl;
        
    }
    if (tmp.size() > 0) {
        // res.push_back(tmp);
        // tmp.clear();
        // cout << tmp.size() << endl;
        for(int i = 0; i < tmp.size();i++){
            matrix.at(idx,i) = tmp[i];
        }
        for(int i = tmp.size(); i < inferbs; i++){
            matrix.at(idx,i) = "NULL";
        }
        // cout << matrix.data.size() << endl;
        // wcout << tmp.size() << endl;
        tmp.clear();
    }
    return matrix;
}
vector<string> make_mp(int num,vector<string> data,vector<shared_ptr<HTTokenizer2>> tok, vector<shared_ptr<HTPostagger2>> pos,const vector<unique_ptr<HFTokenizer>>& hftoks) {
    vector<string> res_merge;

    
    // wcout << L"fffff1" << endl;
    // if (data.size() < 24){
    MatrixString input = normalize(data,4);
    // }else{
    //     MatrixString input = Matri(data,1)
    // }
    // wcout << L"normalize" << endl;
    int cluster_input_size = static_cast<int>(ceil(input.get_rows() / num));
    vector<std::future<vector<string>>> mps;
    // wcout << L"fffff2" << endl;
    if (true){
        int worksize = 0;
        if(num <= input.get_rows()){
            worksize = num;
        }
        else{
            worksize = cluster_input_size;
        }
        if (data.size() < 24){
            worksize = 1;
        }
        // wcout << worksize << endl;
        for (int i = 0; i < worksize; i++) {
            int lastindex = 0;
            int startindex = i*cluster_input_size;
            lastindex = i*cluster_input_size + cluster_input_size;
            if (i != worksize-1)
                lastindex = i*cluster_input_size + cluster_input_size;
            else{
                lastindex = input.get_rows();
            }
            // wcout << "ssssssss1" << endl;
            // vector<vector<string>> subvector(input.begin()+startindex,input.begin()+lastindex);
            vector<string> subvector_ = input.subat(startindex,lastindex);
            // wcout << L"ssssss2" << ceil(static_cast<double>(subvector_.size()) / static_cast<double>(4)) << endl;
            MatrixString subvector(ceil(static_cast<double>(subvector_.size()) / static_cast<double>(input.get_cols())),input.get_cols());
            // wcout << L"sssssss3" << endl;
            subvector.data = subvector_;
            // cout << subvector.data.size() << endl;
            HFTokenizer* hftok = hftoks[i].get();
            shared_ptr<HTTokenizer2> curtok = tok[i];
            shared_ptr<HTPostagger2> curpos = pos[i];
            if (curtok && curpos && hftok) {
                mps.push_back(std::async(std::launch::async, 
                    [curtok, curpos, subvector, hftok]() -> std::vector<std::string> {
                        // [중요] tok, pos가 '값'으로 캡처되어 스레드 내에서 안전하게 생존함
                        return pos_(curtok, curpos, subvector, hftok);
                    }
                ));
            } else {
                std::wcerr << L"Error: 포인터 중 하나가 초기화되지 않았습니다 (NULL)." << std::endl;
            }
        }
        for (auto& result:mps){
            try{

                vector<string> tmp = result.get();
                for(string tt:tmp){
                    res_merge.push_back(tt);
                }
            }
            catch(const exception& e){
                wcerr << L"hi: " << s2ws(e.what()) << endl;
            }
        }
    }
    return res_merge;
}

HTMorph::HTMorph(int thread_num){
    // wcout << L"hi" << endl;
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    // wcout << L"hi" << endl;
    this->thread_num = thread_num;
    
    // wcout << L"hi" << endl;
    const char* env_var_name = "HTCPP"; // 얻고 싶은 환경변수 이름 (예: PATH)
    
    // wcout << L"hi" << endl;
    const char* env_value = std::getenv(env_var_name); // 환경변수 값 가져오기
    // wcout << L"hi" << endl;
    // cout << env_value << endl;;
    // wcout << L"hi" << endl;
    this->htpath = string(env_value);
    
    // wcout << L"hi" << endl;
    
    // wcout << L"hi" << endl;
    wstring htpath_tok = s2ws(this->htpath + "/htmorphs/tok-model-uint8.onnx");
    
    // wcout << L"hi" << endl;
    wstring htpath_pos = s2ws(this->htpath + "/htmorphs/pos-model-uint8.onnx");
    this->load_protobuf();
    
    // wcout << L"hi" << endl;
    for(int i=0;i<thread_num;i++){
        auto blob = LoadBytesFromFile(this->htpath+"/htmorphs/tokenizer/tokenizer.json");
        unique_ptr<HFTokenizer> tokenizer = FromBlobJSON(blob);

        // wstring htpath_p_datapath = s2ws(htpath+)
        hftoks.push_back(move(tokenizer));
        shared_ptr<HTTokenizer2> tok = make_shared<HTTokenizer2>(htpath_tok.c_str(), this->htpath + "/htmorphs/httokenizer.bin", tokdata);
        shared_ptr<HTPostagger2> pos = make_shared<HTPostagger2>(htpath_pos.c_str(), this->htpath + "/htmorphs/htpostagger.bin", htpath + "/htmorphs/tokenizer/tokenizer.json", posdata);
        this->posts.push_back(pos);
        this->tokens.push_back(tok);
    }
    // wcout << L"hi" << endl;
}

HTMorph::~HTMorph(){
    google::protobuf::ShutdownProtobufLibrary();
}

shared_ptr<const TokenizerData> HTMorph::load_tokdata(){
    TokDict prefs;
    if (ReadUserPreferencesFromFileTOK(this->htpath+"/htmorphs/httokenizer.bin", &prefs)) {
        std::wcerr << L"loadded" << std::endl;
    }

    shared_ptr<const TokenizerData> tokdata_ = make_shared<const TokenizerData>(prefs.dict());

    return tokdata_;
}

shared_ptr<const PosTaggerData> HTMorph::load_posdata(){
    PosDict prefs2;
    if (ReadUserPreferencesFromFilePOS(this->htpath+"/htmorphs/htpostagger.bin", &prefs2)) {
        std::wcerr << L"loadded" << std::endl;
    }
    shared_ptr<const PosTaggerData> posdata_ = make_shared<const PosTaggerData>(prefs2.i2p());//make_shared<PosTaggerData>(prefs.i2p());

    return posdata_;
}

void HTMorph::load_protobuf(){
    this->tokdata = this->load_tokdata();
    this->posdata = this->load_posdata();
}

vector<string> HTMorph::tokenizer(vector<string> data){
    vector<string> res_ht = make_mp(this->thread_num,data,tokens,posts,hftoks);
    // for(string s:res_ht){
    //     wcout << s2ws(s) << endl;
    // }
    return res_ht;
}

int main() {
    //SetEnv("HT","C:/Users/ty341/Desktop/httokenizer/out/build/Visual Studio Community 2022 Release - amd64/Release");
    std::setlocale(LC_ALL, "");
    
    HTMorph httagger(6);
    vector<string> data;
    wstring line;
    while (true){
        wcout << L"input: ";
    wcout.flush();
        getline(std::wcin, line);
        data.push_back(ws2s(line));
        //data.push_back(ws2s(line));
        //data.push_back(ws2s(line));
        vector<string> res = httagger.tokenizer(data);
	//wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
	for(auto vs: res){
	   wcout << s2ws(vs) << endl;
	}
        data.clear();
    }
    // std::ifstream inputFile;
    
    // inputFile.open("htmorphs/other.txt");
    // if (!inputFile.is_open()) {
    //     std::wcerr << L"파일을 열 수 없습니다." << std::endl;
    //     return 1;
    // }

    // std::string line;
    // int cnt = 0;
    // std::ofstream ofs("ht_res.txt", std::ios::out);

    // std::ifstream inputFile2;
    
    // inputFile2.open(httagger.htpath+"/htmorphs/other.txt");
    // if (!inputFile2.is_open()) {
    //     std::wcerr << L"파일을 열 수 없습니다." << std::endl;
    //     return 1;
    // }
    // int cline = 0;
    // string cline_str;
    // while (std::getline(inputFile2, cline_str)) {
    //     cline++;
    // }
    // inputFile2.close();
    // size_t probar_total = static_cast<size_t>(ceil(cline/1000));
    // cline = int(cline / 1000);
    // using namespace indicators;
    // ProgressBar bar{
    //     option::BarWidth{50},
    //     option::Start{"["},
    //     option::Fill{"="},
    //     option::Lead{">"},
    //     option::Remainder{" "},
    //     option::End{"]"},
    //     option::MaxProgress{probar_total}, 
        
    //     option::ShowPercentage{true},       // 퍼센트 표시 (필수 추천)
    //     option::ShowElapsedTime{true},      // 경과 시간 표시 (tqdm 느낌)
    //     option::ShowRemainingTime{true},    // 남은 시간 표시 (tqdm 느낌)
    //     // option::ForegroundColor{Color::cyan},
    //     option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
    // };
    // while (std::getline(inputFile, line)) {
    //     data.push_back(line);
    //     if (data.size() == 1000){
    //         cnt++;
    //         vector<string> res_ht = httagger.tokenizer(data);
    //         for(auto ht : res_ht){
    //             // cout << ht << endl;
    //             if(ofs.is_open()){
    //                 ofs << ht << "\n";
    //             }
    //         }
    //         data.clear();
    //         bar.tick();
    //     }
    // }
    // if (data.size() > 0){
    //     vector<string> res_ht = httagger.tokenizer(data);//make_mp(thread_num,data,tokens,posts,hftoks);
    //     for(auto ht : res_ht){
    //         if(ofs.is_open()){
    //             ofs << ht << "\n";
    //         }
    //     }
    //     bar.tick();
    // }
    // bar.mark_as_completed();
    // ofs.close();
    // google::protobuf::ShutdownProtobufLibrary();
    return 0;
}

// int main() {
//     SetEnv("HT","C:/Users/ty341/Desktop/httokenizer/out/build/Visual Studio Community 2022 Release - amd64/Release");
//     const char* env_var_name = "HT"; // 얻고 싶은 환경변수 이름 (예: PATH)
//     const char* env_value = std::getenv(env_var_name); // 환경변수 값 가져오기

//     string htpath = env_value;

//     std::setlocale(LC_ALL, "");
//     GOOGLE_PROTOBUF_VERIFY_VERSION;

//     PosDict prefs2;
//     if (ReadUserPreferencesFromFilePOS(htpath+"/htmorphs/htpostagger.bin", &prefs2)) {
//         std::wcerr << L"loadded" << std::endl;
//     }
//     shared_ptr<const PosTaggerData> posdata = make_shared<const PosTaggerData>(prefs2.i2p());//make_shared<PosTaggerData>(prefs.i2p());

//     TokDict prefs;
//     if (ReadUserPreferencesFromFileTOK(htpath+"/htmorphs/httokenizer.bin", &prefs)) {
//         std::wcerr << L"loadded" << std::endl;
//     }

//     shared_ptr<const TokenizerData> tokdata = make_shared<const TokenizerData>(prefs.dict());
//     vector<string> data;
//     std::ifstream inputFile;
    
//     inputFile.open("htmorphs/other.txt");
//     if (!inputFile.is_open()) {
//         std::wcerr << L"파일을 열 수 없습니다." << std::endl;
//         return 1;
//     }

//     std::string line;
//     int cnt = 0;
//     std::ofstream ofs("ht_res.txt", std::ios::out);

//     int thread_num = 6;
//     vector<unique_ptr<HFTokenizer>> hftoks;
//     vector<shared_ptr<HTPostagger2>> posts;
//     vector<shared_ptr<HTTokenizer2>> tokens;
    
//     wstring htpath_tok = s2ws(htpath + "/htmorphs/tok-model-uint8.onnx");
//     wstring htpath_pos = s2ws(htpath + "/htmorphs/pos-model-uint8.onnx");

//     for(int i=0;i<thread_num;i++){
//         auto blob = LoadBytesFromFile(htpath+"/htmorphs/tokenizer/tokenizer.json");
//         unique_ptr<HFTokenizer> tokenizer = FromBlobJSON(blob);

//         // wstring htpath_p_datapath = s2ws(htpath+)
//         hftoks.push_back(move(tokenizer));
//         shared_ptr<HTTokenizer2> tok = make_shared<HTTokenizer2>(htpath_tok.c_str(), htpath + "/htmorphs/httokenizer.bin", tokdata);
//         shared_ptr<HTPostagger2> pos = make_shared<HTPostagger2>(htpath_pos.c_str(), htpath + "/htmorphs/htpostagger.bin", htpath + "/htmorphs/tokenizer/tokenizer.json", posdata);
//         posts.push_back(pos);
//         tokens.push_back(tok);
//     }
    

//     std::ifstream inputFile2;
    
//     inputFile2.open(htpath+"/htmorphs/other.txt");
//     if (!inputFile2.is_open()) {
//         std::wcerr << L"파일을 열 수 없습니다." << std::endl;
//         return 1;
//     }
//     int cline = 0;
//     string cline_str;
//     while (std::getline(inputFile2, cline_str)) {
//         cline++;
//     }
//     inputFile2.close();
//     size_t probar_total = static_cast<size_t>(ceil(cline/1000));
//     cline = int(cline / 1000);
//     using namespace indicators;
//     ProgressBar bar{
//         option::BarWidth{50},
//         option::Start{"["},
//         option::Fill{"="},
//         option::Lead{">"},
//         option::Remainder{" "},
//         option::End{"]"},
//         option::MaxProgress{probar_total}, 
        
//         option::ShowPercentage{true},       // 퍼센트 표시 (필수 추천)
//         option::ShowElapsedTime{true},      // 경과 시간 표시 (tqdm 느낌)
//         option::ShowRemainingTime{true},    // 남은 시간 표시 (tqdm 느낌)
//         // option::ForegroundColor{Color::cyan},
//         option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
//     };
//     while (std::getline(inputFile, line)) {
//         data.push_back(line);
//         if (data.size() == 1000){
//             cnt++;
//             vector<string> res_ht = make_mp(thread_num,data,tokens,posts,hftoks);
//             for(auto ht : res_ht){
//                 // cout << ht << endl;
//                 if(ofs.is_open()){
//                     ofs << ht << "\n";
//                 }
//             }
//             data.clear();
//             bar.tick();
//         }
//     }
//     if (data.size() > 0){
//         vector<string> res_ht = make_mp(thread_num,data,tokens,posts,hftoks);
//         for(auto ht : res_ht){
//             if(ofs.is_open()){
//                 ofs << ht << "\n";
//             }
//         }
//         bar.tick();
//     }
//     bar.mark_as_completed();
//     ofs.close();
//     google::protobuf::ShutdownProtobufLibrary();
//     return 0;
// }
