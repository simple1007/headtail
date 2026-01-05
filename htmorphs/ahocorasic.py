from queue import Queue
from collections import defaultdict

class Node(dict):
    def __init__(self):
        super().__init__()
        self.final = False
        self.value = set()
        
        self.out = set()
        self.fail = None
        
    def addout(self,out):
        if type(out) is set:
            self.out = self.out.union(out)
        else :
            self.out.add(out)
    
    def addchild(self,alphabet,node = None):
        self[alphabet] = Node() if node is None else node

import re
from collections import deque

class AC():
    def __init__(self):
        # self.patterns = patterns
        self.head = Node()
        
        # self.maketrie()
        # self.constructfail()

    def dfs_recursive(self,graph, current, target, visited, path, all_paths):

        # 현재 노드를 방문 처리
        visited.add(current)  # 현재 노드 방문
        path.append(current)  # 현재 노드를 경로에 추가

        # 목표 노드에 도달했을 경우 경로를 저장
        if current == target:
            all_paths.append(path.copy())
        else:
            # 인접 노드들을 탐색
            for neighbor in graph[current]:
                if neighbor not in visited:
                    self.dfs_recursive(graph, neighbor, target, visited, path, all_paths)

        # 탐색이 끝난 후 현재 노드를 경로와 방문 목록에서 제거 (백트래킹)
        visited.remove(current)
        path.pop()
        
        # return all_paths

    def bfs_path(self,graph, start, end):
        visited = set()  # 방문한 노드를 추적
        queue = deque([[start]])  # 큐에 경로를 넣음 (시작 노드부터)
        visited.add(start)  # 시작 노드를 방문한 것으로 표시
        
        while queue:
            path = queue.popleft()  # 큐에서 경로를 꺼냄
            node = path[-1]  # 현재 경로의 마지막 노드
            
            # 끝 노드를 찾으면 그 경로를 반환
            if node == end:
                return path
            
            # 현재 노드의 이웃 노드를 큐에 추가
            for neighbor in graph[node]:
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append(path + [neighbor])  # 경로를 확장하여 큐에 추가
        
        return None  # 경로가 없으면 None을 반환
    def find(self,pattern):
        crr = self.head
        # print()
        for c in pattern:
            if c in crr:
                # crr.addchild(c)
                crr = crr[c]
            else:
                return False,None
                
        return True,crr.value
        
    def search(self,sentence,dfs=True,import_noword=True):
        crr = self.head
        ret = defaultdict(list)
        countflag = defaultdict(int)
        sent = sentence
        
        for idx,c in enumerate(sentence):
            while crr is not self.head and c not in crr:
                crr = crr.fail
            if c in crr:
                crr = crr[c]
            # print(c,crr)
            if crr.final:
                # print(idx-1,crr.out)
                for a in crr.out:
                    # print(a)
                    p = re.compile(r"("+a+r")")
                    m = p.search(sent,pos=countflag[a])
                    if m:
                        rep = "".join(["-" for i in range(len(a))])
                        span = m.span()
                        
                        a = (a,span[0],span[1])
                        countflag[a[0]] = a[2]
                        # ret.append(a)
                        ret[a[1]].append(a)
        graph = defaultdict(list)
        cnt = -1
        # print(ret)
        # exit()
        
        if import_noword:
            for i in range(len(sentence)):
                if i not in ret:
                    for j in range(i,len(sentence)):
                        if j in ret:
                            break
                        # print(j)
                        # print(sentence[i:j+1])
                        ret[i].append((sentence[i:j+1],i,j+1))
        
        # print(ret)
        for k,v in ret.items():
            for vv in v:
                graph[k].append(vv[2])
                cnt = max(cnt,vv[2])
        # print(ret)
        results = []
        if cnt == -1:
            return []
        # 모든 경로를 저장할 리스트
        all_paths = []

        # DFS 실행
        # start = 1  # 시작 노드
        # target = 7  # 목표 노드
        if dfs:
            self.dfs_recursive(graph, 0, cnt, set(), [], all_paths)

            # 결과 출력
            # print("모든 경로:")
            for path in all_paths:
                # print(path)
                tmp = []
                for p in range(len(path)-1):
                    tmp.append(sentence[path[p]:path[p+1]])
                # print()
                results.append(tmp)
        else:
            # print(1,ret)
            return ret    
        return results
    
    def maketrie(self,pattern,key=None):
        # for pattern in self.patterns:
        crr = self.head
        for c in pattern :
            if c not in crr:
                crr.addchild(c)
            crr = crr[c]
        crr.final = True
        if key != None:
            crr.value.add(key)
        crr.addout(pattern)
        # self.constructfail()
            
    def constructfail(self):
        queue = Queue()
        self.head.fail = self.head
        queue.put(self.head)
        while not queue.empty():
            crr = queue.get()
            for nextc in crr:
                child = crr[nextc]
                
                if crr is self.head:
                    child.fail = self.head
                else :
                    f = crr.fail
                    while f is not self.head and nextc not in f:
                        f = f.fail
                    if nextc in f:
                        f = f[nextc]
                    child.fail = f
                
                child.addout(child.fail.out)
                child.final |= child.fail.final
                
                queue.put(child)

if __name__ == "__main__":
    aho = AC()
    aho.maketrie("블루턱시도",1)
    aho.maketrie("블루",2)
    # aho.set_failur()
    aho.maketrie("턱시도",3)
    aho.maketrie("턱시도구",4)
    # aho.set_failur()
    aho.maketrie("구피",5)
    aho.maketrie("구피차",6)
    # aho.set_failur()
    aho.maketrie("알비노",7)
    # aho.set_failur()
    aho.maketrie("레드",8)
    aho.maketrie("삼성",9)
    aho.maketrie("구피",10)
    aho.maketrie("난태생",11)
    aho.maketrie("송사리과",12)
    # aho.maketrie("이다")
    # aho.constructfail()
    print(aho.find("구피"))
# # aho.set_failur()
# exit()
# class Node:
#     def __init__(self,text):
#         self.text = text
#         self.last = 0
#         self.output = ""
#         self.child = {}
#         self.fail = None
#         self.rank = 0
#     def show(self):
#         print( "text:", self.text, "output:",self.output, "fail:", self.fail.text if self.fail is not None else "None","rank:",self.rank)
#         if self.last:
#             print( "LAST OUTPUT : ", self.output)
#         for k in self.child.keys():
#             s = self.child[k]
#             s.show()
            
# class AhocorasickTrie:
    
#     def __init__(self):
#         self.no = 0
#         self.root = Node(None)
    
#     def add(self,text):
#         rank = 0
#         currentNode = self.root
#         for t in text:
#             if t not in currentNode.child:
#                 self.no +=1
#                 currentNode.child[t] = Node(t)
#                 print(f"ADD : {t} || Current No. : {self.no}")
#             currentNode.rank = rank
#             rank+=1
#             currentNode = currentNode.child[t]
#         currentNode.output = text
#         currentNode.last = 1
#         currentNode.rank = rank
#         print(f"TOTAL : {self.no}")
#         # self.set_failur()
        
#     def set_failur(self):
#         queue = []
#         for c,node in self.root.child.items():
#             node.fail = self.root
#             queue.append(node)
#         while queue:
#             currentNode = queue.pop(0)
#             for t,node in currentNode.child.items():
#                 fail_node = currentNode.fail
#                 while fail_node and t not in fail_node.child:
#                     fail_node = fail_node.fail
#                 if fail_node:
#                     node.fail = fail_node.child[t]
#                 else:
#                     node.fail = self.root
#                 queue.append(node)
    
#     def show(self):
#         self.root.display()  
        
#     def find(self,text):
#         result = []
#         currentNode = self.root
#         for t in text:
#             while currentNode is not None and t not in currentNode.child and currentNode.last > 0:
#                 # print("fail",currentNode.text)
#                 # if currentNode is None:
#                 #     break
#                 currentNode = currentNode.fail
#             # if currentNode is None:
#             #     break
#             if t in currentNode.child:
#                 currentNode = currentNode.child[t]
#                 if currentNode.output != "":
#                     result += [currentNode.output]
#                     # print(result,t,currentNode.output,currentNode.last,currentNode.output)
                    
#         return result
    
# aho = AhocorasickTrie()

# aho.add("블루턱시도")
# aho.add("블루")
# # aho.set_failur()
# aho.add("턱시도")
# # aho.set_failur()
# aho.add("구피")
# # aho.set_failur()
# aho.add("알비노")
# # aho.set_failur()
# aho.add("레드")
# # aho.set_failur()
# aho.set_failur()
# # aho.show()
# print(aho.find("블루턱시도구피"))
# exit()
# import ahocorasick

# aho = ahocorasick.Automaton()
# aho.add_word("블루",(0,"블루"))
# # aho.set_failur()
# aho.add_word("턱시도",(1,"턱시도"))
# # aho.set_failur()
# aho.add_word("구피",(2,"구피"))
# # aho.set_failur()
# aho.add_word("알비노",(3,"알비노"))
# # aho.set_failur()
# aho.add_word("레드",(4,"레드"))
# # aho.set_failur()
# aho.add_word("블루턱시도",(5,"블루턱시도"))
# # aho.set_failur()
# aho.make_automaton()
# # print(aho.get("블루턱시도"))
# for end_index, (insert_order, original_value) in aho.iter("블루턱시도구피"):
#     start_index = end_index - len(original_value) + 1
#     print((start_index, end_index, (insert_order, original_value)))
#     # assert haystack[start_index:start_index + len(original_value)] == original_value