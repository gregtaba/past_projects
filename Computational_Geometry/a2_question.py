import numpy as np

def orient(a, b, c):
    val = (b[1] - a[1]) * (c[0] - b[0]) - (b[0] - a[0]) * (c[1] - b[1])
    if val == 0:
        return 0  # Collinear
    elif val > 0:
        return 1  # Right turn
    else:
        return 2  # Left turn

def inc_hull(p):
    """Finds the convex hull (smallest shape around all points)."""
    if len(p) < 3:  # If less than 3 points no hull is possible
        return p

    p = sorted(set(p))  # Sort points & remove duplicates
    h = []  

    # Build lower hull 
    for pt in p:
        while len(h) > 1 and orient(h[-2], h[-1], pt) != 2:  # Remove right turns
            h.pop()
        h.append(pt)

    lower_hull = h.copy()  # Save lower hull
    h.clear()  

    # Build upper hull 
    for pt in reversed(p):
        while len(h) > 1 and orient(h[-2], h[-1], pt) != 2:  # Remove right turns
            h.pop()
        h.append(pt)
        
    return list(set(lower_hull + h))  # Combine both hulls & remove duplicates

def jarvis_march(points):
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

def find_upper_bridge(h1, h2):
    """Υπολογίζει την άνω γέφυρα μεταξύ δύο κυρτών περιβλημάτων."""
    i, j = h1.index(max(h1, key=lambda p: p[0])), h2.index(min(h2, key=lambda p: p[0]))

    while True:
        moved = False
        if orient(h1[i], h2[j], h2[(j + 1) % len(h2)]) == 2:
            j = (j + 1) % len(h2)
            moved = True
        if orient(h1[(i - 1) % len(h1)], h1[i], h2[j]) == 1:
            i = (i - 1) % len(h1)
            moved = True
        if not moved:
            break

    return i, j

def find_lower_bridge(h1, h2):
    """Υπολογίζει την κάτω γέφυρα μεταξύ δύο κυρτών περιβλημάτων."""
    i, j = h1.index(max(h1, key=lambda p: p[0])), h2.index(min(h2, key=lambda p: p[0]))

    while True:
        moved = False
        if orient(h1[i], h2[j], h2[(j - 1) % len(h2)]) == 1:
            j = (j - 1) % len(h2)
            moved = True
        if orient(h1[(i + 1) % len(h1)], h1[i], h2[j]) == 2:
            i = (i + 1) % len(h1)
            moved = True
        if not moved:
            break

    return i, j

def merge_hulls(h1, h2):
    """Συνδυάζει δύο κυρτά περιβλήματα σε ένα ενιαίο."""


    ui, uj = find_upper_bridge(h1, h2)
    li, lj = find_lower_bridge(h1, h2)

    merged = []
    idx = ui
    while idx != li:
        merged.append(h1[idx])
        idx = (idx + 1) % len(h1)
    merged.append(h1[li])

    idx = lj
    while idx != uj:
        merged.append(h2[idx])
        idx = (idx + 1) % len(h2)
    merged.append(h2[uj])

    return merged

def divide_and_conquer_hull(points):
    """Υπολογίζει το κυρτό περίβλημα με διαίρει και βασίλευε."""
    if len(points) <= 3:

        return sorted(points, key=lambda p: (p[0], p[1]))

    mid = len(points) // 2
    left = points[:mid]
    right = points[mid:]


    left_hull = divide_and_conquer_hull(left)
    right_hull = divide_and_conquer_hull(right)


    return merge_hulls(left_hull, right_hull)

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
    
    min_point = min(points, key=lambda p: (p[0], p[1]))
    max_point = max(points, key=lambda p: (p[0], p[1]))
    ls = [p for p in points if p != min_point and p != max_point and orient(min_point, max_point, p) == 2]
    rs = [p for p in points if p != min_point and p != max_point and orient(min_point, max_point, p) == 1]

    hull = set()
    hull.add(min_point)
    hull.add(max_point)
    
    add_point_to_hull(hull, ls, min_point, max_point)
    add_point_to_hull(hull, rs, max_point, min_point)

    return list(hull)


np.random.seed(42)  # For consistent results
p = [tuple(point) for point in np.random.randint(0, 101, (100, 2))] 
IA_hull = inc_hull(p)
GA_hull = jarvis_march(p)
Q_hull = quickhull(p)

p = sorted(p)  # Ταξινόμηση με βάση το x
DA_hull = divide_and_conquer_hull(p)

print(f"Q_hull={set(Q_hull)}")
print(f"GA_hull={set(GA_hull)}")
print(f"IA_hull={set(IA_hull)}")
print(f"DA_hull={set(DA_hull)}")

if set(Q_hull) == set(GA_hull) == set(IA_hull) == set(DA_hull) :
    print("All four lists contain the same elements (order ignored).")
elif set(Q_hull) == set(GA_hull) == set(IA_hull) :
    print("The divide and Conguer algorithm produces different results from the rest but the rest provide the same results")



