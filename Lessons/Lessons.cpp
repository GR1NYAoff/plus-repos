#include <iostream>
#include <typeinfo>
#include <string>
using namespace std;

int main()
{
	string str = "The end of programm";
	auto number = 'v';
	int value = 10;
	wchar_t symbol = 'c';
	char16_t s = 'b';
	/*cout << value << endl;
	cout << (char)symbol << endl;
	wcout << (wchar_t)s << endl;
	cout << endl;*/
	/*wcout << (char16_t)symbol << endl;*/
	cout << "symbol: " << (char)symbol << endl;
	cout << "sizeof(char): " << sizeof(symbol) << "\tbyte" << endl;

	cout << endl;
	cout << "int: " << value << endl;
	cout << "sizeof(int): " << sizeof(value) << "\tbyte" << endl;

	cout << "type of variable (number): " << typeid(number).name() << endl;
	cout << endl;
	cout << str << endl;


}
