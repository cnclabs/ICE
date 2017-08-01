# ICE: Item Concept Embedding via Textual Information
## 1. Introduction
The ICE toolkit is designed to embed concepts into items such that the resulted item embeddings can be compared in terms of overall conceptual similarity regardless of item types ([ICE: Item Concept Embedding via Textual Information](http://dl.acm.org/citation.cfm?doid=3077136.3080807), SIGIR 2017). For example, a song can be used to retrieve conceptually similar songs (homogeneous) as well as conceptually similar concepts (heterogeneous). 

Specifically, ICE incorporates items and their representative concepts (words extracted from the item's textual information) using a heterogeneous network and then learns the embeddings for both items and concepts in terms of the shared concept words.
Since items are defined in terms of concepts, adding expanded concepts into the network allows the learned embeddings to be used to retrieve conceptually more diverse and yet relevant results.

### 1.1. System Requirements
- g++ > 4.9
- python3
- cython

### 1.2. Getting Started
Download:
```
$ git clone https://github.com/CLIPLab/ICE
$ cd ./ICE/ice
```

c++ compilation:
```
$ make ice
$ ./ice
```
python3 API compilation:
```
$ make python
```

## 2. API Usage
There are two stages in the ICE framework: graph construction and embedding learning. In the first stage, the API combines an entity-text network and a text-text network to form an ICE network. In the second stage, the API learns the embeddings of both items and entities based on the relations specified in the ICE network.
### 2.1 Graph Construction
- Run:
```
$ python construct_ice.py -et ../data/song_et.edge -tt ../data/song_tt.edge -ice song_ice.edge
```
- Parameters:
```
  -et <string>, --et_network <string>
          Input Entity-text Network
  -tt <string>, --tt_network <string>
          Input Text-text Network
  -ice <string>, --ice_network <string>
          Output ICE Network
```
Sample files: See `data/song_et.edge` and `data/song_tt.edge`
#### Example Input
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

### 2.2. Embedding Learning
- Run:
```
./ice -train song_ice.edge -save song.embd -dim 4 -sample 10 -neg 5 -thread 1 -alpha 0.025
```
- Parameters:
```
Options:
        -train <string>
                Path to the network used for embedding learning
        -save <string>
                Path to save the embedding file
        -dim <int>
                Dimension of embedding; default is 64
        -neg <int>
                Number of negative examples; default is 5
        -sample <int>
                Number of training samples *Million; default is 10
        -thread <int>
                Number of training threads; default is 1
        -alpha <float>
                Initial learning rate; default is 0.025
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
ice.train(sample=11, neg=5, alpha=0.025, workers=1)
ice.save_weights(model_name='ICE.rep')
```

## Example Output (Embeddings)
```txt
五月天 0.47944 1.03708 -1.78878 -0.856039
MAYDAY@ 0.48029 1.04692 -1.78591 -0.855289
Sodagreen -0.849743 0.435535 -1.41524 1.75017
SEKAI_NO_OWARI 1.23796 -1.43277 -1.12657 1.28416
The_Beatles 2.21267 1.66775 0.42715 0.702507
```

## 3. Experimental Results
Here, we report the average performance of 10 results for two tasks. For details and other experiments, please refer to our [paper](http://dl.acm.org/citation.cfm?doid=3077136.3080807).
- OMDB word-to-movie retrieval task:
    - Graph construction: 20 representative words per item and 5 expanded words per representative word.
    - Embedding learning: dim=256, sample=200, neg=2

|    Genre   | Horror | Thriller | Western | Action | Short | Sci-Fi | Average |
|:----------:|:------:|:--------:|:-------:|:------:|:-----:|:------:|:-------:|
| Precision@50  |  0.329 |   0.211  |  0.338  |  0.446 | 0.087 |  0.389 |  0.300  |
| Precision@100  |  0.320 |   0.214  |  0.292  |  0.425 | 0.074 |  0.380 |  0.284  |


- KKBOX word-to-song retrieval task:
    - Graph construction: 10 representative words per item and 3 expanded words per representative word.
    - Embedding learning: dim=300, sample=10000, neg=5

|Query|失落 (lost)|心痛 (heartache)|想念 (pining)|深愛 (affectionate)|難過 (sad)|回家 (home)|房間 (room)|海邊 (seaside)|火車 (train)|花園 (garden)|夕陽 (dusk)|日出 (sunrise)|日落 (sunset)|月亮 (moon)|黑夜 (night)|Average|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|Keyword precision@100|0.550|0.266|0.573|0.523|0.544|0.959|0.520|0.360|0.484|0.451|0.376|0.400|0.610|0.897|0.506|0.535|
|Concept-similar word Precision@100|0.063|0.496|0.039|0.263|0.033|0.086|0.049|0.110|0.026|0.000|0.111|0.711|0.506|0.361|0.017|0.191|

    
## 4. Citation
```
@inproceedings{Wang:2017:IIC:3077136.3080807,
 author = {Wang, Chuan-Ju and Wang, Ting-Hsiang and Yang, Hsiu-Wei and Chang, Bo-Sin and Tsai, Ming-Feng},
 title = {ICE: Item Concept Embedding via Textual Information},
 booktitle = {Proceedings of the 40th International ACM SIGIR Conference on Research and Development in Information Retrieval},
 series = {SIGIR '17},
 year = {2017},
 isbn = {978-1-4503-5022-8},
 location = {Shinjuku, Tokyo, Japan},
 pages = {85--94},
 numpages = {10},
 url = {http://doi.acm.org/10.1145/3077136.3080807},
 doi = {10.1145/3077136.3080807},
 acmid = {3080807},
 publisher = {ACM},
 address = {New York, NY, USA},
 keywords = {concept embedding, conceptual retrieval, information network, textual information},
} 
```

