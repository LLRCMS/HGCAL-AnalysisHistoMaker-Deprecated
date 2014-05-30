/**
 *  @file  IAnalysis.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    12/17/2012
 *
 *  @internal
 *     Created :  12/17/2012
 * Last update :  12/17/2012 11:10:41 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */

#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include <TDirectory.h>
#include <TFile.h>
#include <TChain.h>
#include <TEntryList.h>

#include "IAnalysis.h"
#include "ParReader.h"

using namespace AnHiMa;
using namespace std;

/*****************************************************************/
IAnalysis::IAnalysis():
    m_workingDirectory("./"),
    m_outputFileName("histos.root")
/*****************************************************************/
{
}


/*****************************************************************/
IAnalysis::~IAnalysis()
/*****************************************************************/
{
    m_outputFile->Write();
    m_outputFile->Close();
    m_inputChain->Delete();
}


/*****************************************************************/
bool IAnalysis::initialize(const std::string& parameterFile)
/*****************************************************************/
{
    cout<<"In IAnalysis::initialize()\n";
    m_reader.read(parameterFile);


    m_workingDirectory = m_reader.workingDirectory();
    struct stat buf;
    stat(m_workingDirectory.c_str(), &buf);
    if(!S_ISDIR(buf.st_mode))
    {
        cout<<"ERROR: Working directory doesn't exist\n";
        return false;
    }

    m_outputFileName = m_reader.outputFileName();
    stringstream fullFileName;
    fullFileName << m_workingDirectory << "/" << m_outputFileName;

    m_inputChain = new TChain(m_reader.treeName().c_str());
    vector<string>::iterator it = m_reader.inputFiles().begin();
    vector<string>::iterator itE = m_reader.inputFiles().end();
    for(;it!=itE;++it)
    {
        m_inputChain->Add(it->c_str());
    }

    m_outputFile = TFile::Open(fullFileName.str().c_str(), "RECREATE");
    if(!m_outputFile)
    {
        cout<<"ERROR: Cannot open output file for writing\n";
        return false;
    }
    m_cuts = m_reader.cuts();
    m_histos.initialize(m_outputFile, m_reader.histoParameterFile(), m_reader.histoTag());

    m_inputChain->Draw(">>elist", m_cuts.c_str(), "entrylist");
    m_entryList = (TEntryList*)gDirectory->Get("elist");

    m_nEntries = m_entryList->GetN();
    m_inputChain->SetEntryList(m_entryList);


    return true;
}


/*****************************************************************/
void IAnalysis::loop()
/*****************************************************************/
{
    int treenum=0;
    for (Long64_t entry =0;entry<m_nEntries;entry++)
    //for (Long64_t entry =0;entry<100;entry++)
    {
        if(entry%10000==0)
        {
            cout<<"INFO: Getting entry "<<entry<<"/"<<m_nEntries<<"\n";
        }

        Long64_t treeEntry = m_entryList->GetEntryAndTree(entry, treenum);
        Long64_t chainEntry = treeEntry+m_inputChain->GetTreeOffset()[treenum];
        m_inputChain->GetEntry(chainEntry);
        execute();
    }
    cout<<"INFO: Finish - All good\n";
}


