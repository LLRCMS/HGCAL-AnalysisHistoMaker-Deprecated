from BatchJobs import BatchJobs
import glob


batch = BatchJobs()
batch.name = "MinBias_200PU"
batch.inputFiles.extend(glob.glob("/data_CMS/cms/sauvan/HGCAL/FromReducedTrees_30052014_v0/MinBias_200PU/MinBias_200PU_*.root"))
batch.outputDir = "/data_CMS/cms/sauvan/HGCAL/histos/test/"
batch.tree = "HGC"

batch.launch(local=False,simulate=False)
