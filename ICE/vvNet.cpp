#include "vvNet.h"

double random_gen(const int & min, const int & max) {
    static random_device rd;
    static thread_local mt19937* generator = new mt19937( rd() );
    uniform_real_distribution<double> distribution(min, max);

    return distribution(*generator);
}

bool isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

/**
 * vvNet: vertex-to-vertex Network
 * **/

vvNet::vvNet() {
    MAX_edge = 0;
    MAX_vid = 0;
    MAX_edge = 0;
    hash_table.resize(HASH_TABLE_SIZE, -1);
    InitSigmoid();
}

vvNet::~vvNet() {
}

unsigned int vvNet::BKDRHash(string& key) {
    
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    for(size_t i = 0; i < key.length(); i++)
    {
        hash = (hash * seed) + key[i];
    }
    return (hash % HASH_TABLE_SIZE);
}

void vvNet::InitSigmoid() {

    cached_sigmoid.resize(SIGMOID_TABLE_SIZE);
    for (int i = 0; i != SIGMOID_TABLE_SIZE + 1; i++) {
        double x = i * 2.0 * MAX_SIGMOID / SIGMOID_TABLE_SIZE - MAX_SIGMOID;
        cached_sigmoid[i] = 1.0 / (1.0 + exp(-x));
    }
}

double vvNet::fastSigmoid(double x) {
    if (x < -MAX_SIGMOID) {
        return 0.0;
    } else if (x > MAX_SIGMOID) {
        return 1.0;
    } else {
        return cached_sigmoid[ int((x + MAX_SIGMOID) * SIGMOID_TABLE_SIZE / MAX_SIGMOID / 2) ];
    }
}

int vvNet::InsertHashTable(string key)
{
    unsigned int pos = BKDRHash(key);
    while (hash_table[pos] != -1)
        pos = (pos + 1) % HASH_TABLE_SIZE;
    hash_table[pos] = MAX_vid;
    kmap[ key ] = MAX_vid;
    keys.push_back( key );
    MAX_vid++;

    return MAX_vid-1;
}

int vvNet::SearchHashTable(string key)
{
    unsigned int pos = BKDRHash(key);
    while (1)
    {
        if (hash_table[pos] == -1)
            return -1;
        if ( key == keys[ hash_table[pos] ] )
            return hash_table[pos];
        pos = (pos + 1) % HASH_TABLE_SIZE;
    }
}

void vvNet::LoadEdgeList(string filename) {

    // calculate the total connections
    FILE *fin;
    char c_line[10000];
    vector< string > filenames;
    vector< int > filelines;
    
    // load from a folder or from a file
    if (isDirectory(filename.c_str()))
    {
        DIR *dir;
        struct dirent *ent;
        dir = opendir(filename.c_str());
        while ((ent = readdir (dir)) != NULL) {
            string fname = filename + "/" + ent->d_name;
            filenames.push_back(fname);
        }
        closedir(dir);
    }
    else
    {
        filenames.push_back(filename.c_str());
    }
    
    cout << "Preview:" << endl; 
    for (auto fname: filenames)
    {
        fin = fopen(fname.c_str(), "rb");
        while (fgets(c_line, sizeof(c_line), fin))
        {
            if (MAX_edge % MONITOR == 0)
            {
                printf("\t# of connection:\t%lld%c", MAX_edge, 13);
            }
            ++MAX_edge;
        }
        fclose(fin);
        filelines.push_back(MAX_edge);
    }
    cout << "\t# of connection:\t" << MAX_edge << endl;

    // load the connections
    char v1[160], v2[160];
    double w;
    long vid1, vid2;
    unordered_map< long, unordered_map<long, double> > vv;     // vertex -> vertex: weight
    
    cout << "Connections Loading:" << endl;
    unsigned long long line = 0;
    for (int i=0; i<filenames.size();i++)
    {

        fin = fopen(filenames[i].c_str(), "rb");
        for (; line != filelines[i]; line++)
        {
            if ( fscanf(fin, "%s %s %lf", v1, v2, &w) != 3 )
            {
                cout << "\t[ERROR] line " << line << " contains wrong number of column data" << endl; 
                continue;
            }

            // generate keys lookup table (kmap)
            vid1 = SearchHashTable(v1);
            if (vid1 == -1)
            {
                vid1 = InsertHashTable(v1);
            }
            vid2 = SearchHashTable(v2);
            if (vid2 == -1)
            {
                vid2 = InsertHashTable(v2);
            }
            
            vv[vid1][vid2] = w;
            MAX_edge += 1;
 
            if (line % MONITOR == 0)
            {
                printf("\tProgress:\t\t%.2f %%%c", (double)(line)/(MAX_edge+1) * 100, 13);
                fflush(stdout);
            }
        }
        fclose(fin);

    }
    cout << "\tProgress:\t\t100.00 %\r" << endl;
    cout << "\t# of vertex:\t\t" << MAX_vid << endl;
    
    cout << "Build the Alias Method:" << endl;
    BuildAliasMethod(vv);
    cout << "\tFinished." << endl;

}

void vvNet::LoadItemConceptList(string filename) {

    // calculate the total connections
    FILE *fin;
    char c_line[10000];
    vector< string > filenames;
    vector< int > filelines;
    
    // load from a folder or from a file
    if (isDirectory(filename.c_str()))
    {
        DIR *dir;
        struct dirent *ent;
        dir = opendir(filename.c_str());
        while ((ent = readdir (dir)) != NULL) {
            string fname = filename + "/" + ent->d_name;
            filenames.push_back(fname);
        }
        closedir(dir);
    }
    else
    {
        filenames.push_back(filename.c_str());
    }
    
    cout << "Preview:" << endl; 
    for (auto fname: filenames)
    {
        fin = fopen(fname.c_str(), "rb");
        while (fgets(c_line, sizeof(c_line), fin))
        {
            if (MAX_edge % MONITOR == 0)
            {
                printf("\t# of connection:\t%lld%c", MAX_edge, 13);
            }
            ++MAX_edge;
        }
        fclose(fin);
        filelines.push_back(MAX_edge);
    }
    cout << "\t# of connection:\t" << MAX_edge << endl;

    // load the connections
    char* item;
    char* concept;
    long vid_item, vid_concept;
    vector< long > concept_list;
    unordered_map< long, unordered_map<long, double> > ic;     // item -> concept: weight
    
    cout << "Connections Loading:" << endl;
    unsigned long long line = 0;
    for (int i=0; i<filenames.size();i++)
    {
        fin = fopen(filenames[i].c_str(), "rb");
        for (; line != filelines[i]; line++)
        {
            while (fgets(c_line, sizeof(c_line), fin))
            {
                // get item vid
                item = strtok(c_line, " ");
                vid_item = SearchHashTable(item);
                if (vid_item == -1)
                {
                    vid_item = InsertHashTable(item);
                }

                // get concept vid
                concept = strtok(NULL, " \n");
                while(concept != NULL)
                {
                    vid_concept = SearchHashTable(concept);
                    if (vid_concept == -1)
                    {
                        vid_concept = InsertHashTable(concept);
                    }
                    
                    // item-concept connections
                    if (ic[vid_item].find(vid_concept)!=ic[vid_item].end())
                    {
                        // already existed
                    }
                    else
                    {
                        ic[vid_item][vid_concept] = 1.0;
                        concept_list.push_back(vid_concept);
                        MAX_edge += 1;
                    }

                    concept = strtok(NULL, " \n");
                }
                
                // concept-concept connections
                for (auto vid1: concept_list)
                    for (auto vid2: concept_list)
                        if (vid1 != vid2)
                        {
                            if (ic[vid1].find(vid2)!=ic[vid1].end())
                            {
                                // already existed
                            }
                            else
                            {
                                ic[vid1][vid2] = 1.0;
                                MAX_edge += 1;
                            }
                        }
                concept_list.clear();
            }

            if (line % MONITOR == 0)
            {
                printf("\tProgress:\t\t%.2f %%%c", (double)(line)/(MAX_edge+1) * 100, 13);
                fflush(stdout);
            }
        }
        fclose(fin);
    }
    cout << "\tProgress:\t\t100.00 %\r" << endl;
    cout << "\t# of vertex:\t\t" << MAX_vid << endl;
    
    cout << "Build the Alias Method:" << endl;
    BuildAliasMethod(ic);
    cout << "\tFinished." << endl;

}

void vvNet::LoadDict(unordered_map< string, unordered_map< string, double > > &graph) {
    
    long vid1, vid2;
    unordered_map< long, unordered_map<long, double> > vv;     // vertex -> vertex: weight
    
    // generate keys lookup table (kmap) 
    for (auto v1: graph)
    {
        vid1 = SearchHashTable(v1.first);
        if (vid1 == -1)
        {
            vid1 = InsertHashTable(v1.first);
        }

        for (auto v2: v1.second)
        {
            vid2 = SearchHashTable(v2.first);
            if (vid2 == -1)
            {
                vid2 = InsertHashTable(v2.first);
            }

            vv[vid1][vid2] = v2.second;
            MAX_edge += 1;
        }
    }

    cout << "Build the Alias Method:" << endl;
    BuildAliasMethod(vv);
    cout << "\tFinished." << endl;

}


void vvNet::BuildAliasMethod(unordered_map< long, unordered_map< long, double > > &graph) {

    // re-construct the graph
    // source === (weight) === target
    
    cout << "\tInitializing ..." << endl;
    
    vertex.resize(MAX_vid);
    context.resize(MAX_edge);
    
    long vid;
    unsigned long long line_g=0;
    long offset = 0;
    for (long v1=0; v1!=MAX_vid; v1++)
    {
        vertex[v1].offset = offset;
        vertex[v1].branch = graph[v1].size();
        offset += graph[v1].size();

        for (auto v2: graph[v1])
        {
            context[line_g].vid = v2.first;
            vertex[v1].out_degree += v2.second;
            context[line_g].in_degree = v2.second;
            line_g++;
        }
    }

    for (unsigned long long line=0; line!=MAX_edge; line++)
    {
        vid = context[line].vid;
        vertex[vid].in_degree += context[line].in_degree;
    }
    
    graph.clear();

    // compute alias table
    cout << "\tAlias Table Constructing ..." << endl;
    BuildNegativeAliasTable();
    BuildSourceAliasTable();
    BuildTargetAliasTable();

}

void vvNet::BuildNegativeAliasTable() {

    // normalization of vertices weights
    double sum, norm;
    vector <double> norm_prob;
    negative_AT.resize(MAX_vid);
    
    sum = 0;
    for (long v1=0; v1!=MAX_vid; v1++)
    {
        sum += pow((vertex[v1].in_degree), 0.75);
    }
    norm = MAX_vid/sum;

    for (long v1=0; v1!=MAX_vid; v1++)
    {
        norm_prob.push_back( pow((vertex[v1].in_degree), 0.75)*norm );
    }
 
    // block divison
    vector <long> small_block, large_block;
    
    for (long v1=0; v1!=MAX_vid; v1++)
    {
        if ( norm_prob[v1]<1 )
        {
            small_block.push_back( v1 );
        }
        else
        {
            large_block.push_back( v1 );
        }
    }

    // assign alias table
    long small_v, large_v;

    while (small_block.size() && large_block.size())
    {
        small_v = small_block.back();
        small_block.pop_back();
        large_v = large_block.back();
        large_block.pop_back();

        negative_AT[small_v].alias = large_v;
        negative_AT[small_v].prob = norm_prob[small_v];
        norm_prob[large_v] = norm_prob[large_v] + norm_prob[small_v] - 1;
        if (norm_prob[large_v] < 1)
        {
            small_block.push_back( large_v );
        }
        else
        {
            large_block.push_back( large_v );
        }
    }

    while (large_block.size())
    {
        large_v = large_block.back();
        large_block.pop_back();
        negative_AT[large_v].prob = 1.0;
    }

    while (small_block.size())
    {
        small_v = small_block.back();
        small_block.pop_back();
        negative_AT[small_v].prob = 1.0;
    }

}

void vvNet::BuildSourceAliasTable() {

    // normalization of vertices weights
    double sum, norm;
    vector <double> norm_prob;
    vertex_AT.resize(MAX_vid);
    
    sum = 0;
    for (long v1=0; v1<MAX_vid; v1++)
    {
        sum += vertex[v1].out_degree;
    }
    norm = MAX_vid/sum;

    for (long v1=0; v1<MAX_vid; v1++)
    {
        norm_prob.push_back( vertex[v1].out_degree*norm );
    }
 
    // block divison
    vector <long> small_block, large_block;
    
    for (long v1=0; v1<MAX_vid; v1++)
    {
        if ( norm_prob[v1]<1 )
        {
            small_block.push_back( v1 );
        }
        else
        {
            large_block.push_back( v1 );
        }
    }
    
    // assign alias table
    long small_v, large_v;

    while (small_block.size() && large_block.size())
    {
        small_v = small_block.back();
        small_block.pop_back();
        large_v = large_block.back();
        large_block.pop_back();

        vertex_AT[small_v].alias = large_v;
        vertex_AT[small_v].prob = norm_prob[small_v];
        norm_prob[large_v] = norm_prob[large_v] + norm_prob[small_v] - 1;
        if (norm_prob[large_v] < 1)
        {
            small_block.push_back( large_v );
        }
        else
        {
            large_block.push_back( large_v );
        }
    }

    while (large_block.size())
    {
        large_v = large_block.back();
        large_block.pop_back();
        vertex_AT[large_v].prob = 1.0;
    }

    while (small_block.size())
    {
        small_v = small_block.back();
        small_block.pop_back();
        vertex_AT[small_v].prob = 1.0;
    }

}

void vvNet::BuildTargetAliasTable() {
    
    context_AT.resize(MAX_edge);

    for (long vid=0; vid<MAX_vid;vid++)
    {
        // normalization of vertices weights
        long offset, branch;
        offset = vertex[vid].offset;
        branch = vertex[vid].branch;

        double sum, norm;
        vector <double> norm_prob;
        sum = 0;
        for (long i=0; i<branch; i++)
        {
            sum += context[i+offset].in_degree;
        }
        norm = branch/sum;
        for (long i=0; i<branch; i++)
        {
            norm_prob.push_back( context[i+offset].in_degree*norm );
        }

        // block divison
        vector <long> small_block, large_block;
        for (long i=0; i<branch; i++)
        {
            if ( norm_prob[i]<1 )
            {
                small_block.push_back( i );
            }
            else
            {
                large_block.push_back( i );
            }
        }

        // assign alias table
        long small_i, large_i;
        while (small_block.size() && large_block.size())
        {
            small_i = small_block.back();
            small_block.pop_back();
            large_i = large_block.back();
            large_block.pop_back();

            context_AT[small_i+offset].alias = context[large_i+offset].vid;
            context_AT[small_i+offset].prob = norm_prob[small_i];
            norm_prob[large_i] = norm_prob[large_i] + norm_prob[small_i] - 1;
            if (norm_prob[large_i] < 1)
            {
                small_block.push_back( large_i );
            }
            else
            {
                large_block.push_back( large_i );
            }
        }
        
        while (large_block.size())
        {
            large_i = large_block.back();
            large_block.pop_back();
            context_AT[large_i+offset].prob = 1.0;
        }

        while (small_block.size())
        {
            small_i = small_block.back();
            small_block.pop_back();
            context_AT[small_i+offset].prob = 1.0;
        }

    }

}

long vvNet::NegativeSample() {
    
    long rand_v = random_gen(0, MAX_vid);
    double rand_p = random_gen(0, 1);
   
    if (rand_p < negative_AT[rand_v].prob)
        return rand_v;
    else
        return negative_AT[rand_v].alias;

}


long vvNet::SourceSample() {
    
    double rand_p = random_gen(0, 1);
    long rand_v = random_gen(0, MAX_vid);
    
    if (rand_p < vertex_AT[rand_v].prob)
        return rand_v;
    else
        return vertex_AT[rand_v].alias;

}

long vvNet::TargetSample() {
    
    double rand_p = random_gen(0, 1);
    long rand_v = random_gen(0, MAX_edge);

    if (rand_p < context_AT[rand_v].prob)
        return context[rand_v].vid;
    else
        return context_AT[rand_v].alias;

}

long vvNet::TargetSample(long vid) {
    
    if (vertex[vid].branch==0) return -1;

    double rand_p = random_gen(0, 1);
    long rand_v = random_gen(0, vertex[vid].branch) + vertex[vid].offset;

    if (rand_p < context_AT[rand_v].prob)
        return context[rand_v].vid;
    else
        return context_AT[rand_v].alias;

}


// Optimizer
void vvNet::UpdatePair(vector< vector<double> >& w_vertex, vector< vector<double> >& w_context, long vertex, long context, int dimension, int negative_samples, double alpha){
    
    vector< double >* w_vertex_ptr;
    vector< double >* w_context_ptr;
    vector< double > back_err;
    back_err.resize(dimension, 0.0);

    int d;
    long rand_v;
    double label, g, f, rand_p;
    
    label = 1.0;
    w_vertex_ptr = &w_vertex[vertex];
    w_context_ptr = &w_context[context];

    negative_samples += 1;
    // 0 for postive sample, others for negative sample
    for (int neg=0; neg!=negative_samples; ++neg)
    {
        // negative sampling
        if (neg!=0){
            label = 0.0;
            w_context_ptr = &w_context[ NegativeSample() ]; // Negative Sample
        }

        f = 0;
        for (d=0; d<dimension; ++d) // prediciton
            f += (*w_vertex_ptr)[d] * (*w_context_ptr)[d];
        f = fastSigmoid(f); // fast sigmoid(prediction)
        g = (label - f) * alpha; // gradient
        for (d=0; d<dimension; ++d) // store the back propagation error
            back_err[d] += g * (*w_context_ptr)[d];
        for (d=0; d<dimension; ++d) // update context
            (*w_context_ptr)[d] += g * (*w_vertex_ptr)[d];
    }
    for (d=0; d<dimension; ++d)
        (*w_vertex_ptr)[d] += back_err[d];

}



