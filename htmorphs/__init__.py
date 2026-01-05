import os
import pickle
import copy
import sys
import htmorphs
import subprocess
from pathlib import Path
from htmorphs.ahocorasic import AC
path = Path(__file__).parent
#print(str(path) not in os.environ["LD_LIBRARY_PATH"])
if sys.platform.startswith('linux') and str(path) not in os.environ["LD_LIBRARY_PATH"]:
    cmd = f"source {str(path)}/run.sh {str(path)}"
    #print(cmd)
    #a=subprocess.run(cmd,shell=True,stdout=sys.stdout,executable="/bin/bash")
    #print(a)
    print("SET ENV REQUIRED")
    print(f"echo \"export LD_LIBRARY_PATH=\"{str(path)}:$LD_LIBRARY_PATH\"\" >> ~/.bashrc")
    print("source ~/.bashrc")
    exit()
sys.modules["customutils"] = htmorphs
#os.environ["LD_LIBRARY_PATH"] = str(path)+":"+os.environ.get("LD_LIBRARY_PATH","")
from ._htmorphs import *

os.environ["HTCPP"] = os.path.join(str(path),"..")
def xstag_readdic(path):
    vdic = []
    with open(path,"rb") as f:
        vdic.append(pickle.load(f))
        vdic.append(pickle.load(f))
    # NXMP1902008040.json_xsv.pkl
    return vdic

def vstag_readdic(path):
    with open(path,"rb") as f:
        oridic = pickle.load(f)
    
    return oridic

dictpath = os.path.join(os.environ["HTCPP"],"htmorphs")
xsn_dict = xstag_readdic(os.path.join(dictpath,"headtail_xsn.pkl"))
xsa_dict = xstag_readdic(os.path.join(dictpath,"headtail_xsa.pkl"))
xsv_dict = xstag_readdic(os.path.join(dictpath,"headtail_xsv.pkl"))
nvv_dict = xstag_readdic(os.path.join(dictpath,"headtail_nvv.pkl"))
nva_dict = xstag_readdic(os.path.join(dictpath,"headtail_nva.pkl"))
ovv_dict = vstag_readdic(os.path.join(dictpath,"headtail_vv.pkl"))
ova_dict = vstag_readdic(os.path.join(dictpath,"headtail_va.pkl"))
    # headdict = AC()


def get_mp_tags(texts):
    heads = []
    tails = []
    for idx,text in enumerate(texts.split("+")):
        mp = text.split("/")
        # print(text)
        if idx == 0:
            heads.append(mp[0])
            heads.append(mp[1])
        else:
            tails.append(mp[0])
            tails.append(mp[1])
    
    return heads,tails

# filepath = os.path.join(path,"tok-model-un")
def get_dict_value(df):
    return df[0]

def get_dict_nvvalue(value):
    valuetmp = value.split("+")
    if len(valuetmp) >= 2:
        if valuetmp[1].startswith("을") or valuetmp[1].startswith("는") or valuetmp[1].startswith("를"):
            return valuetmp[0]
    return value.replace(" @@@ ","/")

def xstag_join(head,tail,lastnum,htres,orimode=True,tag="NOUN"):
    head_ = head
    # print(1,head_,head)
    if len(head_[0]) > lastnum * -1:
        xstk = ""
        if len(head_[0]) > 1:
            if orimode:
                # print(htres)
                xstk = htres
            else:
                # if len()
                # print(lastnum)
                xstk = head_[0][lastnum:]+"/"+htres.split("+")[0].split("/")[1]
        # print(xstk)
        head_[0] = head_[0][:lastnum] + f"/{tag}+" + xstk
        
        if len(tail) > 0:
            res = head_[0] + "+" + "/".join(tail)
            return res
        else:
            # print(head_)
            return head_[0]#"/".join(head_)
    # print(head_)
    # print(2,head_,head)
    return "/".join(head_)

def nvtag_getlast_search(vdf,hlast):
    # print(2222,hlast)
    flag,res = vdf.find(hlast)
    
    return flag

def get_tag(headvnoun):
    # tag = headdict.find(headvnoun)
    # print(tag)
    # if len(tag) > 0:
    #     tag = list(tag)
    #     return tag[0]
    return "NOUN"

def nv_get_res(orimode,headv,tagname):
    if orimode and tagname == "VV":
        headv = get_vv(headv+f"/{tagname}")
        # headv,tail = get_mp_tags(headv)
        # headv = headv[0]
        return headv
        
    elif orimode and tagname == "VA":
        headv = get_va(headv+f"/{tagname}")
        # headv,tail = get_mp_tags(headv)
        # headv = headv[0]
        return headv
    
    return f"{headv}/{tagname}"

def nvtag_join(head,tail,lastnum,htres,vdf,tagname,orimode=True,tag="NOUN"):
    head_ = head
    head_[0] = list(head_[0])
    head_[0][lastnum] = htres 
    head_[0] = "".join(head_[0])
    
    honelast = xstag_getlast(head,-1)
    htwolast = xstag_getlast(head,-2)
    hthreelast = xstag_getlast(head,-3)
    
    oneres = nvtag_getlast_search(vdf,honelast)
    twores = nvtag_getlast_search(vdf,htwolast)
    threeres = nvtag_getlast_search(vdf,hthreelast)
    # print(oneres)
    # print(twores)
    # print(threeres)
    # print(oneres,twores,threeres,htres,head[0])
    res = ""
    # print(head_)
    nvheadtk = head_[0].split("/")[0]
    # print(nvheadtk)
    # print(nvheadtk)
    if threeres and len(nvheadtk) > 3:
        headvnoun = xstag_getvnoun(head_,-3)
        # print(headvnoun)
        headv = xstag_getlast(head_,-3)
        # if tagname == "VA":
        #     tag = tag#get_tag(headvnoun)#"XR"
        # else:
        #     tag = tag#get_tag(headvnoun)
        # print(headv)
        if "/" in htres:
            res = headvnoun + f"/{tag}+"+nv_get_res(orimode,headv,tagname)+"+"+htres.split("+")[-1]
            # res = headvnoun + f"/{tag}+"+headv+f"/{tagname}"+"+"+htres.split("+")[-1]
        else:
            res = headvnoun + f"/{tag}+"+nv_get_res(orimode,headv,tagname)#headv+f"/{tagname}"
            # res = headvnoun + f"/{tag}+"+headv+f"/{tagname}"
    elif twores and len(nvheadtk) > 2:
        headvnoun = xstag_getvnoun(head_,-2)
        headv = xstag_getlast(head_,-2)
        # print(333,htres)
        # print(headvnoun)
        # tag = get_tag(headvnoun)
        # if tagname == "VA":
        #     tag = tag#get_tag(headvnoun)#"XR"
        # else:
        #     tag = tag#get_tag(headvnoun)
        
        if "/" in htres:
            res = headvnoun + f"/{tag}+"+nv_get_res(orimode,headv,tagname)+"+"+htres.split("+")[-1]
            # res = headvnoun + f"/{tag}+"+headv+f"/{tagname}"+"+"+htres.split("+")[-1]
        else:
            res = headvnoun + f"/{tag}+"+nv_get_res(orimode,headv,tagname)#headv+f"/{tagname}"
            # res = headvnoun + f"/{tag}+"+headv+f"/{tagname}"
    elif oneres and len(nvheadtk) > 1 :
        headvnoun = xstag_getvnoun(head_,-1)
        headv = xstag_getlast(head_,-1)
        # print(headv)
        # print(1,headv)
        # print(2222,headvnoun,headv)
        
        # tag = get_tag(headvnoun)
        if "/" in htres:
            res = headvnoun + f"/{tag}+"+nv_get_res(orimode,headv,tagname)+"+"+htres.split("+")[-1]
            # res = headvnoun + f"/{tag}+"+headv+f"/{tagname}"+"+"+htres.split("+")[-1]
        else:
            res = headvnoun + f"/{tag}+"+nv_get_res(orimode,headv,tagname)
            # res = headvnoun + f"/{tag}+"+headv+f"/{tagname}"
        
    if len(tail) > 0 and res.strip() != "":
        # print(2222,res,tail)
        # print(1,res)
        res = res + "+" + "/".join(tail)
        return res
    else:
        # print(2,res)
        return res
    
def vtag_join(head,tail,htres,vdf):
    head_ = head
    # head_[0] = list(head_[0])
    # head_[0][-1] = htres
    # head_[0] = "".join(head_[0])
    # print(head_)
    # headtmp = head_[0].split("+")[0].split("/")
    # print(headtmp)
    vhead = htres.split("+")[0].split("/")[0]
    # print(head_[0][:-1]+vhead)
    dictres = nvtag_getlast_search(vdf,head_[0][:-1]+vhead)
    # print(dictres)
    # print(11,vhead,htres)
    # print(vhead,head_[0],head[0])
    if dictres:
        # htrestmp = htres.split("+")[1:]
        # htrestmp = "+".join(htrestmp)
        # print(htres)
        if len(tail) > 0:
            return head_[0][:-1]+htres + "+" + "/".join(tail)
        else:
            return head_[0][:-1]+htres

    return ""

def xstag_getvnoun(head,lastnum):
    headtmp = head[0].split("/")[0]
    headvnoun = headtmp[:lastnum]
    return headvnoun

def xstag_getlast(head,lastnum):
    headtmp = head[0].split("/")[0]
    headlast = headtmp[lastnum:]
    # print(headlast)
    return headlast

def xstag_getlast_search(df,word):
    flag,res = df[0].find(word)
    if flag:
        res_ = []
        for w in res:
            # print(df[1][w])
            res_.append(df[1][w].replace(" @@@ ","/"))
        # print(res_)
        return res_
    else:
        return list()

def get_nvv(hteoj,vvmode=True,tag="NOUN"):
    head,tail = get_mp_tags(hteoj)
    
    headtmp = copy.deepcopy(head)
    hlastone = xstag_getlast(head,-1)
    
    onedict = xstag_getlast_search(nvv_dict,hlastone)#xsn_dict.query(f"ori==\"{hlastone}\"")
    # print(onedict)
    ovvres = nvtag_join(head,tail,-1,hlastone,ovv_dict,"VV",tag=tag)
    # print(111,ovvres,onedict)
    # print(ovvres)
    if ovvres != "":# or vvmode:
        # print(ovvres)
        hovvtmp = ovvres.split("+")[1].split("/")
        if vvmode and hovvtmp[0] != head[0][-len(hovvtmp[0]):]: 
            # print(ovvres)
            return ovvres
        # print(headvvtmp)
        # print(headtmp)
        # print(hovvtmp)
        headtmp[0] = headtmp[0].split("/")[0]
        # print(1,hovvtmp)
        # print(headtmp[0])
        return headtmp[0][:-len(hovvtmp)+1]+f"/{tag}+"+headtmp[0][-len(hovvtmp[0]):] +"/VV"
        return ovvres
    
    # print(onedict)
    for one in onedict:
        # print(one)
        # if len(one.split("+")).startswith()
        htres = get_dict_nvvalue(one)
        # print(htres)
        # print(htres)
        ovvres = nvtag_join(head,tail,-1,htres,ovv_dict,"VV",tag=tag)
        # print(ovvres)
        # print(headtmp)
        # print(ovvres)
        if ovvres == "":
            return hteoj
        else:
            hovvtmp = ovvres.split("+")[1].split("/")
            # print(ovvres)
            if vvmode and hovvtmp[0] != head[0][-len(hovvtmp[0]):]: 
                return ovvres
            # print(ovvres)
            headtmp[0] = headtmp[0].split("/")[0]
            # print(hovvtmp)
            # print(headtmp[0])
            # print(2,hovvtmp)

            return headtmp[0][:-len(hovvtmp[0])]+f"/{tag}+"+headtmp[0][-len(hovvtmp[0]):] +"/VV"
    if len(tail) > 0:
        # print(headtmp)
        # print(tail)
        return headtmp[0]+"/VV+"+tail[0]+"/"+tail[1]
    else:
        return headtmp[0]+"/VV"
    # return hteoj
def head_eq(head,htres):
    if head[0] == htres.split(" @@@ ")[0]:
        return True
    return False
def get_nva(hteoj,vamode=True,tag="NOUN"):
    head,tail = get_mp_tags(hteoj)
    headtmp = copy.deepcopy((head))
    hlastone = xstag_getlast(head,-1)
    # print(head,tail)
    onedict = xstag_getlast_search(nva_dict,hlastone)#xsn_dict.query(f"ori==\"{hlastone}\"")
    
    ovvres = nvtag_join(head,tail,-1,hlastone,ova_dict,"VA",tag=tag)
    # print(onedict)
    if ovvres != "":
        # ovvres = ovvres.split("+")[:-1]
        # ovvres = "+".join(ovvres)
        # print(ovvres)
        hovvtmp = ovvres.split("+")[1].split("/")
        if vamode and hovvtmp[0] != head[0][-len(hovvtmp[0]):]: 
            return ovvres
        # print(headvvtmp)
        # print(headtmp)
        # print(hovvtmp)
        headtmp[0] = headtmp[0].split("/")[0]
        # print(headtmp)
        # print(hovvtmp)
        return headtmp[0][:-len(hovvtmp[0])]+f"/{tag}+"+headtmp[0][-len(hovvtmp[0]):] +"/VA"
        return ovvres
    for one in onedict:

        htres = get_dict_nvvalue(one)
        # print(1111,htres)
        ovvres = nvtag_join(head,tail,-1,htres,ova_dict,"VA",tag=tag)
        if ovvres != "":
            hovvtmp = ovvres.split("+")[1].split("/")
            if False and hovvtmp[0] != head[0][-len(hovvtmp[0]):]: 
                return ovvres
            headtmp[0] = headtmp[0].split("/")[0]
            
            return headtmp[0][:-len(hovvtmp[0])]+f"/{tag}+"+headtmp[0][-len(hovvtmp[0]):] +"/VA"
    if len(tail) > 0:
        return head[0]+"/VA+"+tail[0]+"/"+tail[1]
    else:
        return head[0]+"/VA"
    # return hteoj

def get_vv(hteoj):
    head,tail = get_mp_tags(hteoj)
    hlastone = xstag_getlast(head,-1)
    
    onedict = xstag_getlast_search(nvv_dict,hlastone)#xsn_dict.query(f"ori==\"{hlastone}\"")
    # print(onedict)
    # ovvres = vtag_join(head,tail,hlastone,ovv_dict)
    # # print(ovvres)
    # if ovvres != "":
    #     return ovvres
    # print(onedict)
    # return hteoj
    # if len(head[0]) == 1:
    # return hteoj
    for one in onedict:
        htres = get_dict_nvvalue(one)
        ovvres = vtag_join(head,tail,htres,ovv_dict)
        # print(ovvres)
        if ovvres != "":
            # print(ovvres)
            hovvtmp = ovvres.split("+")[0].split("/")
            if hovvtmp[0] != head[0][-len(hovvtmp[0]):]: 
                return ovvres
        
    return hteoj

def get_va(hteoj):
    head,tail = get_mp_tags(hteoj)
    hlastone = xstag_getlast(head,-1)
    
    onedict = xstag_getlast_search(nva_dict,hlastone)#xsn_dict.query(f"ori==\"{hlastone}\"")
    
    # ovvres = vtag_join(head,tail,hlastone,ova_dict)
    # if ovvres != "":
    #     return ovvres
    # return hteoj
    # if len(head[0]) == 1:
    #     return hteoj
    for one in onedict:
        htres = get_dict_nvvalue(one)
        ovvres = vtag_join(head,tail,htres,ova_dict)
        if ovvres != "":
            hovvtmp = ovvres.split("+")[0].split("/")
            if hovvtmp[0] != head[0][-len(hovvtmp[0]):]: 
                return ovvres
        
    return hteoj

def get_xsn(hteoj,orimode,tag="NOUN"):
    head,tail = get_mp_tags(hteoj)
    honeres, htwores = "", "" 
    hlastone = xstag_getlast(head,-1)#head[0][-1]
    hlasttwo = xstag_getlast(head,-2)#head[0][-2:]
    
    onedict = xstag_getlast_search(xsn_dict,hlastone)#xsn_dict.query(f"ori==\"{hlastone}\"")
    if len(onedict) > 0:
        honeres = get_dict_value(onedict)
    
    twodict = xstag_getlast_search(xsn_dict,hlasttwo)#xsn_dict.query(f"ori==\"{hlasttwo}\"")
    if len(twodict) > 0:
        htwores = get_dict_value(twodict)
        
    if htwores != "":
        res = xstag_join(head,tail,-2,htwores,orimode,tag=tag)
        if res != "":
            return res
    elif honeres != "":
        res = xstag_join(head,tail,-1,honeres,orimode,tag=tag)
        if res != "":
            return res
    if len(tail) > 0:
        return head[0]+f"/{tag}+"+tail[0]+"/"+tail[1]
    else:
        return head[0]+f"/{tag}"
    # return hteoj

def get_xsa(hteoj,orimode,tag="NOUN"):
    head,tail = get_mp_tags(hteoj)
    honeres, htwores = "", "" 
    hlastone = xstag_getlast(head,-1)#head[0][-1]
    hlasttwo = xstag_getlast(head,-2)#head[0][-2:]
    
    onedict = xstag_getlast_search(xsa_dict,hlastone)#xsn_dict.query(f"ori==\"{hlastone}\"")
    if len(onedict) > 0:
        honeres = get_dict_value(onedict)
    
    twodict = xstag_getlast_search(xsa_dict,hlasttwo)#xsn_dict.query(f"ori==\"{hlasttwo}\"")
    if len(twodict) > 0:
        htwores = get_dict_value(twodict)
        
    if htwores != "":
        res = xstag_join(head,tail,-2,htwores,orimode,tag=tag)
        if res != "":
            return res
    elif honeres != "":
        res = xstag_join(head,tail,-1,honeres,orimode,tag=tag)
        if res != "":
            return res
    if len(tail) > 0:
        return head[0]+f"/{tag}+"+tail[0]+"/"+tail[1]
    else:
        return head[0]+f"/{tag}"
    # return hteoj

def get_xsv(hteoj,orimode,tag="NOUN"):
    head,tail = get_mp_tags(hteoj)
    honeres, htwores = "", "" 
    hlastone = xstag_getlast(head,-1)#head[0][-1]
    hlasttwo = xstag_getlast(head,-2)#head[0][-2:]
    
    onedict = xstag_getlast_search(xsv_dict,hlastone)#xsn_dict.query(f"ori==\"{hlastone}\"")
    if len(onedict) > 0:
        honeres = get_dict_value(onedict)
    
    twodict = xstag_getlast_search(xsv_dict,hlasttwo)#xsn_dict.query(f"ori==\"{hlasttwo}\"")
    if len(twodict) > 0:
        htwores = get_dict_value(twodict)
        
    if htwores != "":
        res = xstag_join(head,tail,-2,htwores,orimode,tag=tag)
        if res != "":
            return res
    elif honeres != "":
        res = xstag_join(head,tail,-1,honeres,orimode,tag=tag)
        if res != "":
            return res
    if len(tail) > 0:
        return head[0]+"/VV+"+tail[0]+"/"+tail[1]
    else:
        return head[0]+"/VV"
    # return hteoj
def get_vcp(hteoj,orimode,tag="NOUN"):
    head,tail = get_mp_tags(hteoj)
    lastone = head[0].strip()[-1]

    if lastone in ["이","였","일","인","입"]:
        if len(head[0]) > 1:
            if not orimode:
                return head[0][:-1] + f"/{tag}+" + lastone + "/VCP"
            else:
                return head[0][:-1] + f"/{tag}+" + "이/VCP"
        else:
            if orimode:
                return "이/VCP"
            else:
                return hteoj
    elif len(head[0]) > 4 and head[0].strip()[-4:] in ["이시거나","였습니다"]:
      return head[0][:-4] + f"/{tag}+" + head[0][-4:] + "/Eomi"
    elif len(head[0]) > 3 and head[0].strip()[-3:] in ["습니다","입니다","합시다","봅시다","하시고","이라고"]:
        return head[0][:-3] + f"/{tag}+" + head[0][-3:] + "/Eomi"
    elif len(head[0]) > 2 and head[0].strip()[-2:] in ["고요","바랍","라고","이요","이기","이라","하고","이란","니다","마다","지만"]:
        return head[0][:-2] + f"/{tag}+" + head[0][-2:] + "/Eomi"

    elif len(head[0]) > 1 and lastone in [
        "다"  
        ,"라"
        ,"며"
        ,"였"
        ,"란"
        ,"일"
        ,"가"
        ,"입"
        ,"든"
        ,"임"
        ,"겠"
        ,"여"
        ,"면"
        ,"요"
        ,"지"
        ,"건"
        ,"냐"
        ,"니"
        ,"고"
        ,"예"
        ,"야"
        ,"죠"
        ,"데"
        ,"구"
        ,"까"
        ,"랄"
        ,"거"
        ,"래"
        ,"잖"
        ,"로"
    ] + ["랍","시","자","다","라","고","나","니","가","면","죠","란","요","지"]:
        return head[0][:-1] + f"/{tag}+" + head[0][-1:] + "/Eomi"
            
            
    return hteoj

def get_vcn(hteoj,orimode,tag="NOUN"):
    head,tail = get_mp_tags(hteoj)
    lastone = head[0][-2:]

    if lastone in ["아니","아냐","아닌","아녔","아닙","아닌","아닙","아님","아닐","아냐","아녜"]:
        if len(head[0]) > 2:
            if not orimode:
                return head[0][:-1] + f"/{tag}+" + lastone + "/VCN"
            else:
                return head[0][:-1] + f"/{tag}" + "아니/VCN"
        elif len(head[0]) == 2:
            if orimode:
                return "아니/VCN"
            else:
                return hteoj
    return hteoj
def get_xstags(hteoj,orimode=False,line=None):
    # print(hteoj)
    # return hteoj
    try:
        if "NNPXSN" in hteoj:# or ("NOUN" in hteoj and len(hteoj) > (5+1)):# or "NOUN" in hteoj:
            # print(hteoj)
            return get_xsn(hteoj,orimode,tag="NNP")
        elif "NNGXSN" in hteoj:
            return get_xsn(hteoj,orimode,tag="NNG")
        elif "NNPXSV" in hteoj:
            return get_xsv(hteoj,orimode,tag="NNP")
        elif "NNGXSV" in hteoj:
            return get_xsv(hteoj,orimode,tag="NNG")
        elif "XSV" in hteoj:
            return get_xsv(hteoj,orimode,tag="XR")
        elif "NNPXSA" in hteoj:
            return get_xsa(hteoj,orimode,tag="NNP")
        elif "NNGXSA" in hteoj:
            return get_xsa(hteoj,orimode,tag="NNG")
        elif "XSA" in hteoj:
            # print(get_xsa(hteoj))
            return get_xsa(hteoj,orimode,tag="XR")
        elif "NNGVV" in hteoj:
            # print(hteoj)
            return get_nvv(hteoj,vvmode=orimode,tag="NNG")
        elif "NNPVV" in hteoj:
            return get_nvv(hteoj,vvmode=orimode,tag="NNP")
        
        elif "NNGVA" in hteoj:
            # print(hteoj)
            return get_nva(hteoj,vamode=orimode,tag="NNG")
        elif "NNPVA" in hteoj:
            return get_nva(hteoj,vamode=orimode,tag="NNP")
        elif "NNPVCP" in hteoj:
            return get_vcp(hteoj,orimode,tag="NNP")
        elif "NNGVCP" in hteoj:
            return get_vcp(hteoj,orimode,tag="NNG")
        elif "VCP" in hteoj:
            return get_vcp(hteoj,orimode,tag="NP")
        elif "NNPVCN" in hteoj:
            return get_vcn(hteoj,orimode,tag="NNP")
        elif "NNGVCN" in hteoj:
            return get_vcn(hteoj,orimode,tag="NNG")
        elif "VCN" in hteoj:
            return get_vcn(hteoj,orimode,tag="NP")
    except Exception as ex:
        print(ex)
        print(hteoj)
        if line:
            print(line)
        exit()
    
    # elif "VA" in hteoj:
    #     return get_nva(hteoj,vamode=orimode)
    # elif "VV" in hteoj:
    #     # print(2,hteoj)
    #     res = get_vv(hteoj)
    #     restmph,restmpt = get_mp_tags(hteoj)
    #     # print(res)
    #     # print(res,hteoj)
    #     # print(3,res)
    #     if hteoj == res and not ovv_dict.find(restmph[0])[0]:
    #         res = get_nvv(hteoj,vvmode=orimode)
    #         # print(res,hteoj)
    #         # print(get_nvv(hteoj))
    #     # print(4,res)
    #         return res
    #     else:
    #         if orimode:
    #             return res
    #         else:
    #             return hteoj
    # elif "VA" in hteoj:
    #     res = get_va(hteoj)
    #     restmph,restmpt = get_mp_tags(hteoj)
        
    #     # print(res,hteoj)
    #     # print(3,res)
    #     if hteoj == res and not ova_dict.find(restmph[0])[0]:
    #         res = get_nva(hteoj,vamode=orimode)
    #         # print(res,hteoj)
    #         # print(get_nva(hteoj))
    #         return res
    #     else:
    #         if orimode:
    #             return res
    #         else:
    #             return hteoj
    
    return hteoj


class HeadTailCPP:
    def __init__(self):
        self.httagger = HTMorph(6)
        # self.tokenizer = HTTokenizer(os.path.join(path,"tok-model-uint8.onnx"),os.path.join(path,"httokenizer.bin"))
        # self.postagger = HTPostagger(os.path.join(path,"pos-model-uint8.onnx"),os.path.join(path,"htpostagger.bin"),os.path.join(path,"tokenizer","tokenizer.json"))
    
    def pos(self,x):
        posres = self.httagger.tokenizer(x)
        posres = self.splitori(posres)
        return posres
    
    def splitori(self,x):
        import re
        result = []
        for res in x:
            tmp = []
            res = re.sub(r"\s+/(N|V|E|M|J)",r"/\1",res)
            for res_ in res.split():
                if "+" in res_:
                    res_ = res_.split("+")
                    res_[0] = get_xstags(res_[0],line=res)
                    res_ = "+".join(res_)
                    
                else:
                    res_ = get_xstags(res_,line=res)
                tmp.append(res_)
            result.append(" ".join(tmp))
        
        return result
