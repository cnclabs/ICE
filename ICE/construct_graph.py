################################################################################
#        ______                    ______                            __        #
#       /  _/ /____  ____ ___     / ____/___  ____  ________  ____  / /_       #
#       / // __/ _ \/ __ `__ \   / /   / __ \/ __ \/ ___/ _ \/ __ \/ __/       #
#     _/ // /_/  __/ / / / / /  / /___/ /_/ / / / / /__/  __/ /_/ / /_         #
#    /___/\__/\___/_/ /_/ /_/   \____/\____/_/ /_/\___/\___/ .___/\__/         #
#                                                         /_/                  #
#            ______          __             __    ___                          #
#           / ____/___ ___  / /_  ___  ____/ /___/ (_)___  ____ _              #
#          / __/ / __ `__ \/ __ \/ _ \/ __  / __  / / __ \/ __ `/              #
#         / /___/ / / / / / /_/ /  __/ /_/ / /_/ / / / / / /_/ /               #
#        /_____/_/ /_/ /_/_.___/\___/\__,_/\__,_/_/_/ /_/\__, /                #
#                                                       /____/ credit: patorjk #
################################################################################

# Proj: Item Concept Embedding (ICE)
# File: construct_graph.py
# Cont:
#   Func:
#       1) get_user_input                   2) gen_et_network
#       3) gen_tt_network                   4) dict2sparse_mat
#       5) save_ice_et_network              6) save_ice_tt_network

import argparse
from collections import defaultdict
import numpy as np
import sys
from scipy.sparse import csr_matrix
from tqdm import tqdm

def get_user_input():
    """ Get inputs from user.
    Return:
        return1 [string] path to load the entity-text relation edge list.
        return2 [string] path to load the text-text relation edge list.
        return3 [list] of string paths to save the expanded entity-text
            subnetwork within an ICE network.
        return4 [list] of string paths to save the text-text subnetwork within
            an ICE network.
        return5 [int] indicator of whether to use binary or real weights.
    """
    PARSER = argparse.ArgumentParser(description='Construct ICE network from ET and TT relations.')
    PARSER.add_argument('-et', help='Path to load ET relation.')
    PARSER.add_argument('-tt', help='Path to load TT relation.')
    PARSER.add_argument('-ice_full', help='(Optoinal) Path to save full ICE network.')
    PARSER.add_argument('-ice_et', help='(Optional) Path to save ET part of ICE network.')
    PARSER.add_argument('-ice_tt', help='(Optional) Path to save TT part of ICE network.')
    PARSER.add_argument('-w', type=int, default=0, choices=[0,1], help='(Default) 0:unweighted / 1:weighted.')
    CONFIG = PARSER.parse_args()

    if CONFIG.et == None:
        print('Please specify a path to load ET relation edge list.')
        sys.exit()
    elif CONFIG.tt == None:
        print('Please specify a path to load TT relation edge list.')
        sys.exit()
    elif CONFIG.ice_full == CONFIG.ice_et == CONFIG.ice_tt == None:
        print('Please specify at least one path to save the full or part of the ICE network.')
        sys.exit()

    et_save_list = []
    tt_save_list = []

    if CONFIG.ice_full != None:
        et_save_list.append((CONFIG.ice_full, 'w'))
        tt_save_list.append((CONFIG.ice_full, 'a'))
    if CONFIG.ice_et != None:
        et_save_list.append((CONFIG.ice_et, 'w'))
    if CONFIG.ice_tt != None:
        tt_save_list.append((CONFIG.ice_tt, 'w'))

    return CONFIG.et, CONFIG.tt, et_save_list, tt_save_list, CONFIG.w


def gen_et_network(et_path):
    """ Construct an entity-text network from entity-text relations.
    Param:
        param1 [string] path to an edge list file where entity-text relations
            are defined.
    Return:
        return1 [dict] where key=entity & val=list of 2-tuples of rep word and
            respective weight.
    Note:
        1) Assume word set in ET is a subset of word set in TT as a word only in
            TT will NOT benefit from ICE algorithm and is assumed to be removed.
    """
    et_dict = defaultdict(set)

    with open(et_path) as f:
        for line in f:
            entry = line.split()
            et_dict[entry[0]].add((entry[1], float(entry[2]))) # directed

    return et_dict


def gen_tt_network(tt_path):
    """ Construct a text-text network from text-text relations.
    Param:
        param1 [string] path to an edge list file where text-text relations are
            defined.
    Return:
        return [dict] where key=rep word & val=list of 2-tuples of exp word and
            respective weight.
    """
    tt_dict = defaultdict(set)
    word_set = set()
 
    with open(tt_path) as f:
        for line in f:
            entry = line.split()
            tt_dict[entry[0]].add((entry[1], float(entry[2]))) # undirected
            tt_dict[entry[1]].add((entry[0], float(entry[2])))
            tt_dict[entry[0]].add((entry[0], float(1))) # cos similarity to self
            tt_dict[entry[1]].add((entry[1], float(1)))
            word_set.update([entry[0], entry[1]])

    return tt_dict, list(word_set)


def dict2sparse_mat(edge_dict, node2row, node2col):
    """ Convert dictionary-based to sparse-matrix-based network.
    Param:
        param1 [dict] where key=from node & val=list of 2-tuples of to node and
            respective weight.
        param2 [dict] where key=node & val=row number.
        param3 [dict] where key=node & val=col number.
    Return:
        return1 [csr_matrix] sparse matrix network.
    """
    row_list = []
    col_list = []
    weight_list = []
    for from_node, tup_list in tqdm(edge_dict.items()):
        for to_node, weight in tup_list:
            row_list.append(node2row[from_node])
            col_list.append(node2col[to_node])
            weight_list.append(weight)

    return csr_matrix((weight_list, (row_list, col_list)), shape=(len(node2row), len(node2col)), dtype=np.float64)


def save_ice_et_network(exp_et_matrix, row2entity, col2word, et_save_list, weighted):
    """ Save the expanded entity-text subnetwork within an ICE network.
    Param:
        param1 [csr_matrix] sparse matrix of the expanded entity-text network.
        param2 [dict] where key=row number & val=entity.
        param3 [dict] where key=col number & val=rep word
        param4 [list] of 2-tuples of path and input mode.
        param5 [int] indicator of whether to use binary or real weights.
    """
    et_f_list = [open(et_path, mode) for et_path, mode in et_save_list] 

    if weighted == 1:
        for row, col in tqdm(list(zip(*exp_et_matrix.nonzero()))):
            entry = row2entity[row] + ' ' + col2word[col] + ' ' + str(exp_et_matrix[row, col]) + '\n'
        
            for et_f in et_f_list:
                et_f.write(entry)
    else:
        for row, col in tqdm(list(zip(*exp_et_matrix.nonzero()))):
            entry = row2entity[row] + ' ' + col2word[col] + ' 1.0\n'
        
            for et_f in et_f_list:
                et_f.write(entry)


def save_ice_tt_network(tt_dict, tt_save_list, weighted):
    """ Save the text-text subnetwork within an ICE network.
    Param:
        param1 [dict] where key=rep word & val=list of 2-tuples of rep word and
            respective weight.
        param2 [list] of 2-tuples of path and input mode.
        param3 [int] indicator of whether to use binary or real weights.
    """
    tt_f_list = [open(tt_path, mode) for tt_path, mode in tt_save_list]

    for rep_word, tup_list in tqdm(tt_dict.items()):
        for exp_word, weight in tup_list:
            entry = rep_word + ' ' + exp_word + ' ' + str(weight**weighted) + '\n'

            for tt_f in tt_f_list:
                tt_f.write(entry)


def main():
    # Step 0: Get inputs from user.
    et_path, tt_path, et_save_list, tt_save_list, w = get_user_input()

    print('\nStart constructing ICE network!') 
    print('Step 1-1: Construct ET network from ET relations...')
    et_dict = gen_et_network(et_path)
  
    print('Step 1-2: Construct TT network from TT relations...')
    tt_dict, word_list = gen_tt_network(tt_path)

    entity_list = list(et_dict.keys())
    entity2index = {entity:index for index, entity in enumerate(entity_list)}
    word2index = {word:index for index, word in enumerate(word_list)}
    print('Step 2-1: Convert entity-text matrix into a sparse matrix...')
    et_matrix = dict2sparse_mat(et_dict, entity2index, word2index)

    print('Step 2-2: Convert text-text matrix into a sparse matrix...')
    tt_matrix = dict2sparse_mat(tt_dict, word2index, word2index)

    print('Step 3: Perform concept expansion...')
    exp_et_matrix = et_matrix.dot(tt_matrix)

    print('Step 4-1: Save ET part of the ICE network...')
    save_ice_et_network(exp_et_matrix, entity_list, word_list, et_save_list, w)

    print('Step 4-2: Save TT part of the ICE network...')
    save_ice_tt_network(tt_dict, tt_save_list, w)

    print('Finished constructing ICE network!\n')
    
if __name__ == '__main__':
    main()
