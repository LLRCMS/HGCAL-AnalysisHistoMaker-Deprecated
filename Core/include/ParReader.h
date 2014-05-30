/**
 *  @file  ParReader.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    12/28/2012
 *
 *  @internal
 *     Created :  12/28/2012
 * Last update :  12/28/2012 11:17:01 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */




#ifndef PARREADER_H
#define PARREADER_H

#include <TEnv.h>
#include <string>
#include <vector>
#include <algorithm>

namespace AnHiMa
{
    class ParReader
    {
        public:
            ParReader();
            ~ParReader(){};
            bool read(const std::string& parFileName);

            std::string& workingDirectory(){return m_workingDirectory;}
            std::string& outputFileName(){return m_outputFileName;}
            std::string& cuts(){return m_cuts;}
            std::string& histoParameterFile(){return m_histoParameterFile;}
            std::string& histoTag(){return m_histoTag;}
            std::string& treeName(){return m_treeName;}
            std::vector<std::string>& inputFiles(){return m_inputFiles;}
            TEnv& params() {return m_params;}

        private:
            TEnv m_params;
            std::string m_workingDirectory;
            std::string m_outputFileName;
            std::string m_cuts;
            std::string m_histoParameterFile;
            std::string m_histoTag;
            std::string m_treeName;
            std::vector<std::string> m_inputFiles;
    };

}



#endif
