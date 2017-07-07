# ICE: Item Concept Embedding

Developed Environment:
- g++ > 4.9
- python3
- cython

## Getting Started
c++ compilation
```
$ git clone https://github.com/CLIPLab/ICE
$ cd ./ICE/ICE
$ make cli
$ ./cli
```
python3 API compilation
```
make python
```

## Construct ICE graph
```
$ python graph.py -i ./data/lyrics_small.csv -w ./data/word-edges_en.csv -o network.edgelist
```
Parameters:
```
  -i <string>, --input <string>
          Input File Name
  -w <string>, --wordedges <string>
          Word Edges File Name
  -o <string>, --output <string>
          Output File Name
```

## Example Input
network.edgelist
```txt
五月天 Taiwanese 1
五月天 rock 1
五月天 band 1
MAYDAY@ Taiwanese 1
MAYDAY@ rock 1
MAYDAY@ band 1
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

## Command Line Interface

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
./cli -train network.edgelist -save rep.txt -dimensions 4 -sample_times 10 -negative_samples 5 -alpha 0.025 -threads 1
```

## Python3 API usage
After the compilation of python3, see example.py for example usage
```python
from pyICE import pyICE

ice = pyICE()
network = {
    '五月天': {
        'Taiwanese': 1,
        'rock': 1,
        'band': 1
        },
    'MAYDAY@': {
        'Taiwanese': 1,
        'rock': 1,
        'band': 1
        },
    'Sodagreen': {
        'Taiwanese': 1,
        'indie': 1,
        'pop_rock': 1,
        'band': 1
        },
    'SEKAI_NO_OWARI': {
        'Japanese': 1,
        'indie': 1,
        'pop_rock': 1,
        'band': 1
        },
    'The_Beatles': {
        'England': 1,
        'rock': 1,
        'pop': 1
        }
}
ice.load_dict(network)
ice.init(dimension=4)
ice.train(sample_times=11, negative_samples=5, alpha=0.025, workers=1)
ice.save_weights(model_name='ICE.rep')
```

## Example Output (the Learned Representations)
```txt
五月天 0.47944 1.03708 -1.78878 -0.856039
MAYDAY@ 0.48029 1.04692 -1.78591 -0.855289
Sodagreen -0.849743 0.435535 -1.41524 1.75017
SEKAI_NO_OWARI 1.23796 -1.43277 -1.12657 1.28416
The_Beatles 2.21267 1.66775 0.42715 0.702507
```

