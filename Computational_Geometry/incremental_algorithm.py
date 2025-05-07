import numpy as np

def orient(a, b, c):
    """Find if a, b, c make a left turn, right turn, or are in a line."""
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

p = [(0, 3), (1, 1), (2, 2), (4, 4), (0, 0), (1, 2), (3, 1), (3, 3), (0, 4), (3, 5)]
hull = inc_hull(p)
print("The points used to create the Hull will be:", sorted(hull))
