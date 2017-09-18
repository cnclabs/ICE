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
        printf("\t-format <format>\n");
        printf("\t\tFormat of the train network; ic or vv\n");
        printf("\t-save <string>\n");
        printf("\t\tSave the representation data\n");
        printf("\t-save_times <int>\n");
        printf("\t\tsave a model <int> times; default is 1\n");
        printf("\t-dimensions <int>\n");
        printf("\t\tDimension of vertex representation; default is 64\n");
        printf("\t-negative_samples <int>\n");
        printf("\t\tNumber of negative examples; default is 5\n");
        printf("\t-sample_times <int>\n");
        printf("\t\tNumber of training samples *Million; default is 10\n");
        printf("\t-threads <int>\n");
        printf("\t\tNumber of training threads; default is 1\n");
        printf("\t-alpha <float>\n");
        printf("\t\tInit learning rate; default is 0.025\n");
        printf("\nExample Usage:\n");
        printf("\t./cli -train network.txt -save rep.txt -dimensions 64 -sample_times 10 -negative_samples 5 -alpha 0.025 -threads 4\n\n");
        return 0;
    }
    
    char network_file[100], rep_file[100], format[100];
    int dimensions=64, negative_samples=5, sample_times=10, save_times=1, threads=1;
    double init_alpha=0.025;

    if ((i = ArgPos((char *)"-train", argc, argv)) > 0) strcpy(network_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-format", argc, argv)) > 0) strcpy(format, argv[i + 1]);
    if ((i = ArgPos((char *)"-save", argc, argv)) > 0) strcpy(rep_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-save_times", argc, argv)) > 0) save_times = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-dim", argc, argv)) > 0) dimensions = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-sample", argc, argv)) > 0) sample_times = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-neg", argc, argv)) > 0) negative_samples = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-alpha", argc, argv)) > 0) init_alpha = atof(argv[i + 1]);
    if ((i = ArgPos((char *)"-thread", argc, argv)) > 0) threads = atoi(argv[i + 1]);
    
    char ic_format[10] = "ic";
    char vv_format[10] = "vv";

    ICE *ice;
    ice = new ICE();
    ice->LoadEdgeList(network_file);
    ice->Init(dimensions);
    
    int sub_sample_times = sample_times/save_times;
    double alpha_max=init_alpha, alpha_min=init_alpha;
    for (int i=0; i<save_times; ++i)
    {
        alpha_max = alpha_min;
        alpha_min = init_alpha*((double)(save_times-i-1)/save_times);
        if (alpha_min < init_alpha*0.0001) alpha_min = init_alpha*0.0001;
        ice->Train(sub_sample_times, negative_samples, alpha_max, alpha_min, threads);

        if (i==(save_times-1))
        {
            ice->SaveWeights(rep_file);
        }
        else
        {
            string sub_rep_file = rep_file + string(".") + to_string(i);
            ice->SaveWeights(sub_rep_file);
        }
    }

    return 0;

}
