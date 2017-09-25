#define _GLIBCXX_USE_CXX11_ABI 0
#include "./ICE.h"

int ArgPos(char *str, int argc, char **argv) {
    int a;
    for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
        if (a == argc - 1) {
            printf("Argument missing for %s\n", str);
            exit(1);
        }
        return a;
    }
    return -1;
}

int main(int argc, char **argv){
    
    int i;

    if (argc == 1) {
        printf("[ICE-CLI]\n");
        printf("\tcommand line interface for ICE\n\n");
        printf("Options:\n");
        printf("\t-train <string>\n");
        printf("\t\tTrain the Network data\n");
        printf("\t-save <string>\n");
        printf("\t\tSave the representation data\n");
        printf("\t-dim <int>\n");
        printf("\t\tDimension of vertex representation; default is 64\n");
        printf("\t-neg <int>\n");
        printf("\t\tNumber of negative examples; default is 5\n");
        printf("\t-sample <int>\n");
        printf("\t\tNumber of training samples *Million; default is 10\n");
        printf("\t-thread <int>\n");
        printf("\t\tNumber of training threads; default is 1\n");
        printf("\t-alpha <float>\n");
        printf("\t\tInit learning rate; default is 0.025\n");
        printf("\nExample Usage:\n");
        printf("\t./ice -train network.txt -save rep.txt -dim 64 -sample 10 -neg 5 -alpha 0.025 -thread 4\n\n");
        return 0;
    }
    
    char network_file[100], rep_file[100];
    int dimensions=64, negative_samples=5, sample_times=10, threads=1;
    double init_alpha=0.025;

    if ((i = ArgPos((char *)"-train", argc, argv)) > 0) strcpy(network_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-save", argc, argv)) > 0) strcpy(rep_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-dim", argc, argv)) > 0) dimensions = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-sample", argc, argv)) > 0) sample_times = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-neg", argc, argv)) > 0) negative_samples = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-alpha", argc, argv)) > 0) init_alpha = atof(argv[i + 1]);
    if ((i = ArgPos((char *)"-thread", argc, argv)) > 0) threads = atoi(argv[i + 1]);

    ICE *ice;
    ice = new ICE();
    ice->LoadEdgeList(network_file);
    ice->Init(dimensions);
    ice->Train(sample_times, negative_samples, init_alpha, threads);
    ice->SaveWeights(rep_file);

    return 0;

}
