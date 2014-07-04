import os
import subprocess
import stat


class BatchJob:
    def __init__(self, name):
        self.name = name
        self.exe = ""
        self.parameters = ""
        self.inputFiles = []
        self.outputDir = ""
        self.outputFiles = []
        ## CMSSW
        self.scram_arch = "slc6_amd64_gcc472"
        self.cmsswDir   = "/home/llr/cms/sauvan/CMSSW/EmptyArea/CMSSW_5_3_5/"
        ## batch
        self.queue = "cms"
        self.qsubArgs = []

    def checkParams(self):
        if not os.path.isfile(self.exe):
            raise StandardError("ERROR: BatchJob: Non existing executable "+self.exe)
        for inp in self.inputFiles:
            if not os.path.isfile(inp):
                raise StandardError("ERROR: BatchJob: Non existing input file "+inp)


    def prepareArea(self):
        if not os.path.isdir(self.outputDir):
            os.makedirs(self.outputDir)
        if not os.path.isdir(self.outputDir+"/jobs"):
            os.makedirs(self.outputDir+"/jobs")
        if not os.path.isdir(self.outputDir+"/logs"):
            os.makedirs(self.outputDir+"/logs")

    def createScript(self):
        with open(self.outputDir+"/jobs/"+self.name+".sub", 'w') as script:
            print >>script, "#! /bin/sh"
            print >>script, "export SCRAM_ARCH={0}".format(self.scram_arch)
            print >>script, "source /opt/exp_soft/cms/cmsset_default.sh "
            print >>script, "cd "+self.cmsswDir+"/src/"
            print >>script, "cmsenv"
            print >>script, "cd", self.outputDir
            print >>script, "\necho Executing job"
            print >>script, self.exe, self.parameters, "&>", self.outputDir+"/logs/"+self.name+".log"

    def prepareCommand(self):
        self.qsubArgs.append("-k")
        self.qsubArgs.append("oe")
        self.qsubArgs.append("-N")
        self.qsubArgs.append(self.name)
        self.qsubArgs.append("-q")
        self.qsubArgs.append(self.queue+"@llrt3")
        self.qsubArgs.append("-v")
        self.qsubArgs.append("X509_USER_PROXY="+os.environ["HOME"]+"/.t3/proxy.cert")
        self.qsubArgs.append("-V")
        self.qsubArgs.append(self.outputDir+"/jobs/"+self.name+".sub")

    def launch(self, local=False, simulate=False):
        if not local:
            command = "qsub"
            for arg in self.qsubArgs:
                command += " "+arg
            print command
            if not simulate:
                subprocess.call(["qsub"] + self.qsubArgs)
        else:
            print self.outputDir+"/jobs/"+self.name+".sub"
            if not simulate:
                os.chmod(self.outputDir+"/jobs/"+self.name+".sub",stat.S_IRWXU)
                subprocess.call([self.outputDir+"/jobs/"+self.name+".sub"])



if __name__=="__main__":
    job = BatchJob("testJob")
    job.exe = "/home/llr/cms/sauvan/software/tools/python/test.sh"
    #job.dataFiles.append("/data_CMS/cms/sauvan/Regression/Ntuples/SingleElectronPt35/SingleElectronPt35_Summer12_DR53X-PU_S10_START53_V7A-v1/13_regressionNtuple.root")
    job.inputFiles.append("/home/llr/cms/sauvan/software/tools/python/dummyFile.config")
    job.outputDir = "/data_CMS/cms/sauvan/testBatch/"
    job.outputFiles.append("dummyFile.out")
    job.checkParams()
    job.prepareArea()
    job.createScript()
    job.prepareCommand()
    job.launch(simulate=True)

