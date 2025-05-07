def orient(a, b, c):
    val = (b[1] - a[1]) * (c[0] - b[0]) - (b[0] - a[0]) * (c[1] - b[1])
    if val == 0:
        return 0  # Collinear
    elif val > 0:
        return 1  # Right turn
    else:
        return 2  # Left turn

def jm(points):
    """Computes the convex hull of a set of 2D points using the Jarvis March algorithm."""
    if len(points) < 3:
        return points  # Convex hull not possible with fewer than 3 points
    
    hull = []
    start = min(points)  # Find the leftmost point
    point_on_hull = start
    
    while True:
        hull.append(point_on_hull)
        endpoint = points[0]
        
        for c in points[1:]:
            ori = orient(point_on_hull, endpoint, c)
            if endpoint == point_on_hull or ori == 2:
                endpoint = c
            elif ori == 0:  # Collinear case: pick the farthest point
                if distance(point_on_hull, c) > distance(point_on_hull, endpoint):
                    endpoint = c
        
        point_on_hull = endpoint
        
        if point_on_hull == start:  # If we reached the start point, hull is complete
            break
    
    return hull 

def distance(a, b):
    """Returns the squared Euclidean distance between two points (to avoid floating point issues)."""
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

# Example set of points
points = [(0, 3), (1, 1), (2, 2), (4, 4), (0, 0), (1, 2), (3, 1), (3, 3), (0, 4), (3, 5), (0, 5)]
hull = jm(points)
print("The points used to create the Hull will be:", sorted(hull))
