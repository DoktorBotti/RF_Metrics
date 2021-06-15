args <- commandArgs(trailingOnly = TRUE)

library(TreeDist)
trees <- ape::read.tree(file=args[2])
if(args[1] =="SPI")
{
  mat <- SharedPhylogeneticInfo(trees)
  write.table(mat,"/tmp/refOut",row.names = FALSE, col.names = FALSE)
} else if(args[1] == "MSI")
{
  mat <- MatchingSplitInfo(trees)
  write.table(mat,"/tmp/refOut",row.names = FALSE, col.names = FALSE)
} else if(args[1] == "MCI")
{
  mat <- MutualClusteringInfo(trees)
  write.table(mat,"/tmp/refOut",row.names = FALSE, col.names = FALSE)
}