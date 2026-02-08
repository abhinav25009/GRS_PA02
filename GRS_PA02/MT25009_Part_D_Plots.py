import csv
import matplotlib.pyplot as plt

THREAD = 1
VERSIONS = ["A1", "A2", "A3"]

data = {v: {"size": [], "cycles": [], "cache": [], "l1": [], "ctx": []} for v in VERSIONS}

with open("results.csv") as f:
    reader = csv.DictReader(f)

    for row in reader:
        if int(row["threads"]) != THREAD:
            continue

        v = row["version"]

        data[v]["size"].append(int(row["size"]))
        data[v]["cycles"].append(int(row["cycles"]))
        data[v]["cache"].append(int(row["cache_misses"]))
        data[v]["l1"].append(int(row["l1_misses"]))
        data[v]["ctx"].append(int(row["context_switches"]))


def plot_metric(key, filename, ylabel):
    plt.figure()

    for v in VERSIONS:
        plt.plot(data[v]["size"], data[v][key], marker='o', label=v)

    plt.xscale("log")
    plt.xlabel("Message Size (bytes)")
    plt.ylabel(ylabel)
    plt.legend()
    plt.grid()

    plt.savefig(filename)
    plt.close()


plot_metric("cycles", "cycles.png", "CPU Cycles")
plot_metric("cache", "cache_misses.png", "Cache Misses")
plot_metric("l1", "l1_misses.png", "L1 Misses")
plot_metric("ctx", "context_switches.png", "Context Switches")

print("Plots generated")
