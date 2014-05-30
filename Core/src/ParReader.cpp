



#include "ParReader.h"
#include "Utilities.h"

using namespace AnHiMa;
using namespace std;


/*****************************************************************/
ParReader::ParReader():m_workingDirectory("./"),
    m_outputFileName("dummy.root"),
    m_cuts(""),
    m_histoParameterFile("histos.par"),
    m_histoTag("DummyHistos"),
    m_treeName("dummyTree")
/*****************************************************************/
{
}


/*****************************************************************/
bool ParReader::read(const std::string& parFileName)
/*****************************************************************/
{
    int status = m_params.ReadFile(parFileName.c_str(),EEnvLevel(0));
    if(status!=0) 
    {
        cout<<"FATAL: ParReader::read(): Cannot read file "<<parFileName<<"\n"; 
        return false;
    }

    string inputFiles= m_params.GetValue("InputFiles", "");
    tokenize(inputFiles, m_inputFiles, ":");
    m_workingDirectory = m_params.GetValue("WorkingDirectory", "./");
    m_outputFileName = m_params.GetValue("OutputFile", "dummy.root");
    m_cuts = m_params.GetValue("Cuts", "");
    m_histoParameterFile = m_params.GetValue("HistoParameters", "histos.par");
    m_histoTag = m_params.GetValue("HistoTag", "DummyHistos");
    m_treeName = m_params.GetValue("Tree", "dummyTree");


    return true;
}
