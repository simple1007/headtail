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
#ifdef _WIN32
	#include <windows.h>
#endif
#include <cstdlib>
#include <format> // C++20 필수
#include "indicators.hpp"
// #include <filesystem>
#include "HTTokenizer2.h"
#include "HTPostagger2.h"
#include <clocale>
#include <cstdlib>
using namespace std;
// namespace fs = std::filesystem;
// 환경 변수 설정 함수
//int infer_batch = 4;
void SetEnv(const std::string& name, const std::string& value);

vector<string> pos_(const shared_ptr<HTTokenizer2>& tok,const shared_ptr<HTPostagger2>& pos,const MatrixString& data_,HFTokenizer* hftoks);
string replace(const string& text);
MatrixString normalize(vector<string> data,int inferbs);
vector<string> make_mp(int num,vector<string> data,vector<shared_ptr<HTTokenizer2>> tok, vector<shared_ptr<HTPostagger2>> pos,const vector<unique_ptr<HFTokenizer>>& hftoks);
class HTMorph{
public:
    int thread_num;
    string htpath;
    vector<unique_ptr<HFTokenizer>> hftoks;
    vector<shared_ptr<HTPostagger2>> posts;
    vector<shared_ptr<HTTokenizer2>> tokens;
    HTMorph(int thread_num);
    ~HTMorph();
    shared_ptr<const TokenizerData> tokdata;
    shared_ptr<const PosTaggerData> posdata;
    vector<string> tokenizer(vector<string> data);
private:
    void load_protobuf();
    shared_ptr<const TokenizerData> load_tokdata();
    shared_ptr<const PosTaggerData> load_posdata();
};
