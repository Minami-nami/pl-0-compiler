#include "Code.h"
#include <unordered_map>
#include <vector>
#include <cstring>

std::vector<std::string> split(const std::string& str,const std::string& delim) {
	std::vector<std::string> res;
	if("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型
	char *strs = new char[str.length() + 1] ; //不要忘了
	std::strcpy(strs, str.c_str()); 
 
	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());
 
	char *p = std::strtok(strs, d);
	while(p) {
		std::string s = p; //分割得到的字符串转换为string类型
		res.push_back(s); //存入结果数组
		p = strtok(NULL, d);
	}
    delete[] strs;
    delete[] d;
	return res;
}

const std::unordered_map<std::string, oprType> nameToType = {
    {"LIT", oprType::LIT}, {"OPR", oprType::OPR}, {"LOD", oprType::LOD}, {"STO", oprType::STO}, {"CAL", oprType::CAL},
    {"INT", oprType::INT}, {"JMP", oprType::JMP}, {"JPC", oprType::JPC}, {"RED", oprType::RED}, {"WRT", oprType::WRT}, {"POP", oprType::POP}
};

std::string toString(oprType type) {
    switch (type)
    {
    case oprType::LIT: return "LIT"; 
    case oprType::OPR: return "OPR";
    case oprType::LOD: return "LOD";
    case oprType::STO: return "STO";
    case oprType::CAL: return "CAL";
    case oprType::INT: return "INT";
    case oprType::JMP: return "JMP";
    case oprType::JPC: return "JPC";
    case oprType::RED: return "RED";
    case oprType::WRT: return "WRT";
    case oprType::POP: return "POP";
    default: return "";
    }
}

Ins::Ins(const std::string& s) {
    auto list = split(s, " ");
    std::string& insopr = list[0], l = list[1], a = list[2];
    std::unordered_map<std::string, oprType>::const_iterator it = nameToType.find(insopr);
    if (it == nameToType.end()) opr = oprType::OPR;
    else opr = it->second;
    level = std::stoi(l);
    offset = std::stoi(a);
}

std::string Ins::str() const{
    return toString(this->opr) + " " + std::to_string(level) + " " + std::to_string(offset);
}