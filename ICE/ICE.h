#ifndef ICE_H
#define ICE_H

#include "./vvNet.h"

/*****
 * ICE
 * **************************************************************/

class ICE {

    public:
        
        ICE();
        ~ICE();
        
        vvNet vvnet;

        // model parameters
        int dim;
        int vocab_count;
        vector< vector<double> > w_vertex;
        vector< vector<double> > w_context;

        // data function
        void LoadEdgeList(string);
        void LoadWeights(string);
        void LoadItemConceptList(string);
        void LoadDict(unordered_map<string, unordered_map<string, double>>&);
        void SaveVertexWeights(string);
        void SaveContextWeights(string);
        void SaveEntityWeights(string);
        int getVocabCount();
        
        // model function
        void Init(int);
        void Train(int, int, double, double, int, int, string);

};


#endif
