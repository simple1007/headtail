#pragma once  // <--- 이 줄을 추가하세요!
#ifndef MY_STRUCT_H  // 만약 정의되지 않았다면
#define MY_STRUCT_H  // 정의하고
#include <vector>
#include <string>
template <typename T>
struct Matrix {
    std::vector<T> data;
    size_t cols;
    size_t rows;

    Matrix(size_t rows, size_t cols) : data(rows * cols), cols(cols), rows(rows) {}

    // [안전성 수정] 멤버 변수를 직접 수정하지 못하도록 const 반환 또는 값 반환 추천
    inline size_t get_rows() const { return rows; }
    inline size_t get_cols() const { return cols; }

    // 데이터 수정용
    inline T& at(size_t y, size_t x) {
        return data[y * cols + x];
    }

    // 읽기 전용
    inline const T& at(size_t y, size_t x) const {
        return data[y * cols + x];
    }

    // // 읽기 전용
    inline vector<T> at(size_t i) {
        // return data[y * cols + x];
        vector<T> tmp(data.begin()+(i * cols),data.begin()+(i * cols + cols));
        return tmp;
    }

    inline vector<T> subat(size_t i,size_t j) {
        // return data[y * cols + x];
        vector<T> tmp(data.begin()+(i * cols),data.begin()+(j * cols));

        return tmp;
    }

    // C API 연동용 포인터
    T* ptr() { return data.data(); }
    const T* ptr() const { return data.data(); }
};

// 기존 코드와 호환성을 위해 별칭(alias) 사용 가능
using MatrixFloat = Matrix<float>;
using MatrixString = Matrix<std::string>;
using MatrixInt = Matrix<int>;
struct Morphs {
    vector<string> toks;
    vector<string> poss;
};

struct Morph{
    bool isdata = false;
    bool istail = false;
    string head_tk;
    string head_pos;
    string tail_tk;
    string tail_pos;
};
using MatrixMorph = Matrix<Morph>;
// struct MatrixFloat {
//     std::vector<float> data;
//     size_t cols;
//     size_t rows;
//     MatrixFloat(size_t rows, size_t cols) : data(rows * cols), cols(cols), rows(rows) {}

//     inline size_t& rows_(){
//         return rows;
//     }
//     inline size_t& cols_(){
//         return cols;
//     }
//     // 인라인 함수로 오버헤드 없음
//     inline float& at(size_t y, size_t x) {
//         return data[y * cols + x];
//     }
    
//     // const 버전
//     inline float at(size_t y, size_t x) const {
//         return data[y * cols + x];
//     }
    
//     // 로우 데이터 포인터 반환 (C API 연동 시 유용)
//     float* ptr() { return data.data(); }
// };

// struct MatrixString {
//     std::vector<std::string> data;
//     size_t cols;
//     size_t rows;
//     MatrixString(size_t rows, size_t cols) : data(rows * cols), cols(cols), rows(rows) {}

//     inline size_t& rows_(){
//         return rows;
//     }
//     inline size_t& cols_(){
//         return cols;
//     }
//     // 인라인 함수로 오버헤드 없음
//     inline std::string& at(size_t y, size_t x) {
//         return data[y * cols + x];
//     }
    
//     // const 버전
//     inline std::string at(size_t y, size_t x) const {
//         return data[y * cols + x];
//     }
    
//     // 로우 데이터 포인터 반환 (C API 연동 시 유용)
//     std::string* ptr() { return data.data(); }
// };

// struct MatrixInt {
//     std::vector<int> data;
//     size_t cols;
//     size_t rows;
//     MatrixInt(size_t rows, size_t cols) : data(rows * cols), cols(cols), rows(rows) {}

//     inline size_t& rows_(){
//         return rows;
//     }
//     inline size_t& cols_(){
//         return cols;
//     }
//     // 인라인 함수로 오버헤드 없음
//     inline int& at(size_t y, size_t x) {
//         return data[y * cols + x];
//     }
    
//     // const 버전
//     inline int at(size_t y, size_t x) const {
//         return data[y * cols + x];
//     }
    
//     // 로우 데이터 포인터 반환 (C API 연동 시 유용)
//     int* ptr() { return data.data(); }
// };
#endif