import os, subprocess, time

test_files_dir = '/rf_data/BS/'
project_dir = os.path.pardir
our_exe = project_dir + "/bin/commandline_rf"
raxml_exe = '/home/user/raxml/raxmlHPC-AVX'
test_paths = [test_files_dir + file for file in os.listdir(test_files_dir)]

# clean this folder of all previous outputs from raxml
for name in os.listdir(os.curdir):
    if name.startswith("RAxML_"):
        os.remove(os.path.join(os.curdir, name))

# define result array for timing results
ours_times = []
raxml_times = []

# make ugly stuff to hide console output from both executables
FNULL = open(os.devnull)

# for each input file
for num, path in enumerate(test_paths):
    # run our script
    ours_args = [our_exe, '--metric', 'RF', '-o', '/tmp/ourRes.txt', '-i', path]
    print(f"processing ours, {num} of {len(test_paths)}")
    start = time.time_ns()
    proc = subprocess.run(ours_args, shell=False,stdout=FNULL)
    end = time.time_ns()
    ours_times.append(end - start)
    print(f"Took {(end - start) * 1e-9} seconds")
    # RAxML script
    raxml_args = [raxml_exe, '-m', 'GTRCAT', '-f', 'r', '-z', path, '-n', str(num)]
    print(f"processing RAxML, {num} of {len(test_paths)}")
    start = time.time_ns()
    proc = subprocess.run(raxml_args, shell=False,stdout=FNULL)
    end = time.time_ns()
    raxml_times.append(end - start)
    print(f"Took {(end - start) * 1e-9} seconds")
    print(
        f"mean execution times: Ours {sum(ours_times) / len(ours_times) * 1e-9}, RAxML {sum(raxml_times) / len(raxml_times) * 1e-9}")

# prettify results by converting to seconds
ours_out = [str(t * 1e-9) for t in ours_times]
raxml_out = [str(t * 1e-9) for t in raxml_times]

# write results to file
with open(os.curdir + 'timing_results.txt', "w") as file:
    file.write("Our result: ")
    file.write(str(ours_out))
    file.write("RAxML:")
    file.write(str(raxml_out))
