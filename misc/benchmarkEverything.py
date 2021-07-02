import os, subprocess, time
import numpy as np
# normal configuration variables
test_files_dir = '/rf_metrics/BS/'
project_dir = os.path.pardir
our_exe = project_dir + "/bin/commandline_rf"
reference_script = 'referenceTiming.R'
metrics = ["MCI", "MSI", "SPI"]
test_names = [file for file in os.listdir(test_files_dir)]
test_paths = [test_files_dir + a for a in test_names]

# configure which testset to execute
upper_taxa_bound = 2000
tree_counts  = [2,4,10,50,100,1000] # all measurements will be performed with these counts
calc_metrics = [True, True, True] # if MCI, MSI, SPI should be computed respectively
exclude_test_files = []

# test matrix dimensions [treeNum][taxa/testfile][metric]

def writeToFile(file_names, ours_times, reference_times):
    # write results to file
    with open('Unopt_preview_refVSus.txt', "w") as file:
        file.write(f"Evaluated the following samples: {file_names}\n")
        file.write(f"Evaluated with the following tree numbers per file: {tree_counts}\n")
        for t_idx, trees in enumerate(tree_counts):
            file.write(f"Calculated with {trees} trees:\n")
            for m_idx, metric in enumerate(metrics):
                file.write(metric + " scores:\n")
                # prettify results by converting to seconds
                ours_out = [str(t * 1e-9) for t in ours_times[t_idx,:,m_idx]]
                ref_out = [str(t * 1e-9) for t in reference_times[t_idx,:,m_idx]]

                file.write("\tOur result: ")
                file.write(str(ours_out) + '\n')
                file.write("\tReference: ")
                file.write(str(ref_out) + '\n')

        file.write('\n' + print_ex_times(ours_times, reference_times))
    # write raw matrix with timestamp
    timestamp = str(int(time.time()))
    np.save('ours_timingMatrix'+timestamp+'.npy', ours_times)
    np.save('reference_timingMatrix'+timestamp+'.npy', reference_times)
def print_single_instance(ours, ref):
    return f"( {np.average(ours) * 1e-9 } vs. {np.average(ref) * 1e-9} )"

def print_ex_times(ours_times, reference_times):
    res = "Ours vs. reference: "
    mci_ours, msi_ours, spi_ours = np.split(ours_times,3, axis=2)
    mci_ref, msi_ref, spi_ref = np.split(reference_times,3, axis=2)
    res = res + "MCI " + print_single_instance(mci_ours, mci_ref)
    res = res + " MSI " + print_single_instance(msi_ours, msi_ref)
    res = res + " SPI " + print_single_instance(spi_ours, spi_ref)
    return res

# define result array for timing results
ours_times = np.zeros(shape=(len(tree_counts), len(test_names),len(metrics)))
ref_times = np.zeros(shape=(len(tree_counts), len(test_names), len(metrics)))
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
                print("Value error while reading taxa count. Processing file anyway.")
        # create temporary file which holds correct number of trees
        for trees_idx, num_trees in enumerate(tree_counts):
            with open(path, "r") as all_trees:
                with open("/tmp/tmp.trees", "w") as out_trees:
                    head = [next(all_trees) for x in range(num_trees)]
                    out_trees.writelines(head)
            # temporaries which would not print into main array if something fails
            ours_metric_arr =  np.zeros(shape=(len(metrics),))
            ref_metric_arr = np.zeros(shape=(len(metrics),))
            for m_idx, metric in enumerate(metrics):
                # Reference impl
                reference_args = ["Rscript", "--vanilla",reference_script , metric , "/tmp/tmp.trees"]
                status_string = f"processing {metric} in {test_names[num]}, {num+1} of {len(test_paths)}, with {num_trees} trees. "
                print(status_string+"Running Reference:")
                ref_start = time.time_ns()
                proc = subprocess.call(reference_args, stdout=FNULL)
                ref_end = time.time_ns()
                print(f"Took {(ref_end - ref_start) * 1e-9} seconds")

                # run our script
                ours_args = [our_exe, '--metric', metric, '-o', '/tmp/ourRes.txt', '-i', "/tmp/tmp.trees"]
                print(status_string+ "Running ours:")
                our_start = time.time_ns()
                proc = subprocess.run(ours_args, shell=False, stdout=FNULL)
                our_end = time.time_ns()
                print(f"Took {(our_end - our_start) * 1e-9} seconds")

                # writing to array upon successful calculation
                ours_metric_arr[m_idx] = our_end - our_start
                ref_metric_arr[m_idx] = ref_end - ref_start
            #push when all metrics are calculated
            ours_times[trees_idx, num] = ours_metric_arr
            ref_times[trees_idx, num] = ref_metric_arr
            file_names.append(test_names[num])

            print(print_ex_times(ours_times, ref_times))
except KeyboardInterrupt:
    print("exiting")
    writeToFile(file_names, ours_times, ref_times)

writeToFile(file_names, ours_times, ref_times)
