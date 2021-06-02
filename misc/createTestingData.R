# Very dumb script which calculates MSI, MCI and SPI pairwise scores on all first trees in the test data folder
# ... It expects to be run above the BS folder.
# USAGE: Rscript createTestingData.R

library(TreeDist)
library(ape)

files <- list.files("./BS/", full.names=TRUE)
spiPath <- "./SPI_10/"
mciPath <- "./MCI_10/"
msiPath <- "./MSI_10/"
# loop over each file in the directory
lapply(files, function(f){
	
	first_trees_txt <- readLines(f, n=10)
	filename <- basename(f)
	if (filename == "2554" || filename == "2308"){
		next
}
	#print(first_trees_txt)
	trees <- ape::read.tree(text=first_trees_txt)	
	
	# Amount of phylogenetic information in common
	spi <- SharedPhylogeneticInfo(trees) 
	write.table(spi,file = paste(spiPath, filename), sep = ",",row.names=FALSE, col.names=FALSE)
	print(paste("Done ", filename, " SPI"))
		
	# MCI metric
	mci <- MutualClusteringInfo(trees)
	write.table(mci,file = paste(mciPath, filename), sep = ",",row.names=FALSE, col.names=FALSE)
	print(paste("Done ", filename, " MCI"))
	
	# MSI metric
	msi <- MatchingSplitInfo(trees)
	write.table(msi,file = paste(msiPath, filename), sep = ",",row.names=FALSE, col.names=FALSE)
	print(paste("Done ", filename, " MSI"))
	})
#> [1] 13.75284MatchingSplitInfoSplits(splits1, splits2)
#> [1] 17.09254MutualClusteringInfoSplits(splits1, splits2)
#> [1] 3.031424