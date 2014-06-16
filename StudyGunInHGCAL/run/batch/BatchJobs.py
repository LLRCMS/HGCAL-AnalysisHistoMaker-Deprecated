import os
import subprocess
import stat
import sys
import random
import ROOT


class BatchJobs:
    def __init__(self):
        self.name = "fa3VsfL1"
        self.inputFiles = []
        self.tree = "hgcSimHitsAnalyzer/HGC"
        self.outputDir = ""
        self.outputFile = "histos.root"
        self.histoParameters = "/home/llr/cms/sauvan/HGCAL/AnHiMa/StudyGunInHGCAL/run/histos.par"
        self.histoTag = "Histos"

        self.cmsswDir = "/home/llr/cms/sauvan/CMSSW/EmptyArea/CMSSW_5_3_5/"
        self.cwd = os.getcwd()
        self.queue = "cms"
        self.qsubArgs = []

    def createConfigs(self):
        if not os.path.exists(self.outputDir+"/jobs/"):
            os.makedirs(self.outputDir+"/jobs/")
        if not os.path.exists(self.outputDir+"/logs/"):
            os.makedirs(self.outputDir+"/logs/")

        for job,inputFile in enumerate(self.inputFiles):
            with open(self.outputDir+"/jobs/{0}_{1}.config".format(self.name,job), 'w') as config:
                print >>config, "InputFiles: {0}".format(inputFile)
                print >>config, "Tree: {0}".format(self.tree)
                print >>config, "WorkingDirectory: {0}".format(self.outputDir)
                print >>config, "OutputFile: {0}_{1}".format(job,self.outputFile)
                print >>config, "HistoParameters: {0}".format(self.histoParameters)
                print >>config, "HistoTag: {0}".format(self.histoTag)
                print >>config, "SampleType: pileup"
                print >>config, "MaxHitRef: false"


    def createScripts(self):
        for job,inputFile in enumerate(self.inputFiles):
            with open(self.outputDir+"/jobs/{0}_{1}.sub".format(self.name,job), 'w') as script:
                print >>script, "#! /bin/sh"
                print >>script, "export SCRAM_ARCH=slc6_amd64_gcc472"
                print >>script, "source /opt/exp_soft/cms/cmsset_default.sh "
                print >>script, "cd "+self.cmsswDir+"/src/"
                print >>script, "cmsenv"
                print >>script, "cd", self.outputDir
                print >>script, "\necho Executing job"
                print >>script, self.cwd+"/../../analysis.exe","jobs/{0}_{1}.config".format(self.name,job) , "&>", "logs/{0}_{1}.log".format(self.name,job)


    def prepareCommands(self):
        for job,inputFile in enumerate(self.inputFiles):
            qsubArgs = []
            qsubArgs.append("-k")
            qsubArgs.append("oe")
            qsubArgs.append("-N")
            qsubArgs.append(self.name+"_"+str(job))
            qsubArgs.append("-q")
            qsubArgs.append(self.queue+"@llrt3")
            qsubArgs.append("-v")
            qsubArgs.append("X509_USER_PROXY="+os.environ["HOME"]+"/.t3/proxy.cert")
            qsubArgs.append("-V")
            qsubArgs.append("{0}/jobs/{1}_{2}.sub".format(self.outputDir,self.name,job))
            self.qsubArgs.append(qsubArgs)

    def launch(self, local=False, simulate=False):
        self.createConfigs()
        self.createScripts()
        self.prepareCommands()
        for qsubArgs in self.qsubArgs:
            if not local:
                command = "qsub"
                for arg in qsubArgs:
                    command += " "+arg
                print command
                if not simulate:
                    subprocess.call(["qsub"] + qsubArgs)
            else:
                if not simulate:
                    os.chmod("{0}/jobs/{0}_{1}.sub".format(self.outputDir,self.name,job),stat.S_IRWXU)
                    subprocess.call(["{0}/jobs/{0}_{1}.sub".format(self.outputDir,self.name,job)])

