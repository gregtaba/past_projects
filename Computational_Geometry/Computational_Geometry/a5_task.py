import numpy as np

def cross_product(o, a, b):
    """Computes the cross product of vectors OA and OB."""
    o, a, b = np.array(o), np.array(a), np.array(b)  # Ensure numpy arrays
    return np.cross(a - o, b - o)

def distance_point_to_plane(p, a, b, c):
    """Computes the signed distance of point p from the plane defined by a, b, c."""
    p, a, b, c = np.array(p), np.array(a), np.array(b), np.array(c)
    normal = cross_product(a, b, c)
    nm = np.linalg.norm(normal)
    if nm < 1e-6:  # Avoid division by zero
        return 0
    return np.dot(normal, p - a) / nm

def add_point_to_hull(points, hull, a, b, c):
    """Recursive function to add points to the convex hull."""
    if not points:
        return

    # Convert inputs to numpy arrays
    a, b, c = np.array(a), np.array(b), np.array(c)
    
    # Find the farthest point from the plane (a, b, c)
    fp = max(points, key=lambda p: abs(distance_point_to_plane(p, a, b, c)))
    fp = np.array(fp)  # Ensure it's a NumPy array

    # print(f"Checking face ({tuple(a)}, {tuple(b)}, {tuple(c)}): Farthest point = {tuple(fp)}")

    if abs(distance_point_to_plane(fp, a, b, c)) < 1e-9:
        # print("Stopping recursion: Farthest point is too close to the plane.")
        return  # Stop recursion if the farthest point is too close to the plane

    # Separate points that lie outside the current plane
    op = [np.array(p) for p in points if distance_point_to_plane(p, a, b, c) > 1e-5]

    # print(f"Points outside ({tuple(a)}, {tuple(b)}, {tuple(c)}): {len(op)} points")

    # Recursively add new faces
    add_point_to_hull(op, hull, a, b, fp)
    add_point_to_hull(op, hull, a, fp, c)
    add_point_to_hull(op, hull, fp, b, c)

    # print(f"Adding point to hull: {tuple(fp)}")
    hull.append(tuple(fp))  # Convert back to tuple for consistency

def quickhull_3d(points):
    """Computes the convex hull using the QuickHull algorithm in 3D."""
    hull = []

    # Convert points to NumPy array
    points = np.array(points)

    # Find four initial extreme points forming a base tetrahedron
    extreme_points = [
        points[np.argmin(points[:, 0])], points[np.argmax(points[:, 0])],  # Min & max x
        points[np.argmin(points[:, 1])], points[np.argmax(points[:, 1])],  # Min & max y
        points[np.argmin(points[:, 2])], points[np.argmax(points[:, 2])]   # Min & max z
    ]

    # Remove duplicate points
    extreme_points = list(map(tuple, set(map(tuple, extreme_points))))
    
    # print(f"Extreme points selected: {extreme_points}")

    if len(extreme_points) < 4:
        # print("Not enough points to form a convex hull. Returning extreme points.")
        return extreme_points  # Return points if not enough to form a tetrahedron

    a, b, c, d = map(np.array, extreme_points[:4])  # Ensure they are NumPy arrays

    # Separate remaining points into outside & inside sets
    remaining_points = [p for p in points if tuple(p) not in extreme_points]

    # print(f"Remaining points for processing: {len(remaining_points)}")

    add_point_to_hull(remaining_points, hull, a, b, c)
    add_point_to_hull(remaining_points, hull, a, b, d)
    add_point_to_hull(remaining_points, hull, a, c, d)
    add_point_to_hull(remaining_points, hull, b, c, d)

    # print(f"Final hull before removing duplicates: {hull}")

    return sorted(list(map(tuple, set(map(tuple, hull)))))  # Remove duplicates

np.random.seed(42)  # For reproducibility
points = np.random.rand(80, 3) * 100

convex_hull = quickhull_3d(points)
print("\nConvex Hull Points:")
for point in convex_hull:
    print(point)
