#include <PL0Parser.h>
#include <Lexer.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <windows.h>

int main(int argc, char *argv[])
{
	SetConsoleOutputCP(65001);
	Parser test;
	if (argc <= 1) {
		std::string s;
		std::cout << "请输入文件路径: ";
		std::cin >> s;
		test.loadFile(s.c_str());
	}
	else
		test.loadFile(argv[1]);
	test.analyze();
	system("pause");
	return 0;
}