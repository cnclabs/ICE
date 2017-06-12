from __future__ import print_function, division, unicode_literals
import requests
import json
import re
from collections import defaultdict

def get_concept(word_list, lang='en'):
    concept = {}
    for word in word_list:
        obj = requests.get('http://api.conceptnet.io/c/' + lang + '/' + word).text
        obj = json.loads(obj)
        concept[word] = obj

    # get concept words
    result = {}
    for key, value in concept.items():
        concept_words = []
        for item in value['edges']:
            if item['rel']['label'] == 'RelatedTo' and lang == 'en':
                concept_words.append(item['start']['label'])
                concept_words.append(item['end']['label'])
            elif lang == 'zh':
                concept_words.append(item['start']['label'])
                concept_words.append(item['end']['label'])
        concept_words = [x for x in concept_words if x != key]
        result[key] = list(set(concept_words))

    return result

def dict2list(c_dict):
    edge_list = []
    for key, values in c_dict.items():
        for word in values:
            edge_list.append((key, word))
            edge_list.append((word, key))

    return set(edge_list)

def edge2file(edge_list, outfile):
    edge_strings = []
    for edge in edge_list:
        s = ' '.join(edge) + ' 1'
        edge_strings.append(s)

    edge_strings = sorted(edge_strings)
    print(edge_strings)

    with open(outfile, 'w') as f:
        for s in edge_strings:
            print(s, file=f)

def file2dict(path):
    edge_dict = defaultdict(dict)
    with open(path) as f:
        for line in f:
            line = line.split()
            fromNode = line[0]
            toNode = line[1]
            edge_dict[fromNode][toNode] = 1
    return edge_dict

if __name__ == '__main__':
    lang = 'en'
    words_path = './data/wordlist_{}.txt'.format(lang)
    word_list = []

    with open(words_path) as f:
        for line in f:
            word_list.append(line.strip())

    concepts = get_concept(word_list, lang=lang)    
    edge_list = dict2list(concepts)
    edge2file(edge_list, './data/word-edges_{}.txt'.format(lang))
    
