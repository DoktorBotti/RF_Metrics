# Introduction

Written as part of the 2021 KIT bioinformatics practical. The program can calculate the RF distance and the [generalized RF distances][1].
To get started clone the repository:

```
git clone --recursive git@github.com:DoktorBotti/RF_Metrics.git
```

# Building
Prerequisites:
 - boost [Version 1.76.0][2]
 - OR-Tools Version 9.0.9048


# Prequesites / External dependencies
The following libraries should be findable by CMake or the installation should be pointed to with a TARGET_ROOT variable. (This variable is then added to the CMAKE_PREFIX_PATH.)
 - gmp (v6.2.1) with --enable-cxx
 - or-tools (v9.0.9048)
 - graph-lib ?

# Running the program
`commandline_rf --metric [ RF | MCI | MSI | SPI ] -i [input-file-path] -o [output-file-path]`
The input must be in the Newick format.
The output contains the pairwise distances of all trees given in the input.

[1]: https://doi.org/10.1093/bioinformatics/btaa614
[2]: https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.bz2
