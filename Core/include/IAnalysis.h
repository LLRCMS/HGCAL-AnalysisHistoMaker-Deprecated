/**
 *  @file  IAnalysis.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    12/17/2012
 *
 *  @internal
 *     Created :  12/17/2012
 * Last update :  12/17/2012 10:46:38 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */

#ifndef IANALYSIS_H
#define IANALYSIS_H


#include <string>


#include "HistoManager.h"
#include "ParReader.h"

class TFile;
class TChain;
class TEntryList;

namespace AnHiMa
{

    class IAnalysis
    {
        public:
            IAnalysis();
            virtual ~IAnalysis();

            virtual bool initialize(const std::string& parameterFile);
            void loop();
            virtual void execute() = 0;


        protected:
            std::string m_workingDirectory;
            std::string m_outputFileName;
            TChain* m_inputChain;
            TEntryList* m_entryList;
            Long64_t m_nEntries;
            std::string m_cuts;
            TFile* m_outputFile;
            HistoManager m_histos;
            ParReader m_reader;

    };

}

#endif 
