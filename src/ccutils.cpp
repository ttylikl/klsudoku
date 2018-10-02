#include "ccutils.h"

string  Utils::getTimeString()
{
	char buf[100];
	return getTimeStr(time(0), buf, 100);
}
string Utils::getCurrentTime()
{
	time_t t;
	time(&t);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %X", localtime((&t)));

	string timeStr = tmp;
	return timeStr;
}

long Utils::getTime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

vector<string>  Utils::splitString(const string& str, char sp, bool greedy){
	char *buf = new char[str.size()+1];
	memset(buf, 0, str.size() + 1);
	strcpy(buf, str.c_str());
	vector<string> ret;
	char *p = buf;
	char *s = NULL;
	while (*p!='\0'){
		s = strchr(p, sp);
		if (s == NULL){
			ret.push_back(p);
			if (greedy){
				ret.push_back(""); //at least two elements: one for key, second for value
			}
			break;
		}
		*s = '\0';
		if (greedy){ //greedy mode: from key=value=value=value to  key : value=value=value....
			ret.push_back(p);
			ret.push_back(s + 1);
			break;
		}
		ret.push_back(p);
		p = s + 1;
	}
	free(buf);
	return ret;
}

map<string, string>  Utils::parseKV(const string&str, char spk , char spv ){
	vector<string> kvstr = splitString(str, spk);
	map<string, string> ret;
	for (unsigned int i = 0; i < kvstr.size(); i++)
	{
		vector<string> kv = splitString(kvstr[i], spv, true);
		ret[kv[0]] = kv[1];
	}
	return ret;
}

string Utils::itos(int i){
	char buf[20];
	sprintf(buf, "%d", i);
	return string(buf);
}

string Utils::trim(const string& str)
{
	string::size_type pos = str.find_first_not_of(' ');
	if (pos == string::npos)
	{
		return str;
	}
	string::size_type pos2 = str.find_last_not_of(' ');
	if (pos2 != string::npos)
	{
		return str.substr(pos, pos2 - pos + 1);
	}
	return str.substr(pos);
}

int Utils::split(const string& str, vector<string>& ret_, string sep)
{
	if (str.empty())
	{
		return 0;
	}

	string tmp;
	string::size_type pos_begin = str.find_first_not_of(sep);
	string::size_type comma_pos = 0;

	while (pos_begin != string::npos)
	{
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != string::npos)
		{
			tmp = str.substr(pos_begin, comma_pos - pos_begin);
			pos_begin = comma_pos + sep.length();
		}
		else
		{
			tmp = str.substr(pos_begin);
			pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
			ret_.push_back(tmp);
			tmp.clear();
		}
	}
	return 0;
}

string Utils::replace(const string& str, const string& src, const string& dest)
{
	string ret;

	string::size_type pos_begin = 0;
	string::size_type pos = str.find(src);
	while (pos != string::npos)
	{
		//cout << "replacexxx:" << pos_begin << " " << pos << "\n";
		ret.append(str.data() + pos_begin, pos - pos_begin);
		ret += dest;
		pos_begin = pos + 1;
		pos = str.find(src, pos_begin);
	}
	if (pos_begin < str.length())
	{
		ret.append(str.begin() + pos_begin, str.end());
	}
	return ret;
}
