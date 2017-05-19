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
        vector< vector<double> > w_vertex;
        vector< vector<double> > w_context;

        // data function
        void LoadEdgeList(string);
        void LoadItemConceptList(string);
        void SaveWeights(string);
        
        // model function
        void Init(int);
        void Train(int, int, double, int);

};


#endif
