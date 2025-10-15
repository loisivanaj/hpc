import matplotlib.pyplot as plt

n = [32, 16, 8, 4, 2, 1]
m = [1, 2, 4, 8, 16, 32]
mean = [20.25, 23.37, 26.48, 28.12, 29.52, 32]
median = [20, 23, 26, 28, 30, 32]
stddev = [1.90984, 1.79251, 1.55229, 1.71044, 1.55872, 0]

fig, ax1 = plt.subplots(figsize=(8,5))
ax2 = ax1.twinx()

ax1.plot(n, mean, 'o-', label="Mean", color='tab:blue')
ax1.plot(n, median, 's--', label="Median", color='tab:green')
ax2.plot(n, stddev, 'd-.', label="Std Dev", color='tab:red')

ax1.set_xlabel("Cache n (sets)")
ax1.set_ylabel("Mean / Median", color='black')
ax2.set_ylabel("Std Dev", color='tab:red')
ax1.set_title("Cache Performance Metrics vs. Dimensions")

ax1.invert_xaxis()  # optional, to show n decreasing
ax1.legend(loc='upper left')
ax2.legend(loc='upper right')

plt.tight_layout()
plt.savefig("cache_associativity_metrics.png", dpi=300)
plt.close()

