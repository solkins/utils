#ifndef PROP_H
#define PROP_H

#include <stdio.h>
#include <map>
#include <string>

class prop
{
public:
	prop(const char* filename);
	~prop();

public:
	bool isok() const { return state; }
	const char* getstring(const char* key, const char* default="") const;
	int getint(const char* key, int default=0) const;
	double getdouble(const char* key, double default=0.0) const;

private:
	void parse();
	void parseline(char* line);
	char* trimleft(char* str);
	void trimright(char* str);
	bool isKeyExists(const char* key) const;

private:
	std::string name;
	std::map<std::string, std::string> propmap;
	char line[1024], key[256];
	bool state;
};

#endif // PROP_H
