#include <bits/stdc++.h>
#include <windows.h>

using namespace std;

int main(int argc, char** argv)
{
    SetConsoleOutputCP(65001);
    stringstream CodeStream;
    filesystem::path Path;
    auto loadfile = [&CodeStream, &Path](const char* FilePath) {
        Path = std::string(FilePath);
        std::ifstream CodeFile;
        CodeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            CodeFile.open(Path);
            CodeStream << CodeFile.rdbuf();
            CodeStream << '\n';
            CodeFile.close();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "Load Error: " << e.what() << std::endl;
            std::cout << "From " << Path << std::endl;
            return false;
        }
        return true;
    };
	if (argc <= 1) {
		std::string s;
		std::cout << "请输入文件路径: ";
		std::cin >> s;
		loadfile(s.c_str());
	}
	else
		loadfile(argv[1]);
    printf("%s", Path.string().c_str());
    return 0;
}
