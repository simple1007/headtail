#pragma once

#ifndef UTILS_H
#define UTILS_H

#include<string>
#include<vector>
#include <utf8cpp/utf8.h> // 다운로드한 utf8.h 헤더 포함
#include <chrono>
#pragma once
using namespace std;
std::wstring s2ws(const std::string& utf8_str);

// std::wstring (플랫폼 종속) -> std::string (UTF-8)
std::string ws2s(const std::wstring& wstr);

wstring strip(wstring s);
vector<wstring> split(wstring s,wchar_t sep);

vector<wstring> ws2wsvec(const wstring str);

wstring join(const vector<wstring>& str);

void timer(string type, chrono::steady_clock::time_point start);
chrono::steady_clock::time_point get_startt();
#endif