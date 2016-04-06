import matplotlib.pyplot as plt

gaussian_numbers = [ 1, 2,4 , 5 , 4, 5,4, 3,2,4, 5,5, 8, 4,4, 2, 3, 4, 4,5, 2,3, 1, 3, 4, 3,9,5,6,3,7,6,11,8,11,1,10 ]
plt.hist(gaussian_numbers)
plt.title("Occurrence of Count")
plt.xlabel("Count")
plt.ylabel("Frequency")
plt.show()