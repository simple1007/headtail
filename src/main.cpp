// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#ifdef WIN32
#include <Windows.h>
#endif
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
#include <iostream>
#include <string>
#include <fstream>
#include "HTTokenizer2.h"
#include "HTPostagger2.h"
#include "thread_main2.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/complex.h>

namespace py = pybind11;
using namespace std;
// int main(){

//    std::locale::global(std::locale("")); 
//    HTTokenizer tokenizer("tok-model-uint8.onnx","httokenizer.bin");
//    HTPostagger postagger("pos-model-uint8.onnx","htpostagger.bin","tokenizer/tokenizer.json");

//    while(true){
//     vector<string> data;
//     wstring instr = L"나는 밥을 먹고 학교에 갔다";
//     // wcout << L"문장입력: ";
//     // getline(wcin,instr);
//     // wcout << instr << endl;
//     // vector<wstring> s = split(instr,L' ');
//     // for(auto a:s){
//     //   wcout << a << endl;
//     // }
//     // data.push_back("나는 밥을 먹고 학교에 갔다");
//     // data.push_back("집에 가고 싶어 제발요");
//     // data.push_back("구피는 난태생송사리과이다");
//     data.push_back(ws2s(instr));
//     vector<string> tokres = tokenizer.tokenizer(data);
//     // for (string t : tokres) {
//     //     wcout << s2ws(t) << endl;
//     // }
//     // wcout<<tokres.size() << endl;
    
//     vector<Morphs> posres = postagger.postagger(tokres);
//     // wcout<<posres.size() << endl;
    
//     for (Morphs m : posres){
//         for(int i=0;i<m.poss.size();i++){
//             wcout << s2ws(m.toks[i]) << L"/" << s2ws(m.poss[i]) << L" "; 
//         }
//         wcout << endl;
//     }
//     break;
//    }
   
//    return 0;
// }

// int add(int a,int b){
//    return a + b;
// }


PYBIND11_MODULE(_htmorphs, m) {          // "example" module name should be same of module name in CMakeLists
     m.doc() = "korean headtail-tail tokenizer-postagger module"; // optional module docstring

     py::class_<HTMorph>(m,"HTMorph")
      .def(py::init<const int&>())
      .def("tokenizer",&HTMorph::tokenizer);
    //  m.def("add", &add, "A function that adds two numbers");
     //m.def("sub", &sub, "A function that subtracts two numbers");

     /*py::class_<HTTokenizer>(m, "HTTokenizer")
        .def(py::init<const string&,const string&>())
        .def("tokenizer", &HTTokenizer::tokenizer,py::call_guard<py::gil_scoped_release>());

    
     py::class_<HTPostagger>(m, "HTPostagger")
        .def(py::init<const string&,const string&,const string&>())
        .def("postagger", &HTPostagger::postagger,py::call_guard<py::gil_scoped_release>());

      py::class_<Morphs>(m, "Morphs")
        
        // 3. 생성자 바인딩
        // Python에서 Pet() 호출 (기본 생성자)
        .def(py::init<>())
        .def_readwrite("toks", &Morphs::toks)
        .def_readwrite("poss", &Morphs::poss);
        // Python에서 Pet("Fluffy", 5) 호출
      //   .def(py::init<const std::string &, int>(), 
      //        py::arg("name"), py::arg("age")) // 키워드 인수 지원

        // 4. 멤버 변수 바인딩 (읽기/쓰기 가능)
        // Python에서 pet.name, pet.age로 접근 가능
      */
 }
