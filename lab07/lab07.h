#pragma once

#include <iostream>
#include "windows.h"
#include "tchar.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"

using namespace std;

//вихідні дані за варіантом 17
const int mnPts = 20;
const int mxPts = 50;

//const int numG = (17 * 97) % 25 + 10;
const int numG = 106;
//const float maxRad = ((17 * 491) % 50) / 200 + 0.05;
const float maxRad = 0.52f;

//об'єкт для кластеризації
struct s_point
{
	float x, y; //параметри об'єкту
	int cluster_num; //кластер до якого належить
	bool migrate; //мігрував на останньому кроці
	s_point() {};
	s_point(float x_, float y_) : x(x_), y(y_),
		cluster_num(-1), migrate(true) {}
};
//кластер
struct s_cluster
{
	float x, y; //центр кластеру
	int pt_count; //розмір кластеру
	s_cluster() : x(0), y(0), pt_count(0) {}
	void add_point(const s_point& pt)
	{
		x += pt.x;
		y += pt.y;
		++pt_count;
	}
	void calc_pos()
	{
		if (pt_count < 2)
			return;
		x /= pt_count;
		y /= pt_count;
	}
};

//метод k-середніх
struct s_kmeans
{
	//дані для кластеризації
	s_point* objects;
	s_cluster* clusters;
	int pt_count, cl_count;
	int migrate;
	int step;
	s_kmeans(int num_g, int mn_pts, int mx_pts, float max_r);
	virtual ~s_kmeans() { delete[] objects; delete[] clusters; }
	//процент мігруючих об'єктів
	float migrants_perc() const { return 100.f * migrate / pt_count; }
	//Евклідова міра відстаней
	float distance(const s_cluster& cl, const s_point& pt)
	{
		float dx = cl.x - pt.x, dy = cl.y - pt.y;
		return sqrtf(dx * dx + dy * dy);
	}
	//побудова кластерів
	void init_clusters();
	//розподіл об'єктів за кластерами
	void dispatch_objects();
	//розрахунок нових центрів кластерів з урахуванням об'єктів
	void calc_center_pos();
	//наступний крок алгоритму
	void next_step()
	{
		++step;
		calc_center_pos();
		dispatch_objects();
	}
};
//малювання кластерів у вікні додатку
void draw_clusters(const s_kmeans& st, HDC wdc, HWND wnd);
