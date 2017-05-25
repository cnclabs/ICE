from pyICE import pyICE

ice = pyICE()
#ice.LoadEdgeList('./network.edgelist')
graph = {
          '五月天':         {'Taiwanese':1, 'rock':1, 'band':1},
          'MAYDAY@':        {'Taiwanese':1, 'rock':1, 'band':1},
          'Sodagreen':      {'Taiwanese':1, 'indie':1, 'pop_rock':1, 'band':1},
          'SEKAI_NO_OWARI': {'Japanese':1, 'indie':1, 'pop_rock':1, 'band':1},
          'The_Beatles':    {'England':1, 'rock':1, 'pop':1 }
        }
ice.LoadDict(graph)
ice.Init(dimension=4)
ice.Train(sample_times=10, negative_samples=5, alpha=0.025, worker=1)
ice.SaveWeights(model_name='ICE.rep')

