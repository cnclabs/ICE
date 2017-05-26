# distutils: language = c++

from libcpp.unordered_map cimport unordered_map
from libcpp.string cimport string
from libcpp.vector cimport vector
from cpython cimport array
from libcpp cimport bool

"""
[ICE]

"""

cdef extern from "./ICE.h":

    cdef cppclass ICE:

        ICE() except +

        # graph function
        void LoadEdgeList(string)
        void LoadDict(unordered_map[string, unordered_map[string, double]])
        void SaveWeights(string)

        # model function
        void Init(int)
        void Train(int, int, double, int)

cdef class pyICE:
    
    cdef ICE *c_ICE
    
    def __cinit__(self):
        self.c_ICE = new ICE()

    def __dealloc__(self):
        if self.c_ICE != NULL:
            del self.c_ICE

    def init(self, dimension=64):
        self.c_ICE.Init(dimension)
    
    def load_edgelist(self, filename):
        self.c_ICE.LoadEdgeList(filename.encode('utf-8'))

    def load_dict(self, _graph):
        
        cdef unordered_map[string, unordered_map[string, double]] graph
        for v1 in _graph:
            for v2 in _graph[v1]:
                graph[v1.encode('utf-8')][v2.encode('utf-8')] = _graph[v1][v2]

        self.c_ICE.LoadDict(graph)

    def save_weights(self, model_name='ICE.model'):
        self.c_ICE.SaveWeights(model_name.encode('utf-8'))

    def train(self, sample_times=10, negative_samples=5, alpha=0.025, workers=1):
        self.c_ICE.Train(sample_times, negative_samples, alpha, workers)


