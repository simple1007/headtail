#include "utils.h"
#include <iostream>
#include <clocale>
#include <chrono>
//void file_write(vector) {
//    //std::fstream myFile;
//    //myFile.open("output.txt", std::ios_base::out | std::ios_base::binary);
//
//    //// UTF-8 BOM (Byte Order Mark) 추가
//    //myFile.write("\xEF\xBB\xBF", 3);
//
//    //std::string myString = "안녕하세요, UTF-8 파일입니다.";
//    file.write(line.c_str(), myString.size());
//
//    myFile.close();
//}

wstring strip(wstring s) {
    //wcout << "\"" << s << "\"" << endl;
    // 앞 공백 제거
    size_t start = s.find_first_not_of(L" \t\n\r\f\v");
    if (start != std::string::npos) {
        s = s.substr(start);
    }

    // 뒤 공백 제거
    size_t end = s.find_last_not_of(L" \t\n\r\f\v");
    if (end != std::string::npos) {
        s = s.substr(0, end + 1);
    }
    //std::wcout << "\"" << s << "\"" << std::endl; // "Hello, World!" 출력

    return s;
}

vector<wstring> split(wstring s,wchar_t sep) {
    //cout << s << endl;
    //wstring s_ = s2ws(s);
    //wcout << s << endl;
    vector<wchar_t> svector(s.begin(), s.end());

    vector<wstring> resvector;
    vector<wchar_t> tmp;
    //wcout << svector.size() << endl;
    for (int i = 0; i < svector.size(); i++) {
        wchar_t cur = svector[i];
        //wcout << cur << endl;
        if (cur != sep)
            tmp.push_back(cur);
        else {
            //wcout << cur << endl;
            wstring wtmp(tmp.begin(), tmp.end());
            resvector.push_back(wtmp);
            //wcout << wtmp << endl;
            tmp.clear();
            //return L"";
        }

    }
    wstring wtmp(tmp.begin(), tmp.end());
    resvector.push_back(wtmp);
    //wcout << wstring(tmp.begin(), tmp.end()) << endl;
    return resvector;
}


vector<wstring> ws2wsvec(const wstring str) {
    vector<wchar_t> str_arr(str.begin(), str.end());

    vector<wstring> res;
    for (auto s : str_arr) {
        res.push_back(wstring({ s }));
    }

    return res;
}


wstring join(const vector<wstring>& str) {
    wstring res;
    for (wstring res_ : str) {
        res += res_;
    }

    return res;
}

void timer(string type, chrono::steady_clock::time_point start) {
    #ifdef _WIN32
    	chrono::steady_clock::time_point end_ = std::chrono::high_resolution_clock::now();
    #else
	chrono::steady_clock::time_point end_ = std::chrono::steady_clock::now();
    #endif
    std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start);
    std::cout << type << " 실행 시간: " << duration_ms.count() / 1000.0 << " ms" << std::endl;
}
chrono::steady_clock::time_point get_startt() {
    #ifdef _WIN32
	return std::chrono::high_resolution_clock::now();
    #else
	return std::chrono::steady_clock::now();
    #endif
    // return std::chrono::high_resolution_clock::now();
    // return std::chrono::steady_clock::now();
}

// std::string (UTF-8) -> std::wstring (플랫폼 종속)
std::wstring s2ws(const std::string& utf8_str)
{
    //utf8_str()
    //cout << utf8_str << endl;
    std::wstring wstr;
    if (utf8_str.empty()) return wstr;

#ifdef _WIN32
    // Windows: UTF-8 -> UTF-16 (std::wstring)
    std::u16string u16_str;
    utf8::utf8to16(utf8_str.begin(), utf8_str.end(), std::back_inserter(u16_str));
    //std::cout >> u16_str << <>> std::endl;
    // std::u16string을 std::wstring으로 복사 (Windows에서 wchar_t는 16비트)
    wstr.assign(reinterpret_cast<const wchar_t*>(u16_str.c_str()), u16_str.length());

#else
    // Linux: UTF-8 -> UTF-32 (std::wstring)
    std::u32string u32_str;
    utf8::utf8to32(utf8_str.begin(), utf8_str.end(), std::back_inserter(u32_str));
    // std::u32string을 std::wstring으로 복사 (Linux에서 wchar_t는 32비트)
    wstr.assign(u32_str.begin(), u32_str.end());
#endif
    //std::wcout << wstr << std::endl;
    return wstr;

}

// std::wstring (플랫폼 종속) -> std::string (UTF-8)
std::string ws2s(const std::wstring& wstr)
{
    std::string utf8_str;
    if (wstr.empty()) return utf8_str;

#ifdef _WIN32
    // Windows: UTF-16 (std::wstring) -> UTF-8
    // std::wstring을 std::u16string으로 변환 (char16_t와 wchar_t는 크기가 같음)
    std::u16string u16_str(reinterpret_cast<const char16_t*>(wstr.c_str()), wstr.length());
    utf8::utf16to8(u16_str.begin(), u16_str.end(), std::back_inserter(utf8_str));
#else
    // Linux: UTF-32 (std::wstring) -> UTF-8
    // std::wstring을 std::u32string으로 변환 (char32_t와 wchar_t는 크기가 같음)
    std::u32string u32_str(wstr.begin(), wstr.end());
    utf8::utf32to8(u32_str.begin(), u32_str.end(), std::back_inserter(utf8_str));
#endif
    return utf8_str;
}
