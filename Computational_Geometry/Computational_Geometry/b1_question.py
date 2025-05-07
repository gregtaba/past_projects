import numpy as np
from scipy.optimize import linprog

def solve_lp(c, A, b, bounds):
    """
    Solve linear programming problem with given objective function,
    constraints, and variable bounds.
    """
    res = linprog(c, A_ub=A, b_ub=b, bounds=bounds, method='highs')
    return res

def print_solution(res):
    """Print the results of the linear programming solution."""
    if res.success:
        print(f"The optimal solution is x1 = {res.x[0]}, x2 = {res.x[1]}, with objective value {-res.fun}")
    else:
        print("No feasible solution found.")

# Objective function coefficients (e.g., max {3x1 - 10x2} -> minimize -3x1 + 10x2)
c = [-3, 10]

# All constraints
A_all = [
    [-2, 1],    # -2x1 + x2 ≤ 12
    [-1, 3],    # -x1 + 3x2 ≤ 3
    [6, 7],     # 6x1 + 7x2 ≤ 18
    [3, -12],   # 3x1 - 12x2 ≤ -8
    [2, -7],    # 2x1 - 7x2 ≤ 35
    [-1, 8],    # -x1 + 8x2 ≤ 29
    [2, -6]     # 2x1 - 6x2 ≤ 9
]
b_all = [12, 3, 18, -8, 35, 29, 9]

# Variable bounds (x1, x2 ≥ 0)
bounds = [(0, None), (0, None)]

# Loop through constraints progressively
for i in range(2, 8):
    # Take the first i elements of A_all and b_all
    A_cur = A_all[:i]
    b_cur = b_all[:i]
    
    # Solve the linear programming problem with the current set of constraints
    print(f"Solving with {i} constraints:")
    res = solve_lp(c, A_cur, b_cur, bounds)
    print_solution(res)
    
    # If it's the last iteration, compare solutions
    if i == 7:
        if res.success:
            print("\nFinal solution after adding all constraints:")
            print(f"Optimal solution: x1 = {res.x[0]}, x2 = {res.x[1]}, with objective value {-res.fun}")
