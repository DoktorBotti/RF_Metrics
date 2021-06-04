# Very dumb script which calculates MSI, MCI and SPI pairwise scores on all first trees in the test data folder
# ... It expects to be run above the BS folder.
# USAGE: Rscript createTestingData.R

library(TreeDist)
library(ape)

files <- list.files("./data/heads/BS/", full.names=TRUE)
spiPath <- "./SPI_10/"
mciPath <- "./MCI_10/"
msiPath <- "./MSI_10/"
dir.create(spiPath, showWarnings = FALSE)
dir.create(mciPath, showWarnings = FALSE)
dir.create(msiPath, showWarnings = FALSE)

# define function which caclulates and writes all split scores for a custom function
writeSplitScores <- function (trees, output_path, func){
	for(i in c(1 : length(trees))){
		for(j in 1:i){
			out_name <- paste0(i,"_",j)
			res_mat <- attr(func(trees[[i]], trees[[j]], reportMatching = TRUE),"pairScores")
			write.table(res_mat, file = paste0(output_path, out_name), row.names=FALSE, col.names=FALSE)
		}
	}
}
# loop over each file in the directory
lapply(files, function(f){
	
	first_trees_txt <- readLines(f, n=10)
	filename <- basename(f)
	if (filename == "2554" || filename == "2308"){
		return()
}
	#print(first_trees_txt)
	trees <- ape::read.tree(text=first_trees_txt)
	
	# Amount of phylogenetic information in common
	spi <- SharedPhylogeneticInfo(trees)
	sampleFolder <- paste0(spiPath, filename , "/")
	dir.create(sampleFolder, showWarnings = FALSE)
	write.table(spi,file = paste0(sampleFolder, "pairwise_trees"), sep = ",",row.names=FALSE, col.names=FALSE)
	writeSplitScores(trees, sampleFolder, SharedPhylogeneticInfo)
	print(paste("Done ", filename, " SPI"))
		
	# MCI metric
	mci <- MutualClusteringInfo(trees)
	sampleFolder <- paste0(mciPath, filename , "/")
	dir.create(sampleFolder, showWarnings = FALSE)
	write.table(mci, file = paste0(sampleFolder, "pairwise_trees"), sep = ",", row.names=FALSE, col.names=FALSE)
	writeSplitScores(trees, sampleFolder, MutualClusteringInfo)
	print(paste("Done ", filename, " MCI"))
	
	# MSI metric
	msi <- MatchingSplitInfo(trees)
	sampleFolder <- paste0(msiPath, filename , "/")
	dir.create(sampleFolder, showWarnings = FALSE)
	write.table(msi,file = paste0(sampleFolder, "pairwise_trees"), sep = ",",row.names=FALSE, col.names=FALSE)
	writeSplitScores(trees, sampleFolder, MatchingSplitInfo)

	print(paste("Done ", filename, " MSI"))
	})
#> [1] 13.75284MatchingSplitInfoSplits(splits1, splits2)
#> [1] 17.09254MutualClusteringInfoSplits(splits1, splits2)
#> [1] 3.031424