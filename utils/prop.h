#ifndef PROP_H
#define PROP_H

#include <map>
#include <string>

class prop
{
public:
	prop(const char* filename);
	~prop();

public:
    const char* getstring(const char* key, const char* dvalue=nullptr) const;
    int getint(const char* key, int dvalue=0) const;
    double getdouble(const char* key, double dvalue=0.0) const;

private:
    void parse(const char* filename);
	void parseline(char* line);
    void trim(std::string& text);
    bool haskey(const char* key) const;

private:
	std::map<std::string, std::string> propmap;
};

#endif // PROP_H
