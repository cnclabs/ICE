from __future__ import print_function, division, unicode_literals
import argparse
import gensim
from gensim.models.keyedvectors import KeyedVectors
import csv
from collections import defaultdict
from annoy import AnnoyIndex
import numpy as np

if __name__ == '__main__':
    PARSER = argparse.ArgumentParser(description='Transform text data to edge list file.')

    PARSER.add_argument('-i', '--input', default=None, help='Input File Name')
    PARSER.add_argument('-m', '--model', default=None, help='Pretrained Word2Vec Model Path')
    PARSER.add_argument('-o', '--output', default=None, help='Output File Name')
    PARSER.add_argument('-d', '--dim', default=300, type=int, help='word2vec dimension')
    PARSER.add_argument('-t', '--threshold', default=0.9, type=float, help='annoy distance threshold to capture similar concept')

    CONFIG = PARSER.parse_args()

    input_path = CONFIG.input
    model_path = CONFIG.model
    output_path = CONFIG.output
    dim = CONFIG.dim
    thres = CONFIG.threshold

    item_count = 0
    vocabulary_set = set()
    song_list = []
    tokens_list = []
    item_text_dict = {}
    text_text_dict = defaultdict(list)
    print(input_path, model_path, output_path, dim, thres)

    print('Construct item-text structure...')
    with open(input_path) as f:
        reader = csv.reader(f)
        for line in reader:
            key = line[0]
            song_list.append(key.replace('_','@').replace(' ','_'))
            tokens = list(set(line[1].split(' ')))
            vocabulary_set.update(tokens)
            tokens_list.append(tokens)
            item_count += 1

    print('Loading word2vec model...')
    word2vec_model = KeyedVectors.load_word2vec_format(model_path, binary=True)

    # print(song_text_dict)
    # print(vocabulary_set)
    id2word = {}
    vocabulary_list = [x for x in vocabulary_set if x in word2vec_model]
    print('Construct text-text structure...')
    annoy_indexer = AnnoyIndex(dim)
    for i in range(len(vocabulary_list)):
        id2word[i] = vocabulary_list[i]
        vec = word2vec_model[vocabulary_list[i]]
        annoy_indexer.add_item(i, vec)

    annoy_indexer.build(100)
    for i in range(len(vocabulary_list)):
        word = vocabulary_list[i]
        t2t_result = annoy_indexer.get_nns_by_item(i, 20, include_distances=True)
        word_table = t2t_result[0][1:]
        dist_table = t2t_result[1][1:]
        t2t_result = [vocabulary_list[y] for x,y in enumerate(word_table) if dist_table[x] < thres]
        # print(t2t_result)
        if len(t2t_result) > 0:
            text_text_dict[word] = t2t_result 

    # print(text_text_dict)
    print('Build ICE Matrix...')
    M_et = np.zeros( (item_count, len(vocabulary_list)) )
    M_tt = np.zeros( (len(vocabulary_list), len(vocabulary_list)) )

    for i in range(item_count):
        M_et[i, :] = np.array([1 if x in tokens_list[i] else 0 for x in vocabulary_list])
    for i in range(len(vocabulary_list)):
        M_tt[i, :] = np.array([1 if x in text_text_dict[vocabulary_list[i]] else 0 for x in vocabulary_list])

    A = np.dot(M_et, M_tt)
    A = A.tolist()

    print('Matrix to edge list...')
    edge_list = []
    for i in range(item_count):
        words_index = [x for x,y in enumerate(A[i]) if y > 0]
        words = [id2word[x] for x in words_index]
        for word in words:
            edge_list.append((song_list[i], word))

    w2w_hash = []
    for i in range(len(vocabulary_list)):
        word = vocabulary_list[i]
        word2word = [x for x in text_text_dict[word]]
        if len(word2word) == 0:
            continue
        for w in word2word:
            if (word, w) not in w2w_hash and (w, word) not in w2w_hash:
                edge_list.append((word, w))
                w2w_hash.append((word, w))

    with open(output_path, 'w') as f:
        for edge in edge_list:
            f.write(edge[0] + ' ' + edge[1] + ' 1\n')
