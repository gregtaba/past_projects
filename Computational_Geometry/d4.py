import matplotlib.pyplot as plt
import numpy as np

# Function to check whether a point is within the rectangular area
def check_points_in_rectangle(points, xmin, ymin, xmax, ymax):
    ip = []
    for x, y in points:
        if xmin <= x <= xmax and ymin <= y <= ymax:
            ip.append((x, y))
    return ip

#Function to visualize points
def plot_points(all_points, ip, xmin, ymin, xmax, ymax):
    plt.figure(figsize=(8, 8))
    plt.scatter(all_points[:, 0], all_points[:, 1], color='blue', label='Points outside')
    if ip:
        ip = np.array(ip)
        plt.scatter(ip[:, 0], ip[:, 1], color='red', label='Points within')
    plt.plot([xmin, xmin, xmax, xmax, xmin],
             [ymin, ymax, ymax, ymin, ymin], 'g--', label='Rectangular area')
    plt.legend()
    plt.title('Points Inside and Outside the Rectangular Area')
    plt.xlabel('X Coordinates')
    plt.ylabel('Y Coordinates')
    plt.grid(True)
    plt.show()

#Main part of the program
num_points = 150
xmin, ymin, xmax, ymax = 20, 20, 80, 80

points = np.random.rand(num_points, 2) * 100
ip = check_points_in_rectangle(points, xmin, ymin, xmax, ymax)


print("Points within the rectangular area :")
for point in ip:
    print(f"({point[0]:.2f}, {point[1]:.2f})")

plot_points(points, ip, xmin, ymin, xmax, ymax)
