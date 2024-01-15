#include "Configuration.h"

#include <stdlib.h>

Configuration* Configuration::fTheInstance=0;

Configuration::Configuration()
{
    fMpValue.clear(); 
}

void Configuration::Finalize()
{
    fMpValue.clear();
}

Configuration* Configuration::GetInstance()
{
    if( fTheInstance==0 )
    {
        fTheInstance=new Configuration();
        cout<<" Instanciating Configuration for the first time" <<endl;
    }
    return fTheInstance;
}

void Configuration::AddValue(string name, string svalue)
{
    Value_t aVal;
    svalue = ParseMDTROOT(svalue);
    aVal.i = atoi( svalue.c_str() );
    aVal.f = atof( svalue.c_str() );
    aVal.s = svalue;
    fMpValue[name] = aVal;
}

template <> int Configuration::GetValue<int>(string name)
{
    if( this->IsAvailable(name) )
    return fMpValue[name].i;

    return -999.;
}

template <> float Configuration::GetValue<float>(string name)
{
    if( this->IsAvailable(name) )
    return fMpValue[name].f;

    return -999.;
}

template <> string Configuration::GetValue<string>(string name)
{
    if( this->IsAvailable(name) )
    return fMpValue[name].s;

    return "";
}

template <> void Configuration::GetValue<int>(string name, int &t)
{
    if( this->IsAvailable(name, false) ){ t = fMpValue[name].i; }
}

template <> void Configuration::GetValue<float>(string name, float &t)
{
    if( this->IsAvailable(name, false) ){ t = fMpValue[name].f; }
}


template <> void Configuration::GetValue<string>(string name, string &t)
{
    if( this->IsAvailable(name, false) ){ t = fMpValue[name].s; }
}

void Configuration::ReadParameter(string infilename)
{
// Array parameter is not supported yet
    infilename = ParseMDTROOT(infilename);
    ifstream fin(infilename);
    if( !fin.is_open() )
    {
        cout<<" Cannot open the file: " << infilename <<endl;
        exit(-1);
    }
    cout<<" Loading parameter settings from: " << infilename <<endl;
    
    string aLine;
    string sname;
    string svalue;
    while( std::getline(fin, aLine) )
    {
        if( aLine[0] == '#' ){ continue; }

        if( !(aLine.find('<')!=string::npos &&
              aLine.find('=')!=string::npos &&
              aLine.find('>')!=string::npos) ){ continue; }

        stringstream ss(aLine);
        ss.ignore(aLine.size(), '<');
        ss >> sname;   
        ss.ignore(aLine.size(), '=');
        ss >> svalue;
        this->AddValue(sname, svalue);
    }
    fin.close();
}

void Configuration::PrintParameters()
{
    map<string, Value_t>::iterator itr;

    string stmp1 = " *********** Available Pareameters ********** " ;
    string stmp2 = " ******************************************** " ;

    cout<< stmp1 <<endl;
    for(itr=fMpValue.begin(); itr!=fMpValue.end(); itr++)
    {
        cout<<"  - " << itr->first <<": " << itr->second.s <<endl;
    }
    cout<< stmp2 <<endl;
}

bool Configuration::IsAvailable(string &sName, bool forceexit)
{
    bool tmp = false;
    if( fMpValue.count(sName)==0 )
    {
        cout<<"  Parameter: " << sName <<" is not available " <<endl;
        if( forceexit )
        {
            cout<<" [ERROR] Configuration::IsAvailable " <<endl;
            cout<<"  - Please set " << sName <<" in input parameter file properly " <<endl;
            cout<<" -> EXIT " <<endl;
            exit(-1);
        }
    }
    else{ tmp = true; }
    return tmp;
}

string Configuration::ParseMDTROOT(string filename)
{
    std::size_t found = filename.find("$MDTROOT");
    if (found!=string::npos)
    {
        if (std::getenv("MDTROOT")) filename.replace(found,8,std::getenv("MDTROOT"));
        else
        {
            cout<<" [ERROR] Configuration::ParseMDTROOT" <<endl;
            cout<<"  $MDTROOT is not set " <<endl;
            cout<<"  -> EXIT" <<endl;
            exit(-1);
        }
    }

    return filename;
}