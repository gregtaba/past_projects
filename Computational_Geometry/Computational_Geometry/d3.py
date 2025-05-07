class KDNode:
    def __init__(self, point, left=None, right=None):
        self.point = point
        self.left = left
        self.right = right

def build_kdtree(points, depth=0):
    if not points:
        return None

    k = len(points[0])  #We assume that all points have the same dimension
    axis = depth % k

    points.sort(key=lambda x: x[axis])
    median = len(points) // 2

    return KDNode(
        point=points[median],
        left=build_kdtree(points[:median], depth + 1),
        right=build_kdtree(points[median + 1:], depth + 1)
    )

def range_search(node, rect, depth=0):
    if node is None:
        return []

    k = len(node.point)
    axis = depth % k
    result = []

    if (all(rect[i][0] <= node.point[i] <= rect[i][1] for i in range(k))):
        result.append(node.point)

    if node.left and node.point[axis] >= rect[axis][0]:
        result.extend(range_search(node.left, rect, depth + 1))
    if node.right and node.point[axis] <= rect[axis][1]:
        result.extend(range_search(node.right, rect, depth + 1))

    return result


points = [(2, 3),(4, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]
tree = build_kdtree(points)
rect = [(3, 9), (2, 5)]
found_points = range_search(tree, rect)
print("Points within the rectangle:", found_points)
