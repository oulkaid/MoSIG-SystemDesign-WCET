import networkx as nx
import matplotlib.pyplot as plt
from csv import reader

g = nx.DiGraph()
with open('summarize.csv', 'r') as read_obj:
    csv_reader = reader(read_obj)
    header = next(csv_reader)
    if header != None:
        for row in csv_reader:
            if (int(row[0]) < 7): #NOTE: We are only taking the first 7 elements as an example
                if (int(row[1]) != -1):
                    g.add_edge(int(row[0]), int(row[1]))
                if ((int)(row[2]) != -1):
                    g.add_edge(int(row[0]), int(row[2]))


nx.draw(g,with_labels=True)
nx.bipartite_layout(g, g, align='vertical', scale=1, center=None, aspect_ratio=100)
plt.draw()
plt.show()
