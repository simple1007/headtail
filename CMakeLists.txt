cmake_minimum_required(VERSION 3.10)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# Visual Studio (Windows) 설정
# add_compile_options(/O2 /arch:AVX2 /fp:fast)
if(WIN32)
	add_compile_options(/O2 /GL /Oi /Ot /Oy /arch:AVX2 /fp:fast /Gy /Gw)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_compile_options(
        	-O3                   # /O2, /Ot 대응 (가장 강력한 최적화)
        	-flto                 # /GL 대응 (Link Time Optimization)
        	-fomit-frame-pointer  # /Oy 대응
        	-mavx2                # /arch:AVX2 대응
        	-ffast-math           # /fp:fast 대응
        	-ffunction-sections   # /Gy 대응 (함수별 섹션 분리)
        	-fdata-sections       # /Gw 대응 (데이터별 섹션 분리)
    	)
	add_link_options(
        	-flto                 # /GL 대응 (LTO는 링커에도 전달되어야 함)
        	-Wl,--gc-sections     # /Gy, /Gw로 분리된 섹션 중 사용 안 하는 것 제거
    	)
endif()
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
# add_compile_options(/GL /Oi /Ot /Oy /arch:AVX2 /fp:fast /Gy /Gw)

# (선택사항) 전체 프로그램 최적화 (Link Time Optimization)
# 빌드 시간은 길어지지만 실행 속도는 더 빨라집니다.
# add_compile_options(/GL)
set(pybind "1")
# MSVC (Visual Studio)의 경우
# add_compile_options(/fsanitize=address)
# add_compile_options(/Z7)
# add_link_options(/DEBUG)
set(CMAKE_BUILD_TYPE Release)
project("htmorphs")
include_directories(src)
#include_directories("$ENV{VCPKG_ROOT}/installed/x64-linux/include/utf8cpp")
find_package(protobuf CONFIG REQUIRED)

find_package(utf8cpp CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
set(PROTO_FILES
    tok_dict.proto
)
set(PROTO_FILES2
    pos_dict.proto
)
# pybind11_add_module(_htmorphs

if(${pybind} STREQUAL "1")
    set(PYBIND11_FINDPYTHON ON)
    find_package(pybind11 CONFIG REQUIRED)
    pybind11_add_module(_htmorphs
        src/main.cpp
        src/utils.cpp
        src/BaseORT.cpp
        src/HTTokenizer2.cpp
        src/hftokenizer.h    
        src/HTPostagger2.cpp
	src/thread_main2.cpp
        ${PROTO_FILES}
        ${PROTO_FILES2} 
    )
    file(CHMOD htmorphs/libonnxruntime.so.1.17.3 PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ)
    install(TARGETS _htmorphs LIBRARY DESTINATION htmorphs)
else()
    # message("111111")
    message("${CMAKE_BUILD_TYPE}")
    add_executable(_htmorphs
        src/thread_main2.cpp
        src/utils.cpp
        src/BaseORT.cpp
        src/HTTokenizer2.cpp
        src/hftokenizer.h    
        src/HTPostagger2.cpp
        ${PROTO_FILES}
        ${PROTO_FILES2} 
    )
    # add_executable(_htmorphs
    #     src/HTPostagger2.cpp
    #     src/utils.cpp
    #     src/BaseORT.cpp
    #     ${PROTO_FILES}
    #     ${PROTO_FILES2} 
    # )
    if(MSVC)
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/tokenizer")
        file(COPY ${CMAKE_SOURCE_DIR}/htmorphs DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
        file(COPY ${CMAKE_SOURCE_DIR}/htmorphs/tokenizer DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/htmorphs)
    else()
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tokenizer")
        file(COPY ${CMAKE_SOURCE_DIR}/htmorphs DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/htmorphs/tokenizer DESTINATION ${CMAKE_BINARY_DIR}/htmorphs)
    endif()
endif()
target_link_libraries(_htmorphs PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
target_link_libraries(_htmorphs PRIVATE utf8cpp::utf8cpp)
#target_link_libraries(_htmorphs PRIVATE utf8cpp::utf8cpp)
# install(TARGETS _htmorphs LIBRARY DESTINATION htmorphs)
protobuf_generate(TARGET _htmorphs)
# if(WIN32)
target_include_directories(_htmorphs PRIVATE onnxruntime/include)
target_link_directories(_htmorphs PRIVATE onnxruntime/lib) # lib 파일 존재하는 디렉토리 지정
target_link_libraries(_htmorphs PRIVATE onnxruntime) # 실제로 어떤 라이브러리를 사용할지를 지정
# elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
# 	target_include_directories(_htmorphs PRIVATE onnxruntime/onnxruntime_linux/include)
# 	target_link_directories(_htmorphs PRIVATE onnxruntime/onnxruntime_linux/lib) # lib 파일 존재하는 디렉토리 지정
# 	target_link_libraries(_htmorphs PRIVATE onnxruntime)# 실제로 어떤 라이브러리를 사용할지를 지정
# endif()
set(TOKENZIER_CPP_PATH src/tokenizers-cpp)
add_subdirectory(${TOKENZIER_CPP_PATH} tokenizers EXCLUDE_FROM_ALL)
target_include_directories(_htmorphs PRIVATE ${TOKENZIER_CPP_PATH}/include)

target_link_libraries(_htmorphs PRIVATE tokenizers_cpp)
if(MSVC)
    # Windows (Visual Studio)
    target_compile_options(_htmorphs PRIVATE /O2 /W4)
else()
    # Linux (GCC/Clang)
    # /O2 -> -O3 (강력한 최적화)
    # /W4 -> -Wall -Wextra (높은 수준의 경고)
    target_compile_options(_htmorphs PRIVATE -O3 -Wall -Wextra)
endif()
# target_compile_options(_htmorphs PRIVATE /W4)

if(${pybind} STREQUAL "1")
    target_link_libraries(_htmorphs PRIVATE pybind11::embed)
endif()
