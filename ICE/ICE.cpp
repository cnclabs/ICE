#include "ICE.h"
#include <omp.h>

ICE::ICE() {
}
ICE::~ICE() {
}

void ICE::LoadEdgeList(string filename) {
    vvnet.LoadEdgeList(filename);
}

void ICE::LoadItemConceptList(string filename) {
    vvnet.LoadItemConceptList(filename);
}

void ICE::LoadDict(unordered_map<string, unordered_map<string, double>>& graph) {
    vvnet.LoadDict(graph);
}

void ICE::LoadWeights(string filename){
    vvnet.LoadWeights(filename, w_context);
}

void ICE::SaveVertexWeights(string model_name){
    
    cout << "Save Model:" << endl;
    ofstream model(model_name);
    if (model)
    {
        //model << vvnet.MAX_vid << " " << dim << endl;
        for (auto k: vvnet.keys)
        {
            if (vvnet.vertex[vvnet.kmap[k]].branch > 0)
            {
                model << k;
                for (int d=0; d<dim; ++d)
                    model << " " << w_vertex[vvnet.kmap[k]][d];
                model << endl;
            }
        }
        cout << "\tSave to <" << model_name << ">" << endl;
    }
    else
    {
        cout << "\tfail to open file" << endl;
    }
}

void ICE::SaveContextWeights(string model_name){
    
    cout << "Save Model:" << endl;
    ofstream model(model_name);
    if (model)
    {
        //model << vvnet.MAX_vid << " " << dim << endl;
        for (auto k: vvnet.keys)
        {
            if (vvnet.vertex[vvnet.kmap[k]].branch > 0)
            {
                model << k;
                for (int d=0; d<dim; ++d)
                    model << " " << w_context[vvnet.kmap[k]][d];
                model << endl;
            }
        }
        cout << "\tSave to <" << model_name << ">" << endl;
    }
    else
    {
        cout << "\tfail to open file" << endl;
    }
}

void ICE::SaveEntityWeights(string model_name){
    
    cout << "Save Model:" << endl;
    ofstream model(model_name);
    int i = 0;
    if (model)
    {
        //model << vvnet.MAX_vid << " " << dim << endl;
        for (auto k: vvnet.keys)
        {
            // cout << k << endl;
            if (i < vocab_count){
                i++;
                continue;
            }
            if (vvnet.vertex[vvnet.kmap[k]].branch > 0)
            {
                model << k;
                for (int d=0; d<dim; ++d)
                    model << " " << w_vertex[vvnet.kmap[k]][d];
                model << endl;
            }
        }
        cout << "\tSave to <" << model_name << ">" << endl;
    }
    else
    {
        cout << "\tfail to open file" << endl;
    }
}

int ICE::getVocabCount(){
    vocab_count = vvnet.keys.size();
}

void ICE::Init(int dimension) {
   
    cout << "Model Setting:" << endl;
    cout << "\tdimension:\t\t" << dimension << endl;
    dim = dimension;

    w_vertex.resize(vvnet.MAX_vid);
    w_context.resize(vvnet.MAX_vid);

    for (long vid=0; vid<vvnet.MAX_vid; ++vid)
    {
        w_vertex[vid].resize(dim);
        for (int d=0; d<dim;++d)
        {
            w_vertex[vid][d] = (rand()/(double)RAND_MAX - 0.5) / dim;
        }
    }

    for (long vid=0; vid<vvnet.MAX_vid; ++vid)
    {
        w_context[vid].resize(dim);
        for (int d=0; d<dim;++d)
            w_context[vid][d] = 0.0;
    }
}


void ICE::Train(int sample_times, int negative_samples, double alpha, double alpha_min, int workers, int vocab_count, string mode){
    
    omp_set_num_threads(workers);

    cout << "Model:" << endl;
    cout << "\t[ICE]" << endl;

    cout << "Learning Parameters:" << endl;
    cout << "\tsample_times:\t\t" << sample_times << " (*Million)" << endl;
    cout << "\tnegative_samples:\t" << negative_samples << endl;
    cout << "\talpha:\t\t\t" << alpha << endl;
    cout << "\tworkers:\t\t" << workers << endl;

    cout << "Start Training:" << endl;

    unsigned long long total_sample_times = (unsigned long long)sample_times*1000000;
    double alpha_last, alpha_reduce;
    double _alpha = alpha;
    alpha_reduce = (alpha-alpha_min)/(total_sample_times/MONITOR);
    
    unsigned long long current_sample = 0;
    unsigned long long jobs = total_sample_times/workers;

    #pragma omp parallel for
    for (int worker=0; worker<workers; ++worker)
    {
        unsigned long long count = 1;
        long v1, v2;
        
        while (count<jobs)
        {            
            v1 = vvnet.SourceSample();
            v2 = vvnet.TargetSample(v1);

            if (mode == "stage1"){
                vvnet.UpdatePair(w_vertex, w_context, v1, v2, dim, negative_samples, _alpha);
            }
            else{
                if (v1 >= vocab_count){
                    vvnet.UpdateVertex(w_vertex, w_context, v1, v2, dim, negative_samples, _alpha);
                }
            }

            count++;
            if (count % MONITOR == 0)
            {
                _alpha -= alpha_reduce;
                current_sample += MONITOR;
                if (_alpha < alpha_min) _alpha = alpha_min;
                alpha_last = _alpha;
                printf("\tAlpha: %.6f\tProgress: %.3f %%%c", _alpha, (double)(current_sample)/total_sample_times * 100, 13);
                fflush(stdout);
            }
        }
    }
    printf("\tAlpha: %.6f\tProgress: 100.00 %%\n", alpha_last);

}

