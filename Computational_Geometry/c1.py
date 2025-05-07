import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import Delaunay, Voronoi, voronoi_plot_2d

points = np.array([[2, 3], [5, 4], [9, 6], [4, 7], [8, 1], [7, 2]])

tri = Delaunay(points)

vor = Voronoi(points)

fig, ax = plt.subplots(figsize=(8, 6))

ax.triplot(points[:, 0], points[:, 1], tri.simplices, color='blue', alpha=0.5, label="Delaunay")

voronoi_plot_2d(vor, ax=ax, show_vertices=False, line_colors='red', line_width=1, alpha=0.6)

ax.plot(points[:, 0], points[:, 1], 'ko', label="Σημεία P")

for i, (x, y) in enumerate(points):
    ax.text(x, y, f'P{i}', fontsize=12, verticalalignment='bottom', horizontalalignment='right')

ax.legend()
plt.title("Αντιστοιχία Τριγωνοποίησης Delaunay και Διαγράμματος Voronoi")
plt.show()
