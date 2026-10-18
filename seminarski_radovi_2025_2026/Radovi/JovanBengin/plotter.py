from matplotlib import pyplot as plt

fig, ax = plt.subplots(2, 2, figsize=(10, 10))

with open("logs/output_log_solver_large.txt", "r") as log_file:
	sample_counts = {}
	solver_times = {}

	for line in log_file:
		vals = line.split(" ")
		sample_counts[vals[0]] = sample_counts.get(vals[0], 0) + 1
		solver_times[vals[0]] = solver_times.get(vals[0], 0) + int(vals[2])

	solver_times_int = {}
	for key in solver_times:
		solver_times_int[int(key)] = solver_times[key] / sample_counts[key]

	solver_times_int = dict(sorted(solver_times_int.items()))

	ax[0,0].plot(solver_times_int.keys(), solver_times_int.values(), label = "solver")
	ax[0,0].set_xlabel("N")
	ax[0,0].set_ylabel("Solver run time (ms)")
	ax[0,0].legend()
	ax[0,0].set_title("Solver run time on large datasets")

with open("logs/output_log_mst_large.txt", "r") as log_file:
	sample_counts = {}
	solver_times = {}

	for line in log_file:
		vals = line.split(" ")
		sample_counts[(vals[0], vals[1])] = sample_counts.get(vals[0], 0) + 1
		solver_times[(vals[0], vals[1])] = solver_times.get(vals[0], 0) + int(vals[2])

	for key in solver_times:
		solver_times[key] /= sample_counts[key]

	prim_times = {}
	kruskal_times = {}

	for key in solver_times:
		if(key[1] == "prim"):
			prim_times[int(key[0])] = solver_times[key]
		if(key[1] == "kruskal"):
			kruskal_times[int(key[0])] = solver_times[key]

	prim_times = dict(sorted(prim_times.items()))
	kruskal_times = dict(sorted(kruskal_times.items()))

	ax[0,1].plot(prim_times.keys(), prim_times.values(), label = "prim")
	ax[0,1].plot(kruskal_times.keys(), kruskal_times.values(), label = "kruskal")
	ax[0,1].set_xlabel("N")
	ax[0,1].set_ylabel("MST run time (ms)")
	ax[0,1].legend()
	ax[0,1].set_title("MST run time on large datasets")
	#ax[0,1].set_xscale('linear')

with open("logs/output_log_small.txt", "r") as log_file:
	sample_counts = {}
	solver_times = {}

	for line in log_file:
		vals = line.split(" ")
		sample_counts[(vals[0], vals[1])] = sample_counts.get(vals[0], 0) + 1
		solver_times[(vals[0], vals[1])] = solver_times.get(vals[0], 0) + int(vals[2])

	for key in solver_times:
		solver_times[key] /= sample_counts[key]

	prim_times = {}
	kruskal_times = {}
	naive_times = {}

	for key in solver_times:
		if(key[1] == "prim"):
			prim_times[int(key[0])] = solver_times[key]
		if(key[1] == "kruskal"):
			kruskal_times[int(key[0])] = solver_times[key]
		if(key[1] == "naive"):
			naive_times[int(key[0])] = solver_times[key]

	prim_times = dict(sorted(prim_times.items()))
	kruskal_times = dict(sorted(kruskal_times.items()))
	naive_times = dict(sorted(naive_times.items()))

	#ax[1,0].plot(prim_times.keys(), prim_times.values(), label = "prim")
	ax[1,0].plot(kruskal_times.keys(), kruskal_times.values(), label = "kruskal")
	ax[1,0].plot(naive_times.keys(), naive_times.values(), label = "naive")
	ax[1,0].set_title("Total run time on small datasets")
	ax[1,0].legend()
	ax[1,0].set_xlabel("N")
	ax[1,0].set_ylabel("Run time (ms)")

with open("logs/output_log_small.txt", "r") as log_file:
	sample_counts = {}
	solver_times = {}

	for line in log_file:
		vals = line.split(" ")
		sample_counts[(vals[0], vals[1])] = sample_counts.get(vals[0], 0) + 1
		solver_times[(vals[0], vals[1])] = solver_times.get(vals[0], 0) + int(vals[2])

	for key in solver_times:
		solver_times[key] /= sample_counts[key]

	prim_times = {}
	kruskal_times = {}
	naive_times = {}

	for key in solver_times:
		if(key[1] == "prim"):
			prim_times[int(key[0])] = solver_times[key]
		if(key[1] == "kruskal"):
			kruskal_times[int(key[0])] = solver_times[key]
		if(key[1] == "naive"):
			naive_times[int(key[0])] = solver_times[key]

	prim_times = dict(sorted(prim_times.items()))
	kruskal_times = dict(sorted(kruskal_times.items()))
	naive_times = dict(sorted(naive_times.items()))

	#ax[1,0].plot(prim_times.keys(), prim_times.values(), label = "prim")
	ax[1,1].plot(kruskal_times.keys(), kruskal_times.values(), label = "kruskal")
	ax[1,1].plot(naive_times.keys(), naive_times.values(), label = "naive")
	ax[1,1].set_title("Total run time on small datasets (log scaled)")
	ax[1,1].legend()
	ax[1,1].set_xlabel("N")
	ax[1,1].set_ylabel("Run time (ms)")
	ax[1,1].set_yscale('log')

plt.show()