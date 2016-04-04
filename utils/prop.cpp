#include "prop.h"

prop::prop( const char* filename )
{
	state = false;
	name = filename;
	parse();
}

prop::~prop()
{
}

const char* prop::getstring( const char* key, const char* default/*=""*/ ) const
{
	if (!isKeyExists(key))
		return default;
	std::map<std::string, std::string>::const_iterator it = propmap.find(key);
	if (it == propmap.end())
		return default;
	return it->second.c_str();
}

int prop::getint( const char* key, int default /*= 0*/ ) const
{
	if (!isKeyExists(key))
		return default;
	const char* s = getstring(key);
	return s ? atoi(s) : 0;
}

double prop::getdouble( const char* key, double default /*= 0.0*/ ) const
{
	if (!isKeyExists(key))
		return default;
	const char* s = getstring(key);
	return s ? atof(s) : 0;
}

void prop::parse()
{
	FILE* pfile;
	fopen_s(&pfile, name.c_str(), "r");
	if (!pfile)
		return;

	while (fgets(line, 1023, pfile))
		parseline(line);
	fclose(pfile);
	state = true;
}

void prop::parseline(char* line)
{
	trimright(line);
	while (1)
	{
		switch (*line)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			++line;
			continue;
		case '#':
		case ';':
		case 0:
			return;
		}
		char* pos = strchr(line, '=');
		strncpy(key, line, pos - line);
		key[pos-line] = 0;
		trimright(key);
		propmap[key] = trimleft(pos+1);
		return;
	}
}

char* prop::trimleft( char* str )
{
	while (1)
	{
		switch (*str)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			++str;
			continue;
		default:
			return str;
		}
	}
}

void prop::trimright( char* str )
{
	int len = strlen(str);
	while (len)
	{
		switch (str[len-1])
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			--len;
			continue;
		default:
			str[len] = 0;
			return;
		}
	}
}

bool prop::isKeyExists( const char* key ) const
{
	std::map<std::string, std::string>::const_iterator it = propmap.find(key);
	return (it != propmap.end());
}

