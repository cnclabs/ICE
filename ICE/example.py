from pyICE import pyICE

ice = pyICE()
ice.LoadEdgeList('./network.edgelist')
ice.Init(dimension=4)
ice.Train(sample_times=10, negative_samples=5, alpha=0.025, worker=1)
ice.SaveWeights(model_name='ICE.rep')

