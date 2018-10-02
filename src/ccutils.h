#ifndef __CCUTILS_H__
#define __CCUTILS_H__

#include "common.h"

#define TAG_UESR 10000
#define TAG_DIALOG TAG_UESR+1
#define TAG_MENU TAG_DIALOG+1
#define TAG_TOAST TAG_MENU+1

class Utils
{
public:
	static std::string getTimeString();
	static std::string getCurrentTime();
	static long getTime();
	static std::string gbk2utf8(const string text);
	static vector<string> splitString(const string& str, char sp, bool greedy=false);
	static map<string, string> parseKV(const string&str, char spk='&', char spv='=');
	static std::string itos(int i);
	static string trim(const string& str);
	static int split(const string& str, vector<string>& ret_, string sep = ",");
	static string replace(const string& str, const string& src, const string& dest);
};

#endif
