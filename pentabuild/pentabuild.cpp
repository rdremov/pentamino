#include "../ui/gdi.h"
#include "../pentamino/figure.h"

class BuildWnd : public Wnd {
	static const int pix = 30;

public:
	BuildWnd() {
		_brushes[0].Create(Color::dkgrey);
		_brushes[1].Create(Color::ltgrey);
		_i1 = _j1 = 0;
		_i2 = _j2 = 0;
		_drag = false;
	}

	~BuildWnd() {}

protected:
	bool OnPaint(DC* pDC) override {
		DrawBk(pDC);
		DrawLines(pDC);
		DrawLabels(pDC);
		DrawStatus(pDC);
		return true;
	}

	bool OnKeyDown(Key key) override {
		if (key == 'S')
			Save();
		return true;
	}

	bool OnLButtonDown(int x, int y) override {
		int i1, j1;
		if (Test(x, y, i1, j1)) {
			_copy = _data;
			_data.Toggle(i1, j1);
			_drag = true;
			_i1 = _i2 = i1;
			_j1 = _j2 = j1;
			Refresh(i1, j1);
		}
		return true;
	}

	bool OnLButtonUp(int x, int y) override {
		if (_drag)
			Drag(x, y);
		_i1 = _j1 = 0;
		_i2 = _j2 = 0;
		_drag = false;
		Invalidate();
		return true;
	}
	
	bool OnMouseMove(int x, int y) override {
		if (_drag)
			Drag(x, y);
		return true;
	}

	bool Test(int x, int y, int& i, int& j) const {
		int x1 = pix;
		int x2 = pix + pix * Field::N;
		if (x >= x1 && x < x2 && y >= x1 && y < x2) {
			i = (x - x1) / pix;
			j = (y - x1) / pix;
			return true;
		}
		i = j = 0;
		return false;
	}

	void Drag(int x, int y) {
		int i2, j2;
		if (Test(x, y, i2, j2)) {
			if (i2 != _i2 || j2 != _j2) {
				_data = _copy;
				int imin, imax;
				norm(_i1, i2, imin, imax);
				int jmin, jmax;
				norm(_j1, j2, jmin, jmax);
				for (int j=jmin; j<=jmax; j++)
					for (int i=imin; i<=imax; i++)
						_data.Toggle(i, j, _copy.Get(_i1, _j1));
				Refresh(_i2, _j2); // old
				Refresh(i2, j2); // new
				_i2 = i2, _j2 = j2;
			}
		}
	}

	static int left(int i) {return pix + pix * i;}
	static int right(int i) {return left(i) + pix;}
	static void norm(int x1, int x2, int& xmin, int& xmax) {
		xmin = x1, xmax = x2;
		if (x1 > x2)
			xmin = x2, xmax = x1;
	}

	void Refresh(int i, int j) {
		int imin, imax;
		norm(_i1, i, imin, imax);
		int jmin, jmax;
		norm(_j1, j, jmin, jmax);
		Invalidate(left(imin), left(jmin), right(imax), right(jmax), false);
	}

	void DrawLines(DC* pDC) {
		Pen pen(1);
		Select sp(pDC, pen);
		int x1 = pix;
		int x2 = pix + pix * Field::N;
		int y = pix;
		for (int n=0; n<=Field::N; n++) {
			pDC->Line(x1, y, x2, y);
			pDC->Line(y, x1, y, x2);
			y += pix;
		}
	}

	void DrawStatus(DC* pDC) {
		int l, t, r, b;
		int cnt = _data.Bounds(l, t, r, b);
		char text[64];
		sprintf_s(text, "Total = %d", cnt);

		Font font(pix * 2);
		Select sf(pDC, font);

		int x = pix + pix * Field::N + 2 * pix;
		int y = pix + pix * Field::N / 2;
		pDC->Text(x, y, text);

		if (cnt == total_count)
			pDC->Text(x, y + 2 * pix, "Press S to save!");
	}

	void DrawLabels(DC* pDC) {
		int x = 2;
		int y = pix + pix / 4;
		Font font(pix * 2 / 3);
		Select sf(pDC, font);
		for (int n=0; n<Field::N; n++) {
			char label[16];
			sprintf_s(label, "%d", n+1);
			pDC->Text(x, y, label);
			pDC->Text(y, x, label);
			y += pix;
		}
	}

	void DrawBk(DC* pDC) {
		Pen pen;
		Select sp(pDC, pen);
		int y = pix;
		for (int j=0; j<Field::N; j++) {
			int x = pix;
			for (int i=0; i<Field::N; i++) {
				Select sb(pDC, _brushes[_data.IsEmpty(i, j)]);
				pDC->Rect(x, y, x+pix, y+pix);
				x += pix;
			}
			y += pix;
		}
	}

	void Save() {
		int l, t, r, b;
		int cnt = _data.Bounds(l, t, r, b);
		if (cnt == total_count) {
			FileName fn(true);
			if (*fn.name)
				_data.Save(fn.name, l, t, r, b);
		}
	}

private:
	Brush _brushes[2];
	Field _data;
	Field _copy;
	int _i1, _j1;
	int _i2, _j2;
	bool _drag;
};

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow) {
	BuildWnd wnd;
	Instance inst(hInst, &wnd);
	inst.Init(nCmdShow);
	return inst.Run();
}
