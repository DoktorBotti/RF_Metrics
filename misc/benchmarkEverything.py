import os, subprocess, time

def writeToFile(file_names, ours_times, raxml_times):
    # write results to file
    with open('timing_results.txt', "w") as file:
        # prettify results by converting to seconds
        ours_out = [str(t * 1e-9) for t in ours_times]
        raxml_out = [str(t * 1e-9) for t in raxml_times]

        file.write(f"Evaluated the following samples: {file_names}\n")
        file.write("Our result: ")
        file.write(str(ours_out) + '\n')
        file.write("RAxML: ")
        file.write(str(raxml_out))


test_files_dir = '/rf_data/BS/'
project_dir = os.path.pardir
our_exe = project_dir + "/bin/commandline_rf"
raxml_exe = '/home/user/raxml/raxmlHPC-AVX'
test_names = [file for file in os.listdir(test_files_dir)]
test_paths = [test_files_dir + a for a in test_names]

# clean this folder of all previous outputs from raxml
for name in os.listdir(os.curdir):
    if name.startswith("RAxML_"):
        os.remove(os.path.join(os.curdir, name))

# define result array for timing results
ours_times = []
raxml_times = []
file_names = []

# make ugly stuff to hide console output from both executables
FNULL = open(os.devnull)
try:
    # for each input file
    for num, path in enumerate(test_paths):
        if num == 5 or num == 10 or num != 14:
            continue
        # RAxML script
        raxml_args = [raxml_exe, '-m', 'GTRCAT', '-f', 'r', '-z', path, '-n', str(num)]
        print(f"processing {test_names[num]}, {num} of {len(test_paths)}. Running RAxML:")
        raxml_start = time.time_ns()
        proc = subprocess.run(raxml_args, shell=False, stdout=FNULL)
        raxml_end = time.time_ns()
        print(f"Took {(raxml_end - raxml_start) * 1e-9} seconds")

        # run our script
        ours_args = [our_exe, '--metric', 'RF', '-o', '/tmp/ourRes.txt', '-i', path]
        print(f"processing {test_names[num]}, {num} of {len(test_paths)}. Running ours:")
        our_start = time.time_ns()
        proc = subprocess.run(ours_args, shell=False, stdout=FNULL)
        our_end = time.time_ns()

        # writing to array upon successful calculation
        ours_times.append(our_end - our_start)
        raxml_times.append(raxml_end - raxml_start)
        file_names.append(test_names[num])

        print(f"Took {(our_end - our_start) * 1e-9} seconds")
        print(
            f"mean execution times: Ours {sum(ours_times) / len(ours_times) * 1e-9}, RAxML {sum(raxml_times) / len(raxml_times) * 1e-9}")
except KeyboardInterrupt:
    print("exiting")
    writeToFile(file_names, ours_times, raxml_times)

writeToFile(file_names, ours_times, raxml_times)
