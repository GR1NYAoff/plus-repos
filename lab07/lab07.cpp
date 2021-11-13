#include "lab07.h"

s_kmeans kmeans(numG, mnPts, mxPts, maxRad);
//функція обробки повідомлень
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		//код завершення додатку
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	//натиснута будь-яка кнопка на клавіатурі
	case WM_KEYDOWN:
	{
		if (!kmeans.migrate)
			break;
		kmeans.next_step();
		PostMessage(hWnd, WM_PAINT, 0, 0);
		break;
	}
	//малювання вікна
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
	const TCHAR* title = L"Кластерний аналіз. Лабораторна робота №7. КІТ-420г Цяпа";
	//додаємо в систему віконний клас
	WNDCLASSEX wcex{ sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = class_name;
	RegisterClassEx(&wcex);
	//створюємо вікно додатку в якому буде відображатися інформація
	HWND wnd = CreateWindow(class_name, title,
		WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_POPUP,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		0, 0, hInstance, 0);
	if (!wnd)
		return -1;
	//якщо вікно створено, відображаємо його на екрані
	ShowWindow(wnd, nCmdShow);
	UpdateWindow(wnd);
	//починаємо обробку повідомлень яки надходять до вікна
	//до тих пір, поки користувач не закриє вікно додатку
	MSG msg{};
	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			DispatchMessage(&msg);
	} while (WM_QUIT != msg.message);
	return (int)msg.wParam;
}
//малювання стану кластеризації
//викликається після кожного кроку
void draw_clusters(const s_kmeans& st, HDC wdc, HWND wnd)
{
	//створюємо таблицю кольорів на першому виклику
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
	//розміри вікна додатку в пікселях
	RECT rc = {};
	GetClientRect(wnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	int size = min(width, height);
	//очищення всього вікна
	Rectangle(wdc, rc.left, rc.top, rc.right, rc.bottom);
	HPEN pen = 0;
	COLORREF prev_color = 0;
	//малюємо об'єкти у вигляді різнокольорових кіл
	const int sz = max(4, int(size * 0.0075f)), sz2 = sz / 2;
	for (int i = 0; i < st.pt_count; ++i)
	{
		const s_point& pt = st.objects[i];
		//створюємо новий олівець якщо змінився колір кластеру
		COLORREF color = cl_colors[pt.cluster_num % color_cnt];
		if (!pen || prev_color != color)
		{
			if (pen)
				DeleteObject(pen);
			pen = CreatePen(PS_SOLID, 2, color);
			SelectObject(wdc, pen);
			prev_color = color;
		}// if( !pen || prev_color != color )
		//малюємо об'єкт
		int x = int(pt.x * width), y = int(pt.y * height);
		Ellipse(wdc, x - sz, y - sz, x + sz, y + sz);
		//малюємо крапку, якщо об'єкт мігрував
		if (pt.migrate)
			Ellipse(wdc, x - sz2, y - sz2, x + sz2, y + sz2);
	}// for( int i = 0; i < st.pt_count; ++i )
	if (pen)
		DeleteObject(pen);
	//олівець чорного кольору для центрів кластерів
	HPEN penc = CreatePen(PS_SOLID, 2, RGB(32, 32, 32));
	SelectObject(wdc, penc);
	const int csz = sz * 2;
	SIZE siz = {};
	//малюємо центри кластерів у вигляді хрестиків
	char buf[256];
	for (int i = 0; i < st.cl_count; ++i)
	{
		const s_cluster& cl = st.clusters[i];
		int x = int(cl.x * width), y = int(cl.y * height);
		MoveToEx(wdc, x, y - csz, 0);
		LineTo(wdc, x, y + csz);
		MoveToEx(wdc, x - csz, y, 0);
		LineTo(wdc, x + csz, y);
		//малюємо напис - розмір кластеру, центровано під хрестом
		int sl = sprintf_s(buf, sizeof(buf), "%u", cl.pt_count);
		GetTextExtentPoint32A(wdc, buf, sl, &siz);
		TextOutA(wdc, x - siz.cx / 2, y + csz, buf, sl);
	}// for( int i = 0; i < st.cl_count; ++i )
	//малюємо номер кроку, та кількість і процент об'єктів,
	//що мігрувало на останньому кроці
	int sl = sprintf_s(buf, sizeof(buf), "Step %u Migrate %u (%5.2f%%)",
		st.step, st.migrate, st.migrants_perc());
	GetTextExtentPoint32A(wdc, buf, sl, &siz);
	TextOutA(wdc, ((rc.right - rc.left) - siz.cx) / 2,
		rc.bottom - 1 - siz.cy, buf, sl);
	DeleteObject(penc);
}
