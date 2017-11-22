#ifndef VVNET_H
#define VVNET_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <random>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <utility>
#include <stdio.h>
#include <omp.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

#define MONITOR 10000
#define POWER_SAMPLE 0.75
#define HASH_TABLE_SIZE 30000000
#define SIGMOID_TABLE_SIZE 1000
#define MAX_SIGMOID 8.0
#define MAX_NEG 1e8

#if defined (_MSC_VER)  // Visual studio
    #define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
    #define thread_local __thread
#endif

class Vertex {
    public:
        long offset, branch;
        double out_degree;
        double in_degree;
        Vertex() { out_degree=0.0; in_degree=0.0; }
};

// add flag
class Context {
    public:
        long vid;
        double in_degree;
        Context() { vid=-1; in_degree=0.0; }
};

class AliasTable {
    public:
        int alias;
        double prob;
        AliasTable() { alias=-1; prob=0.0; }
};

class vvNet {

    public:
        
        vvNet();
        ~vvNet();
        
        // MAX index number
        unsigned long long MAX_line;
        long MAX_vid;
        unsigned long long MAX_edge;
        
        // graph basics
        vector< long > hash_table;
        vector< string > keys;
        map< string, long > kmap;     // vertex map to index number

        // Alias Graph
        vector< Vertex > vertex;
        vector< Context > context;
        vector< AliasTable > vertex_AT;
        vector< AliasTable > context_AT;
        vector< AliasTable > negative_AT;

        unordered_map< long, unordered_map<long, double> > vv;
        
        // cahce
        vector< double > cached_sigmoid;
        void InitSigmoid();
        void BuildAliasMethod(unordered_map<long, unordered_map<long, double>>&);
        void BuildNegativeAliasTable();
        void BuildSourceAliasTable();
        void BuildTargetAliasTable();
        
        // Key Process
        unsigned int BKDRHash(string&);
        int InsertHashTable(string);
        int SearchHashTable(string);
        
        // Math-related Process
        double fastSigmoid(double);

        // Data Process
        void LoadEdgeList(string);
        void LoadItemConceptList(string);
        void LoadWeights(string, vector< vector<double> > &);
        void LoadDict(unordered_map<string, unordered_map<string, double>>&);
        
        // Network Process
        long SourceSample();
        long TargetSample();
        long TargetSample(long);
        long NegativeSample();

        // Optimizer
        
        // vertex vector, context vector, vertex, context, dimension, negative samples, alpha
        void UpdatePair(vector< vector<double> >&, vector< vector<double> >&, long, long, int, int, double);

        // vertex vector, context vector, vertex, context, dimension, negative samples, alpha
        void UpdateVertex(vector< vector<double> >&, vector< vector<double> >&, long, long, int, int, double);
        void UpdateContext(vector< vector<double> >&, vector< vector<double> >&, long, long, int, int, double);
               
        // vertex vector, context vector, vertex, context, dimension, negative samples, community walk steps, alpha
        void UpdateCommunity(vector< vector<double> >&, vector< vector<double> >&, long, long, int, int, int, double);

};

#endif
