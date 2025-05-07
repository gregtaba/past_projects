import numpy as np
import matplotlib.pyplot as plt


x = np.linspace(0, 10, 400)


upper_constraint_1 = 2*x + 12               # y <= 2x + 12
lower_constraint_1 = (1/4)*x + 2/3         # y >= (1/4)x + 2/3
upper_constraint_2 = (1/3)*x + 1           # y <= (1/3)x + 1
upper_constraint_3 = (-6/7)*x + (18/7)     # y <= (-6/7)x + 18/7
lower_constraint_2 = (1/3)*x - 3/2         # y >= (1/3)x - 3/2
upper_constraint_4 = (1/8)*x + (29/8)      # y <= (1/8)x + 29/8
lower_constraint_3 = (2/7)*x - 5

plt.figure(figsize=(8, 6))
plt.xlim(0, 10)
plt.ylim(0, 20)


plt.plot(x, upper_constraint_1, 'b-', label=r'$y \leq 2x + 12$')
plt.plot(x, lower_constraint_1, 'r-', label=r'$y \geq \frac{1}{4}x + \frac{2}{3}$')
plt.plot(x, upper_constraint_2, 'g-', label=r'$y \leq \frac{1}{3}x + 1$')
plt.plot(x, upper_constraint_3, 'purple', label=r'$y \leq \frac{-6}{7}x + \frac{18}{7}$')
plt.plot(x, lower_constraint_2, 'orange', label=r'$y \geq \frac{1}{3}x - \frac{3}{2}$')
plt.plot(x, upper_constraint_4, 'brown', label=r'$y \leq \frac{1}{8}x + \frac{29}{8}$')
plt.plot(x, lower_constraint_3, 'pink', label=r'$y \geq \frac{2}{7}x - 5$')


plt.axhline(0, color='black', linewidth=1)
plt.axvline(0, color='black', linewidth=1)


upper_bound = np.minimum(np.minimum(np.minimum(upper_constraint_1, upper_constraint_2), upper_constraint_3), upper_constraint_4) 
lower_bound = np.maximum(np.maximum(lower_constraint_1, lower_constraint_2), lower_constraint_3) 


plt.fill_between(x, lower_bound, upper_bound, where=(lower_bound <= upper_bound), alpha=0.2, color='gray')


plt.xlabel(r'$x$')
plt.ylabel(r'$y$')
plt.title('Εφικτή Περιοχή Γραμμικού Προγραμματισμού')
plt.grid()
plt.legend(loc='upper left')
plt.show()
