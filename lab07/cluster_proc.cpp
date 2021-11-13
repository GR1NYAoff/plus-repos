#include "lab07.h"
s_kmeans::s_kmeans(int num_g, int mn_pts, int mx_pts, float max_r) :
	pt_count(0), cl_count(num_g), migrate(0), step(0)
{
	srand((unsigned int)time(0));
	clusters = new s_cluster[cl_count];
	const int max_objects = cl_count * mxPts;
	objects = new s_point[max_objects];
	const float pi = 3.14f;
	const float border = 1.f - 2.f * max_r;
	for (int g = 0; g < num_g; ++g)
	{
		float x0 = max_r + border * rand() / RAND_MAX;
		float y0 = max_r + border * rand() / RAND_MAX;
		int count = mn_pts + (int)(float(rand())
			/ RAND_MAX * (mx_pts - mn_pts));
		for (int p = 0; p < count; ++p)
		{
			float r = max_r * sqrtf(float(rand()) / RAND_MAX);
			float a = 2.f * pi * rand() / RAND_MAX;
			float x = x0 + sinf(a) * r, y = y0 + cosf(a) * r;
			objects[pt_count++] = s_point(x, y);
		}// for( int p = 0; p < count; ++p )
	}// for( int g = 0; g < num_g; ++g )
	//перший крок алгоритму
	init_clusters();
	dispatch_objects();
}
//побудова кластерів
void s_kmeans::init_clusters()
{
	for (int i = 0; i < cl_count; ++i)
	{
		int base_pt = rand() % pt_count;
		clusters[i].x = objects[base_pt].x;
		clusters[i].y = objects[base_pt].y;
	}// for( int i = 0; i < cl_count; ++i )
}
//розподіл об'єктів за кластерами
void s_kmeans::dispatch_objects()
{
	migrate = 0;
	for (int p = 0; p < pt_count; ++p)
	{
		s_point& pt = objects[p];
		int c = -1;
		float dist;
		for (int i = 0; i < cl_count; ++i)
		{
			float dist1 = distance(clusters[i], pt);
			if (c >= 0 && dist1 > dist)
				continue;
			dist = dist1;
			c = i;
		}// for( int i = 0; i < cl_count; ++i )
		pt.migrate = (pt.cluster_num != c);
		migrate += pt.migrate;
		pt.cluster_num = c;
		++clusters[c].pt_count;
	}// for( int p = 0; p < pt_count; ++p )
}
//розрахунок нових центрів кластерів з урахуванням об'єктів
void s_kmeans::calc_center_pos()
{
	for (int i = 0; i < cl_count; ++i)
	{
		clusters[i] = s_cluster();
		for (int p = 0; p < pt_count; ++p)
		{
			if (objects[p].cluster_num == i)
				clusters[i].add_point(objects[p]);
		}// for( int p = 0; p < pt_count; ++p )
		clusters[i].calc_pos();
	}// for( int i = 0; i < cl_count; ++i )
}