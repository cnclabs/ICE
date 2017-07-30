import json
from annoy import AnnoyIndex

with open('single_genre.json') as f:
    data = json.load(f)

points = {}
item = []
vecs = []
result = {}
# print(data)
for cate in data:
    for instance in cate['values']:
        points[instance['title']] = [instance['x'], instance['y']]

# print(points)
for key, value in points.items():
    item.append(key)
    vecs.append(value)

annoy_indexer = AnnoyIndex(2)
for i in range(len(item)):
    annoy_indexer.add_item(i, vecs[i])

annoy_indexer.build(1000)

for i in range(len(item)):
    neighbors = annoy_indexer.get_nns_by_item(i, 20)
    neighbors = [item[x] for x in neighbors]
    result[item[i]] = neighbors

print(result)

with open('neighbors.json', 'w') as f:
    json.dump(result, f, indent=4)
