// #pragma once
// #ifndef HFTOKENIZER_H
// #define HFTOKENIZER_H


// #include <tokenizers_c.h>
// #include <tokenizers_cpp.h>

// #include <cassert>
// #include <chrono>
// #include <fstream>
// #include <iostream>
// #include <string>
// #include <deque>
// #include <map>
// #include "utils.h"

// using namespace std;
// using tokenizers::Tokenizer;

// /*!
//  * \brief A simple c++ header of tokenizer via C API.
//  */
// class HFTokenizer : public Tokenizer {
// public:
//     explicit HFTokenizer(TokenizerHandle handle) : handle_(handle) {
// #ifdef COMPILE_WASM_RUNTIME
//         setenv("TOKENIZERS_PARALLELISM", "false", true);
// #endif
//     }

//     HFTokenizer(const HFTokenizer&) = delete;
//     HFTokenizer(HFTokenizer&& other) { std::swap(other.handle_, handle_); }

//     ~HFTokenizer() {
//         if (handle_ != nullptr) {
//             tokenizers_free(handle_);
//         }
//     }

//     // use i32 to be consistent with sentencepiece
//     std::vector<int32_t> Encode(const std::string& text, bool add_special_tokens) {
//         TokenizerEncodeResult result;
//         tokenizers_encode(handle_, text.data(), text.length(), static_cast<int>(add_special_tokens),
//             &result);
//         std::vector<int32_t> ret(result.token_ids, result.token_ids + result.len);
//         tokenizers_free_encode_results(&result, 1);
//         return ret;
//     }

//     // use i32 to be consistent with sentencepiece
//     std::vector<int32_t> Encode(const std::string& text) final { return Encode(text, false); }

//     std::vector<std::vector<int32_t>> EncodeBatch(const std::vector<std::string>& texts,
//         bool add_special_tokens) {
//         std::vector<const char*> texts_raw;
//         std::vector<size_t> seq_lens;
//         size_t num_seqs = texts.size();
//         texts_raw.reserve(num_seqs);
//         seq_lens.reserve(num_seqs);
//         for (const auto& text : texts) {
//             texts_raw.push_back(text.data());
//             seq_lens.push_back(text.length());
//         }
//         std::vector<TokenizerEncodeResult> results(num_seqs);
//         tokenizers_encode_batch(handle_, texts_raw.data(), seq_lens.data(), texts.size(),
//             static_cast<int>(add_special_tokens), results.data());
//         std::vector<std::vector<int32_t>> ret;
//         ret.reserve(texts.size());
//         for (size_t i = 0; i < texts.size(); ++i) {
//             ret.push_back(
//                 std::vector<int32_t>(results[i].token_ids, results[i].token_ids + results[i].len));
//         }
//         tokenizers_free_encode_results(results.data(), texts.size());
//         return ret;
//     }

//     std::vector<std::vector<int32_t>> EncodeBatch(const std::vector<std::string>& texts) final {
//         return EncodeBatch(texts, false);
//     }

//     // use i32 to be consistent with sentencepiece
//     std::string Decode(const std::vector<int32_t>& ids, bool skip_special_tokens) {
//         tokenizers_decode(handle_, reinterpret_cast<const uint32_t*>(ids.data()), ids.size(),
//             static_cast<int>(skip_special_tokens));
//         const char* data;
//         size_t len;
//         tokenizers_get_decode_str(handle_, &data, &len);
//         return std::string(data, len);
//     }

//     std::string Decode(const std::vector<int32_t>& ids) final { return Decode(ids, false); }

//     size_t GetVocabSize() final {
//         size_t size;
//         tokenizers_get_vocab_size(handle_, &size);
//         assert(size > 0);
//         return size;
//     }

//     std::string IdToToken(int32_t id) final {
//         const char* data;
//         size_t len;
//         tokenizers_id_to_token(handle_, static_cast<uint32_t>(id), &data, &len);
//         return std::string(data, len);
//     }

//     int32_t TokenToId(const std::string& token) final {
//         int32_t id;
//         tokenizers_token_to_id(handle_, token.data(), token.length(), &id);
//         return id;
//     }

// private:
//     // internal handle
//     TokenizerHandle handle_{ nullptr };
// };

// inline std::unique_ptr<HFTokenizer> FromBlobJSON(const std::string& json) {

//     // std::make_shared<Tokenizer>(
//     return std::make_unique<HFTokenizer>(tokenizers_new_from_str(json.data(), json.length()));
// }

// inline std::unique_ptr<HFTokenizer> FromBlobByteLevelBPE(const std::string& vocab,
//     const std::string& merges,
//     const std::string& added_tokens) {
//     return std::make_unique<HFTokenizer>(byte_level_bpe_tokenizers_new_from_str(
//         vocab.data(), vocab.length(), merges.data(), merges.length(), added_tokens.data(),
//         added_tokens.length()));
// }

// inline std::string LoadBytesFromFile(const std::string& path) {
//     std::ifstream fs(path, std::ios::in | std::ios::binary);
//     if (fs.fail()) {
//         std::cerr << "Cannot open " << path << std::endl;
//         exit(1);
//     }
//     std::string data;
//     fs.seekg(0, std::ios::end);
//     size_t size = static_cast<size_t>(fs.tellg());
//     fs.seekg(0, std::ios::beg);
//     data.resize(size);
//     fs.read(data.data(), size);
//     return data;
// }

// inline void PrintEncodeResult(const std::vector<int>& ids) {
//     // setlocale(LC_ALL,"");  
//     std::cout << "tokens=[";
//     for (size_t i = 0; i < ids.size(); ++i) {
//         if (i != 0) std::cout << ", ";
//         std::cout << ids[i];
//     }
//     std::cout << "]" << std::endl;
// }

// inline int get_maxlen_(vector<vector<int>> data) {
//     int maxlen = -1;
//     for (int i = 0; i < data.size(); i++) {
//         int length = data[i].size();
//         if (maxlen < length) {
//             maxlen = length;
//         }
//         // cout << data[i].size() << endl;
//     }
//     // cout << maxlen << endl;
//     return maxlen;
// }

// inline vector<vector<int>> padding_input(const unique_ptr<HFTokenizer>& tok, vector<vector<int>> data) {
//     int padtoken_id = tok->TokenToId("[PAD]");
//     // cout<< typeid(tok->TokenToId("[PAD]")).name() <<endl;
//     // exit(0);
//     int maxlen = get_maxlen_(data);
//     for (size_t i = 0; i < data.size(); i++) {
//         vector<int> input_ = data[i];
//         if (input_.size() < maxlen) {
//             size_t pad_len = maxlen - static_cast<size_t>(input_.size());
//             for (int i = 0; i < pad_len; i++) {
//                 input_.push_back(padtoken_id);
//             }
//             data[i] = input_;
//         }
//     }
//     // cout << data[0].size() << L" " << maxlen << endl;

//     return data;
// }
// inline vector<vector<string>> split_eoj(const unique_ptr<HFTokenizer>& tok, vector<int> data, vector<string> pos, string text) {
//     vector<string> res;
//     vector<vector<string>> totalres;
//     // cout<<s2ws(text) << endl;
//     // cout << pos.size() << endl;
//     // cout << data.size() << endl;
//     int wordidx = 0;
//     auto texts_ = split(s2ws(text), L' ');
//     // cout<<texts_.size()<<endl;
//     if (texts_.size()==1){
//         // cout << texts_[0] << endl;
//         vector<string> tmp;
//         tmp.push_back(ws2s(texts_[0]));
//         // cout << pos.size() << endl;
//         if(data[1] == 19){
//         // start = 2;

//             if(pos[2].compare("O")!=0 && pos[2].compare("+")!=0){
//                 tmp.push_back(pos[2].substr(2,pos[2].size()));
//             }else{
//                 for(auto a:pos){
//                     if (a.compare("O")!=0 && a.compare("+")!=0){
//                         tmp.push_back(a.substr(2,a.size()));
//                     }
//                 }
//             }
//             if (tmp.size()==1){
//                 tmp.push_back("Fail");
//             }
//         }else{
//             if(pos[1].compare("O")!=0 && pos[1].compare("+")!=0){
//                 tmp.push_back(pos[1].substr(2,pos[1].size()));
//             }else{
//                 for(auto a:pos){
//                     if (a.compare("O")!=0 && a.compare("+")!=0){
//                         tmp.push_back(a.substr(2,a.size()));
//                     }
//                 }
//             }
//             if (tmp.size()==1){
//                 tmp.push_back("Fail");
//             }
//         }    
//         totalres.push_back(tmp);
//         return totalres;
//     }
//     // deque<wstring> dq(texts_.begin(), texts_.end());
//     // dq.push_back(L"[SEP]");
//     // vector<wstring> tks;
//     // vector<wstring> poss;
    
//     // for (int i=0;i<pos.size();i++){
//     //     wstring eoj = s2ws(tok->IdToToken(data[i]));
//     //     if (eoj.substr(0,1).compare(L"▁")!=0){
//     //         cout << s2ws(pos[i]) << L"/" << eoj << L" " ;
//     //     }
//     //     else{
//     //         cout << s2ws(pos[i]) << L"/" << L"_" << eoj.substr(1,eoj.size()) << L" ";
//     //     }
//     // }
//     // cout << endl;
//     int idx = 0;
//     vector<string> tmp;
//     map<string,int> cache;
//     int cnt = 0;
//     int start = 1;
//     // tok->IdToToken(data[1]);
//     if (data[1] == 19)
//         start = 2;
//     // cout << start << endl;
//     for (int i = start; i < data.size(); i++) {
//         if (data[i] >= 15000){
//             cout << data[i] << endl;
//         }
//         wstring eoj = s2ws(tok->IdToToken(data[i]));
        
//         // cout << L"test2[" << dq[0] << L"]" << endl;
//         // cout << 0 << endl;
//         // tks.push_back
//         if (eoj.compare(L"[SEP]") == 0){
//             if (texts_[idx-1].compare(L"+")==0){
//                 // cout << idx;
//                 // cout << L"fasdfsafdasf " << endl;
//                 tmp.push_back(ws2s(texts_[idx]));
//                 tmp.push_back(pos[i-1].substr(2,pos[i-1].size()));
//                 totalres.push_back(tmp);
//             }
//             // cout << cache.size() << endl;
//             // tmp.push_back(ws2s(texts_[idx]));
//             // tmp.push_back(pos[i-1].substr(2,pos[i-1].size()));
//             // totalres.push_back(tmp);
//             break;
//         }

//         if (eoj.substr(0,1).compare(L"▁")==0){
//             // if (i == 1 && eoj.size() == 1){
//             //     continue;
//             // }
//         // if (eoj.substr(0,1).compare(L"▁")==0){
//             // cout << eoj
//             // cout << texts.size()
//             // bool is_single_space = false;
//             // if (eoj.size()==1){
//             //     is_single_space = true;
//             // }
//             if (texts_[idx].compare(L"+") != 0){
//                 try{
//                     tmp.push_back(ws2s(texts_[idx]));
                    
//                     if (eoj.size()>0){
//                         // cout << start << L"\"" << eoj.substr(1,eoj.size()) << L"\"" << s2ws(pos[i]) << s2ws(pos[i-1]) <<i << L" "<< texts_[idx] << endl;
//                         // if (texts_[idx].size()>1)
//                         // cout << texts_[idx] <<
                        
//                         if (start == 2){
//                             if (pos[i-1].compare("O")!=0 && pos[i-1].compare("+")!=0 && texts_[idx].compare(L"+") != 0)
//                                 tmp.push_back(pos[i-1].substr(2,pos[i-1].size()));
//                             else{
//                                 if (cache.empty()){
//                                     tmp.push_back("Fail");
//                                 }
                                    
//                                 else{
//                                     string maxKey = cache.rbegin()->first;
//                                     wstring mKey = s2ws(maxKey).substr(2,maxKey.size());
                                    
//                                     tmp.push_back(ws2s(mKey));
//                                 }
//                             }
//                         }
//                         else{
//                             if (pos[i].compare("O")!=0 && pos[i].compare("+")!=0 && texts_[idx].compare(L"+") != 0)
//                                 tmp.push_back(pos[i].substr(2,pos[i].size())); 
//                             else {    
//                                 if (cache.empty()){
//                                     tmp.push_back("Fail");
//                                 }
                                    
//                                 else{
//                                     string maxKey = cache.rbegin()->first;
//                                     wstring mKey = s2ws(maxKey).substr(2,maxKey.size());
                                    
//                                     tmp.push_back(ws2s(mKey));
//                                 }
//                             }
//                         }
                           
//                         // else
//                         //     tmp.push_back(pos[i].substr(2,pos[i].size()));
//                     }
//                     // else if(eoj.size()> 1){
//                     //     cout << 1 << endl;
//                     //     // cout << texts_[idx] << s2ws(pos[i]);
//                     //     // if (i != start)
//                     //     // if (cache.size()>=1){
//                     //     // cout << endl;
//                     //     if(start==2)
//                     //         tmp.push_back(pos[i-1].substr(2,pos[i-1].size()));
//                     //     else
//                     //         tmp.push_back(pos[i].substr(2,pos[i].size()));
//                     //     cout << 2 << endl;
//                     // }
//                     // else{
//                     //     // cout << cache.size() << endl;
//                     //     if (cache.empty()){

//                     //         tmp.push_back("Fail");
//                     //     }
                            
//                     //     else{
//                     //         string maxKey = cache.rbegin()->first;
//                     //         // cout << s2ws(maxKey) << endl;
//                     //         // cout << s2ws(maxKey) << L" ";
//                     //         wstring mKey = s2ws(maxKey).substr(2,maxKey.size());
//                     //         // cout << mKey << endl;
//                     //         tmp.push_back(ws2s(mKey));
//                     //     }
//                     // }
                        
//                     cache.clear();
//                     totalres.push_back(tmp);
//                     tmp.clear();
//                     idx++;
//                 }
//                 catch(std::exception e){

//                     // for(auto r:)
//                     // cerr << i << L" " << s2ws(pos[i]) << L" " << s2ws(pos[i-1]) << endl;
//                     // cerr << idx << L" " << texts_.size() << L" \"_" << eoj.substr(1,eoj.size()) << L" \"" << endl;
//                     cerr << e.what() << endl;
//                     // exit(0);
//                 }
//             }
//                 // cout << texts_[idx] << s2ws(pos[i]) << endl;
//             else{
//                 tmp.push_back("+");
//                 tmp.push_back("+");
//                 totalres.push_back(tmp);
//                 cache.clear();
//                 tmp.clear();
//                 idx++;
//                 // cout << texts_[idx] << endl;
//             }
//                 // cout << L"+" << L"+" << endl;
//             // cnt++;
//             cache.clear();
            
//         }
//         else{
//             // cout << L"111111" << L" ";
//             if (start ==2){
                
//                 if (pos[i-1].compare("+") != 0 && pos[i-1].compare("O") != 0)
//                     cache[pos[i-1]] += 1;
//             }else if (start == 1){
                
//                 if (pos[i].compare("+") != 0 && pos[i].compare("O") != 0)
//                     cache[pos[i]] += 1;
//             }
//             // cout << L"222222" << endl;
//         }
//     }
//     return totalres;
// }
// // inline vector<vector<string>> split_eoj(const unique_ptr<HFTokenizer>& tok, vector<int> data, vector<string> pos, string text) {
// //     vector<string> res;
// //     vector<vector<string>> totalres;

// //     cout << pos.size() << endl;
// //     cout << data.size() << endl;
// //     int wordidx = 0;
// //     // cout << s2ws(text) << endl;
// //     // exit(0);
// //     auto texts_ = split(s2ws(text), L' ');
// //     // for(auto c: texts_){
// //     //     cout << c << endl;
// //     // }
// //     // exit(0);
// //     deque<wstring> dq(texts_.begin(), texts_.end());
// //     dq.push_back(L"[SEP]");
// //     // cout << dq[0] << endl;
// //     // auto poss = split(s2ws(pos),L' ');
// //     // dq.pop_front();
// //     // for (int i = 1; i < data.size(); i++) {
// //     //     // vector<string> tmp;
// //     //     wstring eoj = s2ws(tok->IdToToken(data[i]));
// //     //     if (eoj.substr(0,1).compare(L"▁")!=0){
// //     //         cout << eoj << endl;
// //     //     }
// //     //     else{
// //     //         cout << "_" << eoj.substr(1,eoj.size()) << endl;
// //     //     }
        
// //     // }
// //     for (int i = 1; i < data.size(); i++) {
// //         vector<string> tmp;
// //         wstring eoj = s2ws(tok->IdToToken(data[i]));
// //         // cout << L"test2[" << dq[0] << L"]" << endl;
// //         // cout << 0 << endl;
// //         if (eoj.substr(0,1).compare(L"▁")!=0){
// //             cout << eoj << " " << s2ws(pos[i]) << endl;
// //         }
// //         else{
// //             cout << "_" << eoj.substr(1,eoj.size())  << " " << s2ws(pos[i]) << endl;
// //         }
// //         if (eoj.compare(L"[SEP]") == 0) {
// //             int cnt = 0;
// //             // cout << 3 << endl;
// //             for (int j = 0; j < res.size(); j++) {
// //                 if (cnt > 2) {
// //                     break;
// //                 }
// //                 if (cnt == 0 && res[j].compare("+") != 0) {
// //                     // cout << dq[0] << endl;

// //                     tmp.push_back(ws2s(dq[0]));
// //                     // cout << s2ws(pos[i-1]) << dq[0] << endl;
// //                     dq.pop_front();

// //                     wstring curpos = s2ws(pos[i]);
// //                     curpos = curpos.substr(2, curpos.size());

// //                     tmp.push_back(ws2s(curpos));

// //                     cnt++;
// //                     totalres.push_back(tmp);
// //                 }
// //                 else if (res[j].compare("+") == 0) {
// //                     // cout << dq[0] << endl;
// //                     tmp.push_back("+");
// //                     dq.pop_front();
// //                     tmp.push_back("+");
// //                     cnt++;
// //                     totalres.push_back(tmp);
// //                 }

// //             }

// //             /*tmp.push_back(ws2s(dq[0]));
// //             dq.pop_front();
// //             wstring curpos = s2ws(pos[i-1]);
// //             curpos = curpos.substr(2,curpos.size());
// //             tmp.push_back(ws2s(curpos));
// //             toalres.push_back(tmp);*/

// //             res.clear();
// //             tmp.clear();
// //             break;
// //         }
// //         // cout << 1 << endl;
// //         if (eoj.substr(0, 1).compare(L"▁") == 0 && eoj.size() > 1) {// || eoj.substr(0,1).compare(L"+")==0 ){
// //             // for (int j = 0;j<res.size();j++){
// //             //   if (res[j].compare("+") != 0){
// //             //     tmp.push_back(res[j]);
// //             //     // cout << s2ws(res[j]) << L" ";
// //             //     // cout << s2ws(pos[i]);
// //             //   }else{
// //             //     cout << L"+" << L" ";
// //             //     cout << L"+";
// //             //   }
// //             // }
// //             // cout << s2ws(res[0]) << endl;
// //             int cnt = 0;
// //             for (int j = 0; j < res.size(); j++) {
// //                 // if (res.size() > 1){

// //                 if (cnt > 2) {
// //                     break;
// //                 }
// //                 if (cnt == 0 && res[j].compare("+") != 0) {
// //                     // cout << dq[0] << endl;
                    
// //                     tmp.push_back(ws2s(dq[0]));
// //                     // cout << s2ws(pos[i-1]) << dq[0] << endl;
// //                     dq.pop_front();
                    
// //                     wstring curpos = s2ws(pos[i]);
// //                     curpos = curpos.substr(2,curpos.size());
                    
// //                     tmp.push_back(ws2s(curpos));
                    
// //                     // cout << s2ws(tmp[0]) << L" " << s2ws(tmp[1]) << endl;
// //                     // tmp.push_back(move(pos[i-1]));
// //                     cnt++;
// //                     totalres.push_back(tmp);
// //                     // cout << s2ws(tmp[0])<< L" " << s2ws(tmp[1]) << endl;
// //                 }
// //                 else if (res[j].compare("+") == 0) {
// //                     // cout << dq[0] << endl;
// //                     tmp.push_back("+");
// //                     dq.pop_front();
// //                     tmp.push_back("+");
// //                     cnt++;
// //                     totalres.push_back(tmp);
// //                 }
                
// //             }


// //             tmp.clear();
// //             // cout << endl;
// //             res.clear();
// //             // cout << 2 << endl;
// //             // wstring tmptk = s2ws(tok->IdToToken(data[i]));
// //             if (eoj.substr(0, 1).compare(L"+") != 0) {
// //                 if (eoj.compare(L"▁") != 0) {
// //                     res.push_back("_" + ws2s(eoj.substr(1, eoj.length())));
// //                 }
// //                 // cout << s2ws(pos[i]);
// //             }
// //             else {
// //                 res.push_back("+");
// //                 // cout << s2ws("+");
// //             }
// //             // cout << endl;

// //         }
// //         else {
// //             res.push_back(tok->IdToToken(data[i]));
// //         }
// //     }
// //     // for(int i=0;i<totalres.size();i++){
// //     //     cout << s2ws(totalres[i][0]) << s2ws(totalres[i][1]) << endl;
// //     // }
// //     return totalres;
// // }
// // inline void TestTokenizer2(const std::unique_ptr<HFTokenizer>& tok, bool print_vocab = false,
// //     bool check_id_back = true) {
// //     // Check #1. Encode and Decode
// //     // setlocale(LC_ALL, "");
// //     // std::string prompt = "[CLS] What is the  capital of Canada? [SEP]";
// //     vector<string> data;
// //     data.push_back("나 + 는 밥 + 을 먹 + 고 학교 + 에 갔 + 다");
// //     data.push_back("What is the  capital of Canada?");
// //     data.push_back("만나서 반가워 나는 김정민이야");
// //     // tok->
// //     std::vector<std::vector<int>> idss = tok->EncodeBatch(data, true);
// //     idss = padding_input(tok, idss);
// //     // idss = padding_input(tok,idss);
// //     for (auto d : idss) {
// //         // cout << d << endl;
// //         string dd = tok->Decode(d, false);
// //         cout << d.size() << endl;
// //         vector<string> pos;
// //         for (int i = 0; i < d.size(); i++) {
// //             pos.push_back("NOUN");
// //         }
// //         // exit(0);
// //         // tok->Decode(d,false);
// //         auto join_ = split_eoj(tok, d, pos, data[0]);
// //         // cout << join_.size() << endl;
// //         vector<wstring> restk;
// //         vector<wstring> respos;
// //         for (int i = 0; i < join_.size() - 1; i++) {
// //             // cout << i << endl;
// //             restk.push_back(s2ws(join_[i][0]));
// //             if (join_[i][0].compare("+") != 0 && join_[i + 1][0].compare("+") != 0)
// //                 restk.push_back(L" ");
// //             respos.push_back(s2ws(join_[i][1]));
// //             // if (join_[i][1].compare("+") != 0)
// //             if (join_[i][0].compare("+") != 0 && join_[i + 1][0].compare("+") != 0)
// //                 respos.push_back(L" ");
// //             // cout << s2ws(join[i][0]) << L"/" << s2ws(join[i][1]) << endl;
// //         }
// //         int lastidx = join_.size() - 1;
// //         restk.push_back(s2ws(join_[lastidx][0]));
// //         respos.push_back(s2ws(join_[lastidx][1]));
// //         wstring res_tks = join(restk);
// //         wstring res_pos = join(respos);
// //         cout << ws2s(res_tks) << endl;
// //         cout << ws2s(res_pos) << endl;
// //         // cout << endl;
// //         exit(0);
// //         cout<< dd << endl;
// //         PrintEncodeResult(d);
// //     }
// // }

// // inline void HuggingFaceTokenizerExample() {
// //     std::cout << L"Tokenizer: Huggingface" << std::endl;

// //     auto start = std::chrono::high_resolution_clock::now();

// //     // Read blob from file.
// //     // auto blob = LoadBytesFromFile("dist/tokenizer.json");
// //     // Note: all the current factory APIs takes in-memory blob as input.
// //     // This gives some flexibility on how these blobs can be read.
// //       // Read blob from file.
// //     auto blob = LoadBytesFromFile("dist/tokenizer.json");
// //     // Note: all the current factory APIs takes in-memory blob as input.
// //     // This gives some flexibility on how these blobs can be read.
// //     // auto tok = Tokenizer::FromBlobJSON(blob);
// //     unique_ptr<HFTokenizer> tok = FromBlobJSON(blob);
// //     // tokenizers_new_from_str(json.data(), json.length())
// //     auto end = std::chrono::high_resolution_clock::now();
// //     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

// //     std::cout << L"Load time: " << duration << L" ms" << std::endl;

// //     TestTokenizer2(tok, false, true);
// // }
// #endif