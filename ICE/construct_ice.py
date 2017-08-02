from __future__ import print_function, division, unicode_literals
import argparse
import csv
from collections import defaultdict
import numpy as np
import sys
from utils import get_concept, file2dict
from copy import deepcopy

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
    item_set = {}
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
            entity_text_dict[fromNode].add(toNode)

    item_set = {y:x for x,y in item_set.items()}

    print('Construct text-text structure...')
    with open(word_edge_path) as f:
        for line in f:
            line = line.split()
            fromNode = line[0]
            toNode = line[1]
            text_text_dict[fromNode].add(toNode)
            vocabulary_set.update([fromNode, toNode])
    for word in vocabulary_set:
        text_text_dict[word].add(word)

    vocabulary_list = list(vocabulary_set)
    for i in range(len(vocabulary_list)):
        id2word[i] = vocabulary_list[i]


    for item, tokens in deepcopy(entity_text_dict).items():
        for word in tokens:
            for exp_w in text_text_dict[word]:
                entity_text_dict[item].add(exp_w)

    edge_list = []
    for item, tokens in entity_text_dict.items():
        for word in tokens:
            edge_list.append((item, word))

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
