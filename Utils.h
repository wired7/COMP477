#pragma once
#include <vector>
#include <sstream>
using namespace std;

class Utils
{
public:
	static vector<string> split(const string &s, char delim) {
		stringstream ss(s);
		string item;
		vector<string> tokens;
		while (getline(ss, item, delim)) {
			tokens.push_back(item);
		}
		return tokens;
	}
};