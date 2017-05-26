from pyICE import pyICE

ice = pyICE()
# ice.load_edge_list('./network.edgelist')
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
