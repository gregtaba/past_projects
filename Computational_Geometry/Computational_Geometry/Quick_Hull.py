def orient(a, b, c):
    """Find if a, b, c make a left turn, right turn, or are in a line."""
    val = (b[1] - a[1]) * (c[0] - b[0]) - (b[0] - a[0]) * (c[1] - b[1])
    if val == 0:
        return 0  # Collinear
    elif val > 0:
        return 1  # Right turn
    else:
        return 2  # Left turn

def quickhull(points):
    if len(points) < 3:
        return points

    def add_point_to_hull(hull, points, p1, p2):
        if not points:
            return
        
        p = max(points, key=lambda point: distance_point_to_line(point, p1, p2))
        hull.add(p)
        
        points1 = [point for point in points if orient(p1, p, point) == 2]
        points2 = [point for point in points if orient(p, p2, point) == 2]
        
        add_point_to_hull(hull, points1, p1, p)
        add_point_to_hull(hull, points2, p, p2)

    def distance_point_to_line(p, a, b):
        return abs((b[1] - a[1]) * p[0] - (b[0] - a[0]) * p[1] + b[0] * a[1] - b[1] * a[0]) / ((b[1] - a[1]) ** 2 + (b[0] - a[0]) ** 2) ** 0.5
    
    min_p = min(points, key=lambda p: (p[0], p[1]))
    max_p = max(points, key=lambda p: (p[0], p[1]))
    ls = [p for p in points if p != min_p and p != max_p and orient(min_p, max_p, p) == 2]
    rs = [p for p in points if p != min_p and p != max_p and orient(min_p, max_p, p) == 1]

    hull = set()
    hull.add(min_p)
    hull.add(max_p)
    
    add_point_to_hull(hull, ls, min_p, max_p)
    add_point_to_hull(hull, rs, max_p, min_p)

    return list(hull)

# Example usage
points = [(0, 3), (1, 1), (2, 2), (4, 4), (0, 0), (1, 2), (3, 1), (3, 3), (0, 4), (3, 5), (0, 5)]
hull = quickhull(points)
print("Hull:", hull)
