from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob

batch = AnhimaBatchLauncher()
batch.name = "MinBias_200PU"
batch.exe = "/home/llr/cms/sauvan/HGCAL/AnHiMa/StudyGunInHGCAL/analysis.exe"
batch.inputFiles.extend(glob.glob("/data_CMS/cms/sauvan/HGCAL/FromReducedTrees_30052014_v0/MinBias_200PU/MinBias_200PU_*.root"))
batch.tree = "HGC"
batch.outputDirectory = "/data_CMS/cms/sauvan/HGCAL/histos/StudyGunInHGCAL/Pileup200/"
batch.outputFile = "pileup200.root"
batch.histoParameters = "../histos.par"
batch.histoTag = "Histos"
batch.nFilesPerJob = 5

batch.scram_arch = "slc6_amd64_gcc472"
batch.cmsswDir   = "/home/llr/cms/sauvan/CMSSW/EmptyArea/CMSSW_5_3_5/"
batch.queue      = "cms"


batch.additionalParameters["SampleType"] = "pileup"
batch.additionalParameters["MaxHitRef"]  = "false"




