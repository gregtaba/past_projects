import matplotlib.pyplot as plt
import numpy as np

class Node:
    def __init__(self, point, left=None, right=None, axis=None):
        self.point = point
        self.left = left
        self.right = right
        self.axis = axis  # 0 for x-axis, 1 for y-axis

def build_kdtree(points, depth=0):
    if not points:
        return None
    
    k = len(points[0])  # 2 dimensions (x, y)
    axis = depth % k  # Alternate between x and y
    
    points.sort(key=lambda x: x[axis])
    median = len(points) // 2
    
    print(f"Depth {depth}: Splitting at {points[median]} along {'x' if axis == 0 else 'y'}-axis")
    
    return Node(
        point=points[median],
        left=build_kdtree(points[:median], depth + 1),
        right=build_kdtree(points[median + 1:], depth + 1),
        axis=axis
    )

def plot_kdtree(node, xmin, xmax, ymin, ymax, depth=0):
    if node is None:
        return
    
    x, y = node.point
    axis = node.axis
    
    if axis == 0:  # Vertical split (x-axis)
        plt.plot([x, x], [ymin, ymax], 'r--')
        plot_kdtree(node.left, xmin, x, ymin, ymax, depth + 1)
        plot_kdtree(node.right, x, xmax, ymin, ymax, depth + 1)
    else:  # Horizontal split (y-axis)
        plt.plot([xmin, xmax], [y, y], 'b--')
        plot_kdtree(node.left, xmin, xmax, ymin, y, depth + 1)
        plot_kdtree(node.right, xmin, xmax, y, ymax, depth + 1)
    
    plt.plot(x, y, 'go')  # Plot the point
    plt.text(x, y, f"{(x, y)}", fontsize=10, verticalalignment='bottom')

# Example points
points = [(4, 5), (18, 15), (14, 15), (6, 12), (8, 3), (2, 7), (10, 20)]
kdtree = build_kdtree(points)

# Plot the KD-Tree
plt.figure(figsize=(8, 6))
plt.xlim(0, 20)
plt.ylim(0, 20)
plot_kdtree(kdtree, 0, 20, 0, 20)
plt.xlabel("x-axis")
plt.ylabel("y-axis")
plt.title("KD-Tree Visualization")
plt.show()
