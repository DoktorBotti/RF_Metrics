library(TreeDist)
tree1 <- ape::read.tree(text = '((((F,C),B),A),(E,D));')
tree2 <- ape::read.tree(text = '(((B,C),(D,A)),(E,F));')

tree1 <- ape::read.tree(text = '((((F,C),B),A),((E,D),(G,(H,J))));')
tree2 <- ape::read.tree(text = '(((B,(C,G)),(((H,J),D),A)),(E,F));')

VisualizeMatching( SharedPhylogeneticInfo,tree1,tree2)
spi <- MatchingSplitInfo(tree1,tree2, reportMatching = TRUE, normalize = FALSE)
attr(spi, "pairScores")
matr <- attr(spi, "pairScores")
output_path <- "/rf_metrics/smallmtx"
write.table(matr, file = output_path, row.names=FALSE, col.names=FALSE)
