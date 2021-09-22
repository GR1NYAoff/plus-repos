#include <iostream>
#include "lab02.h"
using namespace std;
void make_rnd_vals(t_rand_val* A, int cnt, t_rand_val L, t_rand_val H)
{
	for (int i = 0; i < cnt; i++)
	{
		float Random = rand();
		float R0_1 = float(Random) / RAND_MAX;
		t_rand_val RL_H = t_rand_val(R0_1 * (H - L)) + L;
		A[i] = RL_H;
	}
	//Вивод масиву в консоль
	for (int i = 0; i < cnt; i++)
	{
		cout << "element N " << i + 1<< ": " << A[i] << endl;
	}
}

int main()
{
	//Змінні для діапазону
	int min, max;	
	min = -123;
	max = 321;

	srand((unsigned int)time(0));
	t_rand_val R[N]; 
	make_rnd_vals(R, N, min, max);

	return 0;
}
