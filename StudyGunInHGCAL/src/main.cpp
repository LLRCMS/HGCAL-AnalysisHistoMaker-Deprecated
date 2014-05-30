
#include <string>
#include <iostream>

#include "Analysis.h"

using namespace std;
using namespace AnHiMa;

int main(int argc, char** argv)
{
    if(argc!=2)
    {
        cout<<"Usage: analysis.exe parFile\n";
        return 1;
    }
    string parFile(argv[1]);

    Analysis analysis;
    
    try
    {
        bool status = analysis.initialize(parFile);
        if(!status)
            return 1;
        analysis.loop();
    }
    catch(string s)
    {
        cout<<"ERROR: "<<s<<"\n";
        return 1;
    }

    return 0;
}
