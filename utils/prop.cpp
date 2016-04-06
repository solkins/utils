#include "prop.h"
#include <string.h>
#include <fstream>

prop::prop( const char* filename )
{
    parse(filename);
}

prop::~prop()
{
}

const char* prop::getstring( const char* key, const char* dvalue) const
{
    auto it = propmap.find(key);
    if (it == propmap.end())
        return dvalue;
	return it->second.c_str();
}

int prop::getint( const char* key, int dvalue) const
{
    if (!haskey(key))
        return dvalue;
	const char* s = getstring(key);
    return s ? atoi(s) : dvalue;
}

double prop::getdouble( const char* key, double dvalue) const
{
    if (!haskey(key))
        return dvalue;
	const char* s = getstring(key);
    return s ? atof(s) : dvalue;
}

void prop::parse(const char* filename)
{
    std::ifstream in(filename);
    if (in.is_open())
    {
        char line[1024];
        while (!in.eof())
        {
            in.getline(line, 1023);
            parseline(line);
        }
    }
}

void prop::parseline(char* line)
{
    std::string l(line);
    trim(l);
    if (l.empty())
        return;

    char c = l[0];
    if (c == '#' || c == ';')
        return;

    size_t pos = l.find_first_of('=');
    std::string key = l.substr(0, pos);
    l = l.substr(pos + 1);
    trim(key);
    trim(l);
    propmap[key] = l;
}

void prop::trim(std::string& text)
{
    if(!text.empty())
    {
        text.erase(0, text.find_first_not_of(" \n\r\t"));
        text.erase(text.find_last_not_of(" \n\r\t") + 1);
    }
}

bool prop::haskey( const char* key ) const
{
    return (propmap.find(key) != propmap.end());
}

