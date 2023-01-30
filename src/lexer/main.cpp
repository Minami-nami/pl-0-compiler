#include <Lexer.h>
#include <iostream>
#include <windows.h>
#include <filesystem>

int main(int argc, char *argv[])
{
	SetConsoleOutputCP(65001);
	Lexer test;
	if (argc <= 1) {
		std::string s;
		std::cout << "请输入文件路径: ";
		std::cin >> s;
		test.LoadFile(s.c_str());
	}
	else
		test.LoadFile(argv[1]);
	for(;;) {
		auto [type, it] = test.getToken();
		if (type == TokenType::ENDOFFILE) break;
		//std::cout << static_cast<int>(type) << std::endl;
	}
	test.WriteFile();
	return 0;
}