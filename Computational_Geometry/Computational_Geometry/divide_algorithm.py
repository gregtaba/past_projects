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

def find_upper_bridge(h1, h2):
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
    ui, uj = find_upper_bridge(h1, h2)
    li, lj = find_lower_bridge(h1, h2)

    m = []
    idx = ui
    while idx != li:
        m.append(h1[idx])
        idx = (idx + 1) % len(h1)
    m.append(h1[li])

    idx = lj
    while idx != uj:
        m.append(h2[idx])
        idx = (idx + 1) % len(h2)
    m.append(h2[uj])

    return m

def divide_and_conquer_hull(points):
    if len(points) <= 3:

        return sorted(points, key=lambda p: (p[0], p[1]))

    mid = len(points) // 2
    left = points[:mid]
    right = points[mid:]

    left_hull = divide_and_conquer_hull(left)
    right_hull = divide_and_conquer_hull(right)

    return merge_hulls(left_hull, right_hull)



# Παράδειγμα χρήσης:
p = [(0, 3), (1, 1), (2, 2), (4, 4), (0, 0), (1, 2), (3, 1), (3, 3), (0, 4), (3, 5), (0, 5)]
p = sorted(p)  # Ταξινόμηση με βάση το x
hull = divide_and_conquer_hull(p)
print("Hull:", hull)
