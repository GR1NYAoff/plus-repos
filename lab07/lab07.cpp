#include "lab07.h"

s_kmeans kmeans(numG, mnPts, mxPts, maxRad);
//������� ������� ����������
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		//��� ���������� �������
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	//��������� ����-��� ������ �� ��������
	case WM_KEYDOWN:
	{
		if (!kmeans.migrate)
			break;
		kmeans.next_step();
		PostMessage(hWnd, WM_PAINT, 0, 0);
		break;
	}
	//��������� ����
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		HDC wdc = GetDC(hWnd);
		draw_clusters(kmeans, wdc, hWnd);
		ReleaseDC(hWnd, wdc);
		EndPaint(hWnd, &ps);
		break;
	}
	default: return DefWindowProc(hWnd, Message, wParam, lParam);
	}
	return 0;
}
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	const TCHAR* class_name = L"Prog Lab Class";
	const TCHAR* title = L"���������� �����. ����������� ������ �7. ʲ�-420� ����";
	//������ � ������� ������� ����
	WNDCLASSEX wcex{ sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = class_name;
	RegisterClassEx(&wcex);
	//��������� ���� ������� � ����� ���� ������������ ����������
	HWND wnd = CreateWindow(class_name, title,
		WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_POPUP,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		0, 0, hInstance, 0);
	if (!wnd)
		return -1;
	//���� ���� ��������, ���������� ���� �� �����
	ShowWindow(wnd, nCmdShow);
	UpdateWindow(wnd);
	//�������� ������� ���������� ��� ��������� �� ����
	//�� ��� ��, ���� ���������� �� ����� ���� �������
	MSG msg{};
	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			DispatchMessage(&msg);
	} while (WM_QUIT != msg.message);
	return (int)msg.wParam;
}
//��������� ����� �������������
//����������� ���� ������� �����
void draw_clusters(const s_kmeans& st, HDC wdc, HWND wnd)
{
	//��������� ������� ������� �� ������� �������
	static int color_cnt = 0;
	static COLORREF cl_colors[32];
	if (!color_cnt)
	{
		color_cnt = sizeof(cl_colors) / sizeof(COLORREF);
		for (int i = 0; i < color_cnt; ++i)
		{
			int r = rand() % 14, g = rand() % 14, b = rand() % 14;
			cl_colors[i] = RGB((r + 1) * 16, (g + 1) * 16, (b + 1) * 16);
		}// for( int i = 0; i < color_cnt; ++i )
	}// if( !color_cnt )
	//������ ���� ������� � �������
	RECT rc = {};
	GetClientRect(wnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	int size = min(width, height);
	//�������� ������ ����
	Rectangle(wdc, rc.left, rc.top, rc.right, rc.bottom);
	HPEN pen = 0;
	COLORREF prev_color = 0;
	//������� ��'���� � ������ �������������� ��
	const int sz = max(4, int(size * 0.0075f)), sz2 = sz / 2;
	for (int i = 0; i < st.pt_count; ++i)
	{
		const s_point& pt = st.objects[i];
		//��������� ����� ������ ���� ������� ���� ��������
		COLORREF color = cl_colors[pt.cluster_num % color_cnt];
		if (!pen || prev_color != color)
		{
			if (pen)
				DeleteObject(pen);
			pen = CreatePen(PS_SOLID, 2, color);
			SelectObject(wdc, pen);
			prev_color = color;
		}// if( !pen || prev_color != color )
		//������� ��'���
		int x = int(pt.x * width), y = int(pt.y * height);
		Ellipse(wdc, x - sz, y - sz, x + sz, y + sz);
		//������� ������, ���� ��'��� �������
		if (pt.migrate)
			Ellipse(wdc, x - sz2, y - sz2, x + sz2, y + sz2);
	}// for( int i = 0; i < st.pt_count; ++i )
	if (pen)
		DeleteObject(pen);
	//������ ������� ������� ��� ������ ��������
	HPEN penc = CreatePen(PS_SOLID, 2, RGB(32, 32, 32));
	SelectObject(wdc, penc);
	const int csz = sz * 2;
	SIZE siz = {};
	//������� ������ �������� � ������ ��������
	char buf[256];
	for (int i = 0; i < st.cl_count; ++i)
	{
		const s_cluster& cl = st.clusters[i];
		int x = int(cl.x * width), y = int(cl.y * height);
		MoveToEx(wdc, x, y - csz, 0);
		LineTo(wdc, x, y + csz);
		MoveToEx(wdc, x - csz, y, 0);
		LineTo(wdc, x + csz, y);
		//������� ����� - ����� ��������, ���������� �� �������
		int sl = sprintf_s(buf, sizeof(buf), "%u", cl.pt_count);
		GetTextExtentPoint32A(wdc, buf, sl, &siz);
		TextOutA(wdc, x - siz.cx / 2, y + csz, buf, sl);
	}// for( int i = 0; i < st.cl_count; ++i )
	//������� ����� �����, �� ������� � ������� ��'����,
	//�� �������� �� ���������� �����
	int sl = sprintf_s(buf, sizeof(buf), "Step %u Migrate %u (%5.2f%%)",
		st.step, st.migrate, st.migrants_perc());
	GetTextExtentPoint32A(wdc, buf, sl, &siz);
	TextOutA(wdc, ((rc.right - rc.left) - siz.cx) / 2,
		rc.bottom - 1 - siz.cy, buf, sl);
	DeleteObject(penc);
}
