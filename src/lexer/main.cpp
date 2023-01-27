#include <Lexer.h>
#include <iostream>
#include <filesystem>

int main(int argc, char *argv[])
{
	Lexer test;
	test.LoadFile(argv[1]);
	while (!test.End()) {
		auto [type, _] = test.getToken();
		//std::cout << static_cast<int>(type) << std::endl;
	}
	test.WriteFile();
	system("pause");
	return 0;
}