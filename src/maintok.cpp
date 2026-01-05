// // Copyright (c) Microsoft Corporation. All rights reserved.
// // Licensed under the MIT License.
#define UNICODE

#include <Windows.h>
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
#include "HTTokenizer.h"
#include "HTPostagger.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;
//int main(){
//
//    std::locale::global(std::locale("")); 
//    HTTokenizer tokenizer("tok-model-uint8.onnx","httokenizer.bin");
//    HTPostagger postagger("pos-model-uint8.onnx","htpostagger.bin","tokenizer/tokenizer.json");
//
//    vector<string> data;
//    data.push_back("나는 밥을 먹고 학교에 갔다");
//    data.push_back("집에 가고 싶어 제발요");
//    data.push_back("구피는 난태생송사리과이다");
//    vector<string> tokres = tokenizer.tokenizer(data);
//    for (string t : tokres) {
//        wcout << s2ws(t) << endl;
//    }
//    vector<Morphs> posres = postagger.postagger(tokres);
//
//    for (Morphs m : posres){
//        for(int i=0;i<m.poss.size();i++){
//            wcout << s2ws(m.toks[i]) << L"/" << s2ws(m.poss[i]) << L" "; 
//        }
//        wcout << endl;
//    }
//    return 0;
//}


PYBIND11_MODULE(_htmorphs, m) {          // "example" module name should be same of module name in CMakeLists
     m.doc() = "korean headtail-tail tokenizer-postagger module"; // optional module docstring

     //m.def("add", &add, "A function that adds two numbers");
     //m.def("sub", &sub, "A function that subtracts two numbers");

     py::class_<HTTokenizer>(m, "HTTokenizer")
         .def(py::init<const string&,const string&>())
         .def("tokenizer", &HTTokenizer::tokenizer);

    
     py::class_<HTPostagger>(m, "HTPostagger")
         .def(py::init<const string&,const string&,const string&>())
         .def("postagger", &HTPostagger::postagger);
 }