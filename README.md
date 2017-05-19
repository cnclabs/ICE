# ICE: Item Concept Embedding

Developed Environment:
- g++ > 4.9

# Getting Started
```
$ git clone https://github.com/CLIPLab/ICE
$ cd ./ICE/ICE
$ make
```

# Example Input
Given a network input:
```txt
Mayday Taiwanese 1
Mayday rock 1
Mayday band 1
Sodagreen Taiwanese 1
Sodagreen indie 1
Sodagreen pop_rock 1
Sodagreen band 1
SEKAI_NO_OWARI Japanese 1
SEKAI_NO_OWARI indie 1
SEKAI_NO_OWARI pop_rock 1
SEKAI_NO_OWARI band 1
The_Beatles England 1
The_Beatles rock 1
The_Beatles pop 1
```
The model learns the representations according to the relations:
```
Mayday 0.356144 0.650535 0.418468 -2.62163
Sodagreen 1.59218 1.55554 0.581035 0.173465
SEKAI_NO_OWARI 1.5532 -1.02193 1.47116 0.20981
The_Beatles -1.2945 0.467587 2.24252 -0.274896
```

# Command Line Interface

Parameters:
```
Options:
        -train <string>
                Train the Network data
        -save <string>
                Save the representation data
        -dimensions <int>
                Dimension of vertex representation; default is 64
        -negative_samples <int>
                Number of negative examples; default is 5
        -sample_times <int>
                Number of training samples *Million; default is 10
        -threads <int>
                Number of training threads; default is 1
        -alpha <float>
                Init learning rate; default is 0.025
```

Example Usage:
```
./cli -train network.txt -save rep.txt -dimensions 64 -sample_times 10 -negative_samples 5 -alpha 0.025 -threads 1
```
