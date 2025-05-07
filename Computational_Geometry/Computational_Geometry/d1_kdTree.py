class Node:
    def __init__(self, point, left=None, right=None):
        self.point = point
        self.left = left
        self.right = right

def build_kdtree(points, depth=0):
    if not points:
        return None
    
    # Choosing the dimension for partitioning
    k = len(points[0])  # Can handle any dimensionality of points
    axis = depth % k
    
    #Classification of points by selected dimension
    points.sort(key=lambda x: x[axis])
    median = len(points) // 2  #Find the midpoint
    
    # Creation of a new node and retrospective construction of subtrees
    return Node(
        point=points[median],
        left=build_kdtree(points[:median], depth + 1),
        right=build_kdtree(points[median + 1:], depth + 1)
    )

#Example use
points = [(4, 5), (18, 15), (14, 15), (6, 12), (8, 3), (2, 7), (10, 20)]
kdtree = build_kdtree(points)

def print_kdtree(node, depth=0):
    if node is not None:
        print("  " * depth + str(node.point))
        print_kdtree(node.left, depth + 1)
        print_kdtree(node.right, depth + 1)

print_kdtree(kdtree)
