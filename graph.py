from __future__ import print_function, division, unicode_literals
import argparse
import csv
from collections import defaultdict
import numpy as np
import sys
from utils import get_concept, file2dict

if __name__ == '__main__':
    PARSER = argparse.ArgumentParser(description='Transform text data to edge list file.')

    PARSER.add_argument('-i', '--input', default=None, help='Input File Name')
    PARSER.add_argument('-o', '--output', default=None, help='Output File Name')
    PARSER.add_argument('-w', '--wordedges', default=None, help='Word Edges File Name')

    CONFIG = PARSER.parse_args()

    input_path = CONFIG.input
    output_path = CONFIG.output
    word_edge_path = CONFIG.wordedges

    item_count = 0
    vocabulary_set = set()
    # item_list = []
    item_set = {}
    # tokens_list = []
    tokens_list = defaultdict(list)
    item_text_dict = {}
    text_text_dict = defaultdict(list)
    id2word = {}
    print(input_path, output_path, word_edge_path)

    print('Construct item-text structure...')
    with open(input_path) as f:
        # reader = csv.reader(f)
        # for line in reader:
        #     key = line[0]
        #     song_list.append(key.replace('_','@').replace(' ','_'))
        #     tokens = list(set(line[1].split(' ')))
        #     vocabulary_set.update(tokens)
        #     tokens_list.append(tokens)
        #     item_count += 1
        for line in f:
            line = line.split()
            fromNode = line[0]
            toNode = line[1]
            if fromNode not in item_set:
                item_set[fromNode] = item_count
                item_count += 1
            vocabulary_set.add(toNode)
            tokens_list[fromNode].append(toNode)

    tokens_list = {item_set[x]:y for x,y in tokens_list.items()}
    item_set = {y:x for x,y in item_set.items()}

    print('Construct text-text structure...')
    with open(word_edge_path) as f:
        for line in f:
            line = line.split()
            fromNode = line[0]
            toNode = line[1]
            text_text_dict[fromNode].append(toNode)
            vocabulary_set.update([fromNode, toNode])
    for word in vocabulary_set:
        text_text_dict[word].append(word)

    for word in text_text_dict.keys():
        text_text_dict[word] = list(set(text_text_dict[word]))

    vocabulary_list = list(vocabulary_set)
    for i in range(len(vocabulary_list)):
        id2word[i] = vocabulary_list[i]

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
            # edge_list.append((item_list[i], word))
            edge_list.append((item_set[i], word))

    w2w_hash = []
    for i in range(len(vocabulary_list)):
        word = vocabulary_list[i]
        word2word = [x for x in text_text_dict[word]]
        if len(word2word) == 0:
            continue
        for w in word2word:
            edge_list.append((word, w))

    with open(output_path, 'w') as f:
        for edge in edge_list:
            f.write(edge[0] + ' ' + edge[1] + ' 1\n')
