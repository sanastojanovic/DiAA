import matplotlib.pyplot as plt
import numpy as np

labels = ['1k/1k', '10k/1k', '10k/10k', '100k/10k', '100k/100k', '100k/1M']
trie_times = [0.005, 0.023, 0.027, 0.110, 0.181, 0.775]
naive_times = [0.008, 0.056, 0.300, 3.131, 31.197, 315.048]

x = np.arange(len(labels))

plt.figure(figsize=(10, 6))
plt.plot(x, trie_times, marker='o', label='Weighted Trie', linewidth=3)
plt.plot(x, naive_times, marker='s', label='Naive Search', linewidth=2, linestyle='--')

plt.yscale('log')
plt.xticks(x, labels)
plt.xlabel('Veličina rečnika / Broj upita')
plt.ylabel('Vreme (s) - Logaritamska skala')
plt.title('Benchmark: Trie vs Naive')
plt.legend()
plt.grid(True, which="both", ls="-", alpha=0.3)

plt.tight_layout()
plt.savefig('benchmark_graph.png')
plt.show()