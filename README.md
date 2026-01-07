# Install
1. onnxruntime libraryfile Download
1. 의존성 추가
```
# https://github.com/microsoft/vcpkg.git vcpkg 설치 및 VCPKG_ROOT환경변수 등록

vcpkg install utfcpp
vcpkg install protobuf
```
1. Build
```
git submodule init
git submodule update
uv sync
uv build --wheel
```

# USAGE
```
>>> from htmorphs import HeadTailCPP()
>>> ht = HeadTailCPP()
>>> ht.pos(["나는 너가 정말 보고 싶었어."])
['나/NP+는/Josa 너/NP+가/Josa 정말/MAG 보/VV+고/Eomi 싶/VX+었어/Eomi']
```