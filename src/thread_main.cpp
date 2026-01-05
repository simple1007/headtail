// cppthreadtest.cpp : 애플리케이션의 진입점을 정의합니다.
//
// #include <Windows.h>
#include <thread>
#include <iostream>
#include <vector>
#include <cmath>
#include <future>
#include <fstream>
#include <regex>
#include <windows.h>
#include <cstdlib>
#include <format> // C++20 필수

// #include <filesystem>
#include "HTTokenizer2.h"
#include "HTPostagger2.h"
using namespace std;
// namespace fs = std::filesystem;
// 환경 변수 설정 함수
int infer_batch = 4;
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

// class ThreadHeadTail {
// public:
    // vector<vector<string>> data;
    // vector<string> resultht;
    // bool is_running = true;
    // /*HTTokenizer tok;
    // HTPostagger pos;*/

    // void setData(vector<vector<string>>& data){
    //     this->data = data;
    //     /*this->tok = tok;
    //     this->pos = pos;*/
    // }
    // vector<string> get_resultstr(Morphs m){
    //     std::locale::global(std::locale("")); 
    //     // cout << resultht.size() << endl;
    //     vector<string> resultht;
    //     // cout << m.toks.size() << L" " << m.poss.size() << endl;
    //     for(int i =0;i<m.toks.size();i++){
    //         if (m.toks[i].compare("+")!=0){
    //             cout << s2ws(m.toks[i]+"/"+m.poss[i]) << L" ";
    //             resultht.push_back(m.toks[i]+"/"+m.poss[i]);
    //         }else{
    //             resultht.push_back("+");
    //         }
    //     }
    //     cout << endl;
    //     return resultht;
    //     // cout << endl;
    // }
    vector<string> pos_(HTTokenizer2* tok,HTPostagger2* pos,vector<vector<string>> data_,HFTokenizer* hftoks) {
        //cout << "test" << endl;
        // std::locale::global(std::locale("")); 
        vector<string> resultht;
        vector<vector<string>> data = data_;
        // cout << L"HT: " << data.size()<<endl;
        // HFTokenizer hftok = (*hftoks);
        if (data.size()>0){
            for(vector<string> dv:data){
                
                try{
                    // for(auto s: dv){
                    //     cout << s2ws(s) << endl;
                    // }
                    // cout << dv.size() << endl;
                    // auto starttt = get_startt();
                    vector<string> toks = tok->tokenizer(dv);
                    // timer("tokenizer: ",starttt);
                    // for(auto tk:toks){
                    //     cout << "LLLLL" << tk << endl;
                    // }
                    // cout << endl;
                    vector<Morphs> poss = pos->postagger(hftoks,toks);
                    // cout << L"adfzdfasdfafa" << endl;
                    for(Morphs m:poss){
                        vector<wstring> tmp;
                        // bool prev_htsep = false;
                        for(int i =0;i<m.toks.size();i++){
                            tmp.push_back(s2ws(m.toks[i]+"/"+m.poss[i])+L" ");
                        }
                        wstring res_tk_pos = join(tmp);
                        // cout << ws2s(res_tk_pos) << endl;
                        std::wregex re(L"\\s\\+/\\+\\s"); 

                        // 2. 치환 수행
                        // 원본 문자열, 정규식 객체, 바꿀 문자열
                        res_tk_pos = std::regex_replace(res_tk_pos, re, L"+");
                        resultht.push_back(ws2s(strip(res_tk_pos)));
                    }
                    // cout << "endl" << endl;
                    // cout << L"adfzdfasdfafa" << endl;
                }catch(const std::exception& e){
                    
                    cerr << e.what() << endl;
                    // exit(0);
                }
                            
            }
        }
        return resultht;
        // this->is_running = false;
    }
 
// };
string replace(const string& text){
    wstring text_ = strip(s2ws(text));
    // cout << text_ << endl;

    std::wregex re(L"([^a-zA-Z\\s,\\d,가-힣]+)"); 

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
        // cout << ws2s(result) << endl;
    }
    // cout << "[" << ws2s(strip(result)) << "]"<< endl;
    // cout << 
    // cout << result << endl;
    // exit(0);
    return ws2s(result);
}
vector<vector<string>> normalize(const vector<string>& data,int inferbs) {
    vector<string> tmp;
    vector<vector<string>> res;
    // cout << data.size() << " " << inferbs << endl;
    for (string d : data) {
        d = replace(d);
        // cout << d << endl;
        tmp.push_back(d);
        if (tmp.size() == inferbs) {
            // cout << tmp.size() << endl;
            res.push_back(tmp);
            tmp.clear();
        }
    }
    if (tmp.size() > 0) {
        // cout << tmp.size() << endl;
        res.push_back(tmp);
        tmp.clear();
    }
    // cout << res.size() << endl;
    // cout << "hiffff" << endl;
    return res;
}
vector<string> make_mp(int num,const vector<string>& data,vector<HTTokenizer2>& tok, vector<HTPostagger2>& pos,const vector<unique_ptr<HFTokenizer>>& hftoks) {
    // std::locale::global(std::locale("")); 
    // MyClass myObject
    // 첫번째 인자로 멤버 함수를 전달할 때는 두번째 인자로 해당 객체 인스턴스를 같이 전달한다.
    // cout<<data.size() << endl;
    vector<string> res_merge;
    res_merge.reserve(data.size());
    vector<vector<string>> input = normalize(data,4);

    // cout << "norm.size" << input.size() << endl;
    int cluster_input_size = static_cast<int>(ceil(input.size() / num));
    // MyClass test;
    // test.setData();
    vector<std::future<vector<string>>> mps;
    // ThreadHeadTail ht;
    // vector<std::future<vector<string>> > resultht;
    // std::cout << f.get() << std::endl;
    if (true){
        int worksize = 0;
        if(num <= input.size()){
            worksize = num;
        }
        else{
            worksize = cluster_input_size;
        }
        // cout << data.size() << " "  << input.size() << " "<< worksize << endl;
        // exit(0);
        for (int i = 0; i < worksize; i++) {
            // vector<string> tmp;
            vector<vector<string>> subvector;
            // cout << L"input size:" << cluster_input_size << endl;
            int lastindex = 0;
            if (i != worksize-1)
                lastindex = i*cluster_input_size + cluster_input_size;
            else
                lastindex = input.size();
            for(int j=(i*cluster_input_size);j<lastindex;j++){
                // cout << j << endl;
                subvector.push_back(input[j]);
            }
            // cout << subvector.size() << endl;
            // subvector = vector<vector<string>>(input.begin() + (i*cluster_input_size), input.begin() + (i*cluster_input_size + cluster_input_size));
            // else
            //     subvector = vector<vector<string>>(input.begin() + (i*cluster_input_size), input.end());
            
            HFTokenizer* hftok = hftoks[i].get();
            std::future<vector<string>> f = std::async(pos_, &tok[i],&pos[i],subvector,hftok);
            mps.push_back(move(f));
            
        }
        
        for (auto& result:mps){
        //     // cout << result.size() << endl;
        //     // cout << s2ws(typeid(result).name()) << endl;
            try{

                vector<string> tmp = result.get();
                // cout << tmp.size() << endl;
                for(string tt:tmp){
                    // cout << tt<<endl;
                    res_merge.push_back(tt);

                }
                // cout << res_merge.size() << " ";
                //.insert(res_merge.end(),tmp.begin(),tmp.end());
                // cout << res_merge.size() << endl;
        //         wcout << tmp.size() << endl;
        //         for (auto s : tmp){
        //             cout << s << endl;
        //             // 1 == 1;
        //         }
        //         // cout << endl;
        //         // cout << s2ws(typeid(tmp).name()) << endl;
        //         // cout << tmp.size() << endl;
            }
            catch(const exception& e){
                cerr << "hi: " << e.what() << endl;
            }
        //     // for(auto r: tmp){
        // //     cout << s2ws(r) << L" ";
        // // }
        // // cout << endl;
        }
    }
    return res_merge;
}



// void join(vector<thread>& mps) {
//     // cout << mps.size();
//     cout << L"44444444444" << endl;
//     for (thread& t: mps) {
//         // if (th_ht[i].is_running) {
//         if (t.joinable()){
//             t.join();
//         }
//         //mps[i].join();
//     }
//     cout << L"44444444444" << endl;
//     // for(auto ht:th_ht){
        
//     //     for(auto s:ht.resultht){
//     //         cout << s2ws(s) << endl;
//     //     }
//     // }
// }

//class ThreadRUN {
//public:
//    vector<vector<string>> data;
//    vector<string> resultht;
//
//    ThreadRUN(vector<vector<string>>& data) {
//        this->data = data;
//    }
//
//    void run(HTTokenizer tok,HTPostagger pos) {
//
//    }
//
//};

int main() {
    //MyClass myObject;
    // 첫번째 인자로 멤버 함수를 전달할 때는 두번째 인자로 해당 객체 인스턴스를 같이 전달한다.
    /*thread t(&MyClass::memberFunction, &myObject, 5, 1);
    thread t2(&MyClass::memberFunction, &myObject, 5,2);*/
    // std::locale::global(std::locale(""));
    // std::cout.imbue(std::locale("kor")); 
    SetConsoleOutputCP(65001);
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // SetEnv("TOKENIZERS_PARALLELISM", "false");
    // int* p = nullptr;
    // *p = 42; // 여기서 무조건 죽어야 함
    // string s = "안녕하세요";
    // cout << s.substr(0,1) << endl;
    // return 0;
    // cout << replace("<<EOD>>") << endl;

    // return 0;
    // SetConsoleOutputCP(65001);
     // vector<string> data;
    // for (int i=0;i<4;i++){
    //     data.push_back("슈에의 앞모습 역시 코리도라스의 최강 매력은 복주머니 같은 앞 얼굴인 것 같다");
    //     data.push_back("가끔 카페에 잘 먹어서 뚠빵한 애들을 보면 진짜 그렇게 복스러운 돼지같을 수가 없다");
    //     data.push_back("지나칠 정도로 볼매 오늘은 어항 속 아이들을 보면서 또 다시 마음의 평화를 찾았다");
    //     data.push_back("나는 평화가 좋다");
    // }
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // vector<Morphs> p = pos.postagger(data);
    // make_mp(3,data,tok,pos);
    // for (auto m:p){
    //     for(int i=0;i<m.poss.size();i++){
    //         cout << m.poss[i] << "/" << m.toks[i] << " ";
    //     }
    //     cout << endl;
    //     // for(auto mm:m){

    //     // }
    // }
    vector<string> data;
    // fs::path p = u8"htmorphs/other.txt";
    // std::ifstream file(p); // OK
    std::ifstream inputFile;
    
    inputFile.open("htmorphs/other.txt");
    if (!inputFile.is_open()) {
        std::cerr << "파일을 열 수 없습니다." << std::endl;
        return 1;
    }

    std::string line;
    int cnt = 0;
    auto start = get_startt();
        // 파일을 UTF-8 모드로 쓰기 위해 파일 스트림 생성
    std::ofstream ofs("ht_res.txt", std::ios::out);

    // if (ofs.is_open()) {
    //     // 파일에 내용을 씁니다.
    //     ofs << content;
    //     ofs.close();
    //     std::cout << "파일 '" << filename << "'에 성공적으로 썼습니다." << std::endl;
    // } else {
    //     std::cerr << "파일을 열 수 없습니다: " << filename << std::endl;
    // }
    // const std::string filename = dictpath;//"HTTokenizer2.bin";
    // TokDict prefs;
    // if (ReadUserPreferencesFromFileTOK(, &prefs)) {
    //     std::cout << "loadded" << std::endl;
    // }

    //GOOGLE_PROTOBUF_VERIFY_VERSION;

    PosDict prefs2;
    if (ReadUserPreferencesFromFilePOS("htmorphs/htpostagger.bin", &prefs2)) {
        std::cerr << "loadded" << std::endl;
    }
    // cout << L"HI" << endl;
    shared_ptr<const PosTaggerData> posdata = make_shared<const PosTaggerData>(prefs2.i2p());//make_shared<PosTaggerData>(prefs.i2p());

    // const std::string filename = dictpath;//"httokenizer.bin";
    TokDict prefs;
    if (ReadUserPreferencesFromFileTOK("htmorphs/httokenizer.bin", &prefs)) {
        std::cout << "loadded" << std::endl;
    }

    // this->dict = prefs.dict();
    shared_ptr<const TokenizerData> tokdata = make_shared<const TokenizerData>(prefs.dict());
    // for(const auto& s:tokdata->get_word_to_id_map()){
    //     string ss = s.first;
    //     int64_t vv = s.second;
    //     cout << ss << " " << vv << endl;
    // }
    // exit(0);
    int thread_num = 6;
    vector<unique_ptr<HFTokenizer>> hftoks;
    vector<HTPostagger2> posts;\
    vector<HTTokenizer2> tokens;
    for(int i=0;i<thread_num;i++){
        auto blob = LoadBytesFromFile("htmorphs/tokenizer/tokenizer.json");
        unique_ptr<HFTokenizer> tokenizer = FromBlobJSON(blob);
        hftoks.push_back(move(tokenizer));

        HTTokenizer2 tok(L"htmorphs/tok-model-uint8.onnx","htmorphs/httokenizer.bin",tokdata);
        HTPostagger2 pos(L"htmorphs/pos-model-uint8.onnx","htmorphs/htpostagger.bin","htmorphs/tokenizer/tokenizer.json",posdata);
        posts.push_back(pos);
        tokens.push_back(tok);
    }
    
    std::ifstream inputFile2;
    
    inputFile2.open("htmorphs/other.txt");
    if (!inputFile2.is_open()) {
        std::cerr << "파일을 열 수 없습니다." << std::endl;
        return 1;
    }
    int cline = 0;
    string cline_str;
    while (std::getline(inputFile2, cline_str)) {
        cline++;
    }
    inputFile2.close();
    // inputFile._Seekbeg();
    cline = int(cline / 1000);
    // inputFile.seekg(std::ios::beg);
    // cout << cline;

    while (std::getline(inputFile, line)) {
        // 읽어온 한 줄(line)을 가지고 처리
        // std::cout << s2ws(line) << std::endl;
        // cout << line << endl;
        data.push_back(line);
        // cnt++;


        if (data.size() == 1000){
            cnt++;
            // cout << "\r" << cnt;
            // for(auto d: data){
            //     cout << d << endl;
            // }
            // cout << data.size() <<endl;
            auto start_pos = get_startt();
            vector<string> res_ht = make_mp(thread_num,data,tokens,posts,hftoks);
            for(auto ht : res_ht){
                // cout << ht << endl;
                if(ofs.is_open()){
                    ofs << ht << "\n";
                }
            }
            data.clear();
            timer("cline: "+to_string(cline)+"/"+to_string(cnt)+":",start_pos);
            // cout << data.size() << endl;
            // break;
        }
    }
    if (data.size() > 0){
        vector<string> res_ht = make_mp(thread_num,data,tokens,posts,hftoks);
        for(auto ht : res_ht){
            // cout << ht << endl;
            if(ofs.is_open()){
                ofs << ht << "\n";
            }
        }
        // make_mp(8,data,tok,pos);
    }

    ofs.close();
    timer("ht run",start);
    google::protobuf::ShutdownProtobufLibrary();
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    // data.push_back("는 밥을 먹고 학교에 갔다");
    // data.push_back("나는 밥을 먹고 학교에 갔다");
    //vector<thread> mps;
    // vector<ThreadHeadTail> hts;
    // make_mp(3, data,tok,pos);
    // join(mps);
    return 0;
}