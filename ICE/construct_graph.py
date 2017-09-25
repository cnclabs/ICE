from __future__ import print_function, division, unicode_literals
import argparse
import csv
from collections import defaultdict
import numpy as np
import sys
from copy import deepcopy
from scipy.sparse import csr_matrix


if __name__ == '__main__':
    PARSER = argparse.ArgumentParser(description='Transform text data to edge list file.')

    PARSER.add_argument('-et', '--entitytext', default=None, help='Entity-Text edgelist File Name')
    PARSER.add_argument('-ice', '--icefile', default=None, help='ICE graph File Name')
    PARSER.add_argument('-tt', '--textedges', default=None, help='Text-Text edgelist File Name')

    CONFIG = PARSER.parse_args()

    if CONFIG.entitytext == None:
        print("Please give Entity-Text edgelist!\nusage: graph.py [-h] [-et ENTITYTEXT] [-o OUTPUT] [-tt TEXTEDGES]")
        sys.exit()
    elif CONFIG.icefile == None:
        print("Please give ICE graph name\nusage: graph.py [-h] [-et ENTITYTEXT] [-o OUTPUT] [-tt TEXTEDGES]")
        sys.exit()
    elif CONFIG.textedges == None:
        print("Please give Text-Text edgelist\nusage: graph.py [-h] [-et ENTITYTEXT] [-o OUTPUT] [-tt TEXTEDGES]")
        sys.exit()

    input_path = CONFIG.entitytext
    output_path = CONFIG.icefile
    word_edge_path = CONFIG.textedges

    item_count = 0
    vocabulary_set = set()
    word2id = {}
    id2word = {}
    entity_text_dict = defaultdict(set)
    text_text_dict = defaultdict(set)
    print(input_path, output_path, word_edge_path)

    print('Construct item-text structure...')
    with open(input_path) as f:
        for line in f:
            line = line.split()
            fromNode = line[0]
            toNode = line[1]
            vocabulary_set.add(toNode)
            if fromNode not in entity_text_dict:
                item_count += 1
            entity_text_dict[fromNode].add(toNode)
            


    print('Construct text-text structure...')
    with open(word_edge_path) as f:
        for line in f:
            line = line.split()
            fromNode = line[0]
            toNode = line[1]
            text_text_dict[fromNode].add(toNode)
            text_text_dict[toNode].add(fromNode)
            vocabulary_set.update([fromNode, toNode])
            vocabulary_set.update([toNode, fromNode])
    for word in vocabulary_set:
        text_text_dict[word].add(word)

    vocabulary_list = list(vocabulary_set)
    vocabulary_count = len(vocabulary_list)
    for i in range(len(vocabulary_list)):
        word2id[vocabulary_list[i]] = i
        id2word[i] = vocabulary_list[i]

    i = 0
    row = []
    column = []
    data = []
    print("Build entity-text Matrix...")
    entity_list = []
    for entity, words in entity_text_dict.items():
        entity_list.append(entity)
        words_index = [word2id[w] for w in words]
        for w in words_index:
            row.append(i)
            column.append(w)
            data.append(1)
        i += 1
    row = np.array(row)
    column = np.array(column)
    data = np.array(data)
    M_et = csr_matrix((data, (row, column)), shape=(item_count, vocabulary_count), dtype=np.int8)

    i = 0
    row = []
    column = []
    data = []
    print("Build text-text Matrix...")
    for word, words in text_text_dict.items():
        words_index = [word2id[w] for w in words]
        for w in words_index:
            row.append(word2id[word])
            column.append(w)
            data.append(1)
        i += 1
    row = np.array(row)
    column = np.array(column)
    data = np.array(data)
    M_tt = csr_matrix((data, (row, column)), shape=(vocabulary_count, vocabulary_count), dtype=np.int8)

    print("Build ICE Graph...")

    A = M_et.dot(M_tt)

    edge_list = []
    result = A.nonzero()
    size = result[0].size
    row = result[0]
    col = result[1]
    for i in range(size):
        edge_list.append((entity_list[row[i]], vocabulary_list[col[i]]))
        # print("{}/{}".format(i, size), end='\r')

    for i in range(len(vocabulary_list)):
        word = vocabulary_list[i]
        word2word = [x for x in text_text_dict[word]]
        if len(word2word) == 0:
            continue
        for w in word2word:
            edge_list.append((word, w))


    print("Write edge list...")
    with open(output_path, 'w') as f:
        for edge in edge_list:
            f.write(edge[0] + ' ' + edge[1] + ' 1\n')
