import argparse
import json
import pandas as pd
import matplotlib.pyplot as plt


def read_ccdb_perf_log(filename):
    result = []
    with open(filename) as f:
        for line in f:
            if not line.startswith("CCDB_PERF_LOG:"):
                continue

            if " total" in line:
                continue

            line = line[len("CCDB_PERF_LOG:"):]
            row = json.loads(line)
            row["path"]=row['descr'].split('=>')[1]
            result.append(row)

    return result


def print_full(x):
    pd.set_option('display.max_rows', len(x))
    print(x)
    pd.reset_option('display.max_rows')


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('filename')
    args = parser.parse_args()

    data = read_ccdb_perf_log(args.filename)

    print("Total CCDB requests: ", len(data))
    df = pd.DataFrame(data)

    # we don't need these columns for now
    df.drop('t_units', axis=1, inplace=True)
    df.drop('descr', axis=1, inplace=True)
    df.drop('thread_id', axis=1, inplace=True)
    df.drop('start_stamp', axis=1, inplace=True)
    df.sort_values('elapsed', ascending=False, inplace=True)

    print("Total time reading from CCDB [s]: ", df.elapsed.sum() / 1000000.0)

    print("Top of the longest queires:")
    print_full(df.head(15))

    counted_paths = df.path.value_counts().reset_index().rename(columns={'index': 'path', 'path': 'count'})
    print("\nRequests by path: ")
    print_full(counted_paths)

    # Show histogram
    (df.elapsed / 1000).plot.hist(alpha=0.5, bins=30)
    plt.show()

