from BatchJob import BatchJob
import copy
import os
import shutil
import stat
import sys
import datetime
import subprocess
import ROOT


class AnhimaBatchLauncher:
    def __init__(self):
        self.exe = "/home/llr/cms/sauvan/AnHiMa/Test/test.exe"
        self.tree = "simpleRootTree"
        self.inputFiles = []
        self.outputDirectory = "./"
        self.name = ""
        self.outputFile = "histoOutput.root"
        self.cuts = []
        self.histoParameters = "histo.par"
        self.histoTag = "DummyHisto"
        self.nFilesPerJob = 10
        self.nJobs = 0
        self.queue = "cms"
        self.local = False
        self.version = ""
        self.additionalParameters = {}

    def addCuts(self, cuts):
        self.cuts.extend(cuts)

    def  inputToString(self):
        if len(self.inputFiles)==0:
            raise StandardError("ERROR: No input files")
        inputFiles = ""
        for ip in self.inputFiles:
            inputFiles += ip+":"
        inputFiles = inputFiles[0:-1]
        return inputFiles

    def cutsToString(self):
        if len(self.cuts)==0:
            return ""
        cut = ""
        for c in self.cuts:
            cut += c+"&&"
        cut = cut[0:-2]
        return cut

    def additionalParToString(self):
        pars = ""
        for key,value in self.additionalParameters.items():
            pars += key+": "+value+"\n"
        return pars

    def prepareArea(self):
        self.findVersion()
        ### perform some checks
        if not os.path.isfile(self.exe):
            raise StandardError("ERROR: cannot find executable")
        if not (stat.S_IXUSR & os.stat(self.exe)[stat.ST_MODE]):
            raise StandardError("ERROR: you don't have exe permission for "+self.exe)
        if not os.path.isfile(self.histoParameters):
            raise StandardError("ERROR: cannot find histo parameters"+self.histoParameters)
        for inFile in self.inputFiles:
            if not os.path.isfile(inFile):
                raise StandardError("ERROR: cannot find input file "+inFile)
        ### Create running area
        if not os.path.isdir(self.outputDirectory):
            os.makedirs(self.outputDirectory)
        if not os.path.isdir(self.outputDirectory+"/"+self.version):
            os.makedirs(self.outputDirectory+"/"+self.version)
        if not os.path.isdir(self.outputDirectory+"/"+self.version+"/jobs"):
            os.makedirs(self.outputDirectory+"/"+self.version+"/jobs")
        if not os.path.isdir(self.outputDirectory+"/"+self.version+"/src"):
            os.makedirs(self.outputDirectory+"/"+self.version+"/src")
        if not os.path.isdir(self.outputDirectory+"/"+self.version+"/logs"):
            os.makedirs(self.outputDirectory+"/"+self.version+"/logs")
        if not os.path.isdir(self.outputDirectory+"/"+self.version+"/obj"):
            os.makedirs(self.outputDirectory+"/"+self.version+"/obj")
        if not os.path.isdir(self.outputDirectory+"/"+self.version+"/../Core"):
            os.makedirs(self.outputDirectory+"/"+self.version+"/../Core")
        if not os.path.isdir(self.outputDirectory+"/"+self.version+"/../Core/lib"):
            os.makedirs(self.outputDirectory+"/"+self.version+"/../Core/lib")


        ### Create configurations files
        inputFiles = copy.copy(self.inputFiles)
        while len(inputFiles)>0:
            self.nJobs += 1
            inputs = ""
            for i in range(0,self.nFilesPerJob):
                inputs += inputFiles[0]+":"
                inputFiles = inputFiles[1:]
                if len(inputFiles)==0:
                    break
            inputs = inputs[:-1]
            with open(self.outputDirectory+"/"+self.version+"/jobs/"+self.name+"."+str(self.nJobs)+".config", 'w') as fConfig:
                print >>fConfig, "## Configuration file for histo maker", self.name
                print >>fConfig, "InputFiles: ",inputs
                print >>fConfig, "Tree: ", self.tree
                if(len(self.cuts)>0):
                    print >>fConfig, "Cuts: ", self.cutsToString()
                print >>fConfig, "\n"
                print >>fConfig, "WorkingDirectory:", self.outputDirectory+"/"+self.version
                print >>fConfig, "OutputFile:", os.path.splitext(self.outputFile)[0]+"."+str(self.nJobs)+os.path.splitext(self.outputFile)[1]
                print >>fConfig, "\n"
                print >>fConfig, "HistoParameters:", self.outputDirectory+"/"+self.version+"/jobs/"+os.path.basename(self.histoParameters)
                print >>fConfig, "HistoTag:", self.histoTag
                print >>fConfig, "\n"
                print >>fConfig, self.additionalParToString()
                print >>fConfig, "\n"

        ### Copy histo parameters 
        shutil.copy(self.histoParameters, self.outputDirectory+"/"+self.version+"/jobs/")

        ### Backup src files
        baseDir = os.path.dirname(self.exe)
        srcFiles = [baseDir+"/src/"+f for f in os.listdir(baseDir+"/src/") if not "main" in f]
        srcFiles += [baseDir+"/include/"+f for f in os.listdir(baseDir+"/include/")]
        for src in srcFiles:
            shutil.copy(src, self.outputDirectory+"/"+self.version+"/src/")

    def printInfo(self):
        print "\n>>>>>> You are going to send", self.nJobs, "jobs on batch <<<<<<"
        print "  "+self.name+"  Version "+self.version
        print ""

    def launch(self, simulate=False):
        self.printInfo()
        answer = ""
        go = False
        while answer!='y' and answer!='n':
            sys.stdout.write("Do you want to continue (y/n)? ")
            answer = sys.stdin.readline()
            answer = answer.strip()
        if answer == 'y':
            go = True
            print "-- LAUNCH\n"
        elif answer =='n':
            print "-- STOP\n"
        if go:
            for id in range(1,self.nJobs+1):
                job = BatchJob("job_"+self.name+"."+str(id))
                job.exe = self.exe
                job.queue = self.queue
                job.parameters = "jobs/"+self.name+"."+str(id)+".config"
                job.inputFiles.append(self.outputDirectory+"/"+self.version+"/jobs/"+self.name+"."+str(id)+".config")
                job.inputFiles.append(self.outputDirectory+"/"+self.version+"/jobs/"+os.path.basename(self.histoParameters))
                job.outputDir = self.outputDirectory+"/"+self.version
                job.outputFiles.append(os.path.splitext(self.outputFile)[0]+"."+str(id)+os.path.splitext(self.outputFile)[1])
                job.checkParams()
                job.prepareArea()
                job.createScript()
                job.prepareCommand()
                job.launch(self.local, simulate)

    def findVersion(self):
        self.version = "v_1_"+str(datetime.date.today())
        if os.path.isdir(self.outputDirectory):
            listdirs= [f for f in os.listdir(self.outputDirectory) if os.path.isdir(os.path.join(self.outputDirectory,f))]
            numberMax = 0
            for d in listdirs:
                if "v_" in d:
                    number = int(d.split("_")[1])
                    if number > numberMax:
                        numberMax = number
            self.version = "v_"+str(numberMax+1)+"_"+str(datetime.date.today())

    def findLastVersion(self):
        lastVersion = ""
        if os.path.isdir(self.outputDirectory):
            listdirs= [f for f in os.listdir(self.outputDirectory) if os.path.isdir(os.path.join(self.outputDirectory,f))]
            numberMax = 0
            for d in listdirs:
                if "v_" in d:
                    number = int(d.split("_")[1])
                    if number > numberMax:
                        numberMax = number
                        lastVersion = d
        return lastVersion

    def addHistos(self):
        lastVersion = self.findLastVersion()
        if os.path.exists(self.outputDirectory+"/"+lastVersion+"/"+self.outputFile):
            print "INFO: Summed file exists already"
            return
        files = [self.outputDirectory+"/"+lastVersion+"/"+os.path.splitext(self.outputFile)[0]+".tmp"+os.path.splitext(self.outputFile)[1]]
        files.extend([self.outputDirectory+"/"+lastVersion+"/"+f for f in os.listdir(self.outputDirectory+"/"+lastVersion) if ".root" in f])
        subprocess.call(["hadd"] + files)
        summedFile = ROOT.TFile.Open(self.outputDirectory+"/"+lastVersion+"/"+os.path.splitext(self.outputFile)[0]+".tmp"+os.path.splitext(self.outputFile)[1])
        scaledFile = ROOT.TFile.Open(self.outputDirectory+"/"+lastVersion+"/"+self.outputFile, "RECREATE")
        listOfHisto = []
        listOfHistoD = []
        listOfNtuple = []
        next = ROOT.TIter(summedFile.GetListOfKeys())
        key = next()
        while key:
            if key.GetClassName()=="TH1F" or \
            key.GetClassName()=="TH2F":
                listOfHisto.append(key.GetName())
            if key.GetClassName()=="TH1D" or \
            key.GetClassName()=="TH2D":
                listOfHistoD.append(key.GetName())
            if key.GetClassName()=="TNtupleD":
                listOfNtuple.append(key.GetName())
            key = next()
        for histName in listOfHisto:
            histo = summedFile.Get(histName)
            histo.__class__ = ROOT.TH1F
            histo.SetDirectory(0)
            if "_BIN_" in histName:
                histo.Scale(1./(len(files)-1))
            scaledFile.cd()
            histo.Write()
        for histName in listOfHistoD:
            histo = summedFile.Get(histName)
            histo.__class__ = ROOT.TH1D
            histo.SetDirectory(0)
            if "_BIN_" in histName:
                histo.Scale(1./(len(files)-1))
            scaledFile.cd()
            histo.Write()
        for ntName in listOfNtuple:
            ntuple = summedFile.Get(ntName)
            args = ntuple.GetArgs()
            ntNew = ntuple.CloneTree(0)
            entries = ntuple.GetEntriesFast()
            for ientry in xrange( entries ):
                ntuple.GetEntry(ientry);
                ntNew.Fill(args)
            scaledFile.cd()
            ntNew.Write();
        summedFile.Close()
        scaledFile.Close()
        os.remove(self.outputDirectory+"/"+lastVersion+"/"+os.path.splitext(self.outputFile)[0]+".tmp"+os.path.splitext(self.outputFile)[1])





