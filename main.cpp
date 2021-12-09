#include "SameBinaryFiles.hpp"

int main()
{
	std::string first;
	std::string second;

	std::cout << "Write two folders in which you would like to find a match." << std::endl;
	std::cout << "Write first directory: ";
	std::cin >> first;
	std::cout << "Write second directory: ";
	std::cin >> second;
	try
	{
		SameBinary a(first, second);
		a.printSame();
	}
	catch (const char * ex)
	{
		std::cout << ex << std::endl;
	}
}