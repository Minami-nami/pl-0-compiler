#include "Interpreter.h"

void menu() {
	std::cout << "__________________________________________________________________\n"
				 "1.run\n2.show Pcode\n3.show Symbol Table\n4.show Tokens\n5.exit\n>>";
}

int main(int argc, char *argv[])
{
	Interpreter interpreter(argc, argv);
	while (1) {
		int choice;
		menu();
		if (!(std::cin >> choice)) {
			std::cout << "Invalid Input!\n";
			std::cin.clear();
			std::cout << "__________________________________________________________________\n";
			continue;
		}
		std::cout << "__________________________________________________________________\n";
		switch (choice)
		{
		case 1:
			interpreter.run();
			break;
		case 2:
			interpreter.printPcode();
			break;
		case 3:
			interpreter.printSymbolTable();
			break;
		case 4:
			interpreter.printTokens();
			break;
		case 5:
			exit(0);
		default:
			std::cout << "Invalid Choice!\n";
		}
	}
	
	return 0;
}