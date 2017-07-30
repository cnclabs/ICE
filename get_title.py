import json
import requests
from bs4 import BeautifulSoup, NavigableString
from collections import Counter
import time

# with open('single_genre.json') as f:
#     data = json.load(f)

# for cate in data:
#     for instance in cate['values']:
#         if instance['cate'] == 'item':
#             instance['shape'] = 'circle'
#             print(instance['imdb_id'])
#             url = "http://www.imdb.com/title/" + instance['imdb_id']
#             geturl = requests.get(url).text
#             soup = BeautifulSoup(geturl, "lxml")
#             main_content = soup.select('div.poster img[src]')
#             for s in main_content:
#                 ss = s['src']
#                 print(ss)
#             instance['src'] = ss
#             time.sleep(0.5)
#         else:
#             instance['shape'] = 'triangle-up'

# with open('single_genre_new.json', 'w') as f:
#     json.dump(data, f, indent=4)

with open('single_genre.json') as f:
    data = json.load(f)

title = {}
for cate in data:
    for instance in cate['values']:
        title[instance['title']] = None

# word_count = dict(Counter(title))
# word_count = [x for x,y in word_count.items() if y > 1]
# print(word_count)
# print(title)

with open('a.json', 'w') as f:
    json.dump(title, f, indent=4)
