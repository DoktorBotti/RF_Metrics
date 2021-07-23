import os, subprocess, time
import numpy as np
# normal configuration variables
test_files_dir = '/rf_metrics/BS/'
project_dir = os.path.pardir
our_exe = project_dir + "/bin/commandline_rf"
result_file_path = project_dir + '/benchmark_ours/'
metrics = ["MCI", "MSI", "SPI"]
timeout_in_secs = 60 * 8 # 12 minutes
test_names = [file for file in os.listdir(test_files_dir)]
test_paths = [test_files_dir + a for a in test_names]

# configure which testset to execute
upper_taxa_bound = 1000
tree_counts  = [2,10,50,100,130] # all measurements will be performed with these counts
# test matrix dimensions [treeNum][taxa/testfile][metric]

def writeToFile(file_names, ours_times):
    # write results to file
    timestamp = str(int(time.time()))
    with open(result_file_path + 'overview_ours_'+ timestamp+ '.txt', "w") as file:
        file.write(f'Evaluated the following samples: {file_names}\n')
        file.write(f"Evaluated with the following tree numbers per file: {tree_counts}\n")
        for t_idx, trees in enumerate(tree_counts):
            file.write(f"Calculated with {trees} trees:\n")
            for m_idx, metric in enumerate(metrics):
                file.write(metric + " scores:\n")
                # prettify results by converting to seconds
                ours_out = [str(t * 1e-9) for t in ours_times[t_idx,:,m_idx]]

                file.write("\tOur result: ")
                file.write(str(ours_out) + '\n')

        file.write('\n' + print_ex_times(ours_times))
    # write raw matrix with timestamp
    np.save(result_file_path +'ours_timingMatrix'+timestamp+'.npy', ours_times)
def print_single_instance(ours):
    return f"( {np.average(ours) * 1e-9 } )"

def print_ex_times(ours_times):
    res = "Mean values: "
    mci_ours, msi_ours, spi_ours = np.split(ours_times,3, axis=2)
    res = res + "MCI " + print_single_instance(mci_ours)
    res = res + " MSI " + print_single_instance(msi_ours)
    res = res + " SPI " + print_single_instance(spi_ours)
    res = res + "\n"
    return res

# define result array for timing results
ours_times = np.zeros(shape=(len(tree_counts), len(test_names),len(metrics)))
file_names = []

# make ugly stuff to hide console output from both executables
FNULL = open(os.devnull)
try:
    # for each input file
    for num, path in enumerate(test_paths):
        try:
            if(int(test_names[num])>=upper_taxa_bound):
                continue
        except ValueError:
                print("Value error while reading taxa count. Skipping")
                continue
        # create temporary file which holds correct number of trees
        for trees_idx, num_trees in enumerate(tree_counts):
            with open(path, "r") as all_trees:
                with open("/tmp/tmp.trees", "w") as out_trees:
                    head = [next(all_trees) for x in range(num_trees)]
                    out_trees.writelines(head)
            # temporaries which would not print into main array if something fails
            ours_metric_arr =  np.zeros(shape=(len(metrics),))
            for m_idx, metric in enumerate(metrics):
                status_string = f"processing {metric} in {test_names[num]}, {num+1} of {len(test_paths)}, with {num_trees} trees. "
                # run our script
                ours_args = [our_exe, '--metric', metric, '-o', '/tmp/ourRes.txt', '-i', "/tmp/tmp.trees"]
                print(status_string+ "Running ours:")
                runtime = 0
                try:
                    our_start = time.time_ns()
                    proc = subprocess.run(ours_args, shell=False, stdout=FNULL, timeout=timeout_in_secs).returncode
                    our_end = time.time_ns()
                    print(f"Took {(our_end - our_start) * 1e-9} seconds")
                    runtime = our_end - our_start
                except subprocess.TimeoutExpired as exc:
                    print("Timeout!")
                    runtime = -2.
                # writing to array upon successful calculation
                if proc != 0:
                    print("error occured!")
                    ours_metric_arr[m_idx] = -1.
                else:
                    ours_metric_arr[m_idx] = runtime

            #push when all metrics are calculated
            ours_times[trees_idx, num] = ours_metric_arr
            print(print_ex_times(ours_times))

    file_names.append(test_names[num])

except KeyboardInterrupt:
    print("\nexiting")
    writeToFile(file_names, ours_times)

writeToFile(file_names, ours_times)
