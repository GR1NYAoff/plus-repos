#include <iostream>
// string stream | поток строк
#include <sstream> 
using namespace std;
int main()
{
	stringstream ss;
	ss << "22";
	int k = 0;
	ss >> k;
	cout << k << endl;	
}