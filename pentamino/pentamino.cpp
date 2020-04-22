#include "figure.h"
#include "../ui/console.h"
#include "../ui/gdi.h"
#include "../ui/elapsed.h"

static const int pix = 15;

static const Color colors[] = {
	{0x80, 0x00, 0x00}, // maroon
	{0x9a, 0x63, 0x24}, // brown
	{0x80, 0x80, 0x00}, // olive
	{0x46, 0x99, 0x90}, // teal
	{0x00, 0x00, 0x75}, // navy
	{0xe6, 0x19, 0x48}, // red
	{0xf5, 0x82, 0x31}, // orange
	{0xff, 0xe1, 0x19}, // yellow
	{0xbf, 0xef, 0x45}, // lime
	{0x3c, 0xb4, 0x4b}, // green
	{0x42, 0xd4, 0xf4}, // cyan
	{0x43, 0x63, 0xd8}, // blue
	{0x91, 0x1e, 0xb4}, // purple
	{0xf0, 0x32, 0xe6}, // magenta
	{0xa9, 0xa9, 0xa9}, // grey
	{0xfa, 0xbe, 0xbe}, // pink
	{0xff, 0xd8, 0xb1}, // apricot
	{0xff, 0xfa, 0xc8}, // beige
	{0xaa, 0xff, 0xc3}, // mint
	{0xe6, 0xbe, 0xff}, // lavender
};

static void draw_field(Field const& f, DC* pDC, Brush* brushes, int x0, int y0) {
	int y = y0;
	for (IND j=0; j<f.Height(); j++) {
		int x = x0;
		for (IND i=0; i<f.Width(); i++) {
			Select sb(pDC, brushes[f.Get(i, j)]);
			pDC->Rect(x, y, x + pix + 1, y + pix + 1);
			x += pix;
		}
		y += pix;
	}
}

class MainWnd : public Wnd {
public:
	MainWnd() {
		for (int i=0; i<figure_count; i++)
			_brushes[i].Create(colors[i]);
		_brushes[figure_count].Create(Color::white);
	}
	
protected:
	bool OnKeyDown(Key key) override {
		if (key == 'L') {
			FileName fn(false);
			if (*fn.name) {
				_field.Load(fn.name);
				_sol.resize(0);
				Elapsed el;
				Context cntx{_sol};
				printf("SolveMT(mirror=%d):\n", cntx.mirror);
				_field.SolveMT(cntx);
				printf("  dbgcnt = %lld\n", cntx.dbgcnt);
				printf("  elapsed = %g sec\n", el.sec());
				printf("  solutions = %d\n", (int)_sol.size());
				Invalidate();
			}
		}
		return true;
	}
	
	bool OnPaint(DC* pDC) override {
		if (!_sol.empty()) {
			int w, h;
			GetSize(w, h);

			int w1 = (1 + _sol[0].Width()) * pix;
			int h1 = (1 + _sol[0].Height()) * pix;
			int nx = w / w1;

			Pen pen(1);
			Select sp(pDC, pen);

			int n = 0, y = pix;
			bool done = false;
			for (int j=0; !done && y<h; j++) {
				int x = pix;
				for (int i=0; !done && i<nx; i++) {
					draw_field(_sol[n], pDC, _brushes, x, y);
					x += w1;
					if (++n == _sol.size())
						done = true;
				}
				y += h1;
			}
		}
		return true;
	}

private:
	Brush _brushes[figure_count+1];
	Solution _sol;
	Field _field;
};

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow) {
	Console cons;
	MainWnd wnd;
	Instance inst(hInst, &wnd);
	inst.Init(nCmdShow);
	return inst.Run();
}
