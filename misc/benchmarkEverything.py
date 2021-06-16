import os, subprocess, time

test_files_dir = '../test/samples/data/heads/BS/'
project_dir = os.path.pardir
our_exe = project_dir + "/bin/commandline_rf"
reference_script = 'referenceTiming.R'
metrics = ["MCI", "MSI", "SPI"]
test_names = [file for file in os.listdir(test_files_dir)]
test_paths = [test_files_dir + a for a in test_names]

def writeToFile(file_names, ours_times, reference_times):
    # write results to file
    with open('Unopt_preview_refVSus.txt', "w") as file:
        file.write(f"Evaluated the following samples: {file_names}\n")

        for m_idx, metric in enumerate(metrics):
            file.write(metric + " scores:\n")
            # prettify results by converting to seconds
            ours_out = [str(t[m_idx] * 1e-9) for t in ours_times]
            ref_out = [str(t[m_idx] * 1e-9) for t in reference_times]

            file.write("\tOur result: ")
            file.write(str(ours_out) + '\n')
            file.write("\tReference: ")
            file.write(str(ref_out) + '\n')
def print_single_instance(ours, ref):
    return f"( {sum(ours)/len(ours) * 1e-9 } vs. {sum(ref)/len(ref)*1e-9} )"

def print_ex_times(ours_times, reference_times):
    res = "Ours vs. reference: "
    mci_ours, msi_ours, spi_ours = zip(*ours_times)
    mci_ref, msi_ref, spi_ref = zip(*reference_times)
    res = res + "MCI " + print_single_instance(mci_ours, mci_ref)
    res = res + " MSI " + print_single_instance(msi_ours, msi_ref)
    res = res + " SPI " + print_single_instance(spi_ours, spi_ref)
    return res


# clean this folder of all previous outputs from raxml
for name in os.listdir(os.curdir):
    if name.startswith("RAxML_"):
        os.remove(os.path.join(os.curdir, name))

# define result array for timing results
ours_times = []
ref_times = []
file_names = []

# make ugly stuff to hide console output from both executables
FNULL = open(os.devnull)
try:
    # for each input file
    for num, path in enumerate(test_paths):
        try:
            if(int(test_names[num])>=2000):
                continue
        except ValueError:
                print("Value error while reading taxa count. Processing file anyway.")
        ours_metric_arr = [0,0,0]
        ref_metric_arr = [0,0,0]
        for m_idx, metric in enumerate(metrics):
            # Reference impl
            reference_args = ["Rscript", "--vanilla",reference_script , metric , path]
            print(f"processing {metric} in {test_names[num]}, {num} of {len(test_paths)}. Running Reference:")
            ref_start = time.time_ns()
            proc = subprocess.call(reference_args, stdout=FNULL)
            ref_end = time.time_ns()
            print(f"Took {(ref_end - ref_start) * 1e-9} seconds")

            # run our script
            ours_args = [our_exe, '--metric', metric, '-o', '/tmp/ourRes.txt', '-i', path]
            print(f"processing {metric} in {test_names[num]}, {num+1} of {len(test_paths)}. Running ours:")
            our_start = time.time_ns()
            proc = subprocess.run(ours_args, shell=False, stdout=FNULL)
            our_end = time.time_ns()
            print(f"Took {(our_end - our_start) * 1e-9} seconds")

        # writing to array upon successful calculation
            ours_metric_arr[m_idx] = our_end - our_start
            ref_metric_arr[m_idx] = ref_end - ref_start
        #push when all metrics are calculated
        ours_times.append(ours_metric_arr)
        ref_times.append(ref_metric_arr)
        file_names.append(test_names[num])

        print(print_ex_times(ours_times, ref_times))
except KeyboardInterrupt:
    print("exiting")
    writeToFile(file_names, ours_times, ref_times)

writeToFile(file_names, ours_times, ref_times)
