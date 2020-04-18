#include "stdafx.h"
#include "figure.h"

const int w1 = 40;
static const char piece_count = 5;
static const char figure_count = 12;

static const char figures[figure_count][piece_count][2] = {
	0,0, 2,0, 0,1, 1,1, 2,1,
	1,0, 0,1, 1,1, 2,1, 1,2,
	1,0, 0,1, 1,1, 0,2, 1,2,
	1,0, 0,1, 1,1, 1,2, 2,2,
	0,0, 0,1, 0,2, 0,3, 0,4,
	0,0, 1,0, 2,0, 1,1, 1,2,
	1,0, 2,0, 3,0, 0,1, 1,1,
	1,0, 2,0, 0,1, 1,1, 0,2,
	0,0, 1,0, 1,1, 1,2, 2,2,
	2,0, 2,1, 0,2, 1,2, 2,2,
	0,0, 0,1, 1,1, 2,1, 3,1,
	2,0, 0,1, 1,1, 2,1, 3,1,
};

static const Color colors[figure_count+1] = {
	{0xcc, 0xff, 0xcc},
	{0xff, 0xcc, 0x33},
	{0xcc, 0x99, 0xff},
	{0xcc, 0x66, 0x99},
	{0xff, 0x66, 0x33},
	{0x99, 0x66, 0xcc},
	{0x33, 0x66, 0x99},
	{0x00, 0xcc, 0xff},
	{0xcc, 0xff, 0xff},
	{0x33, 0xff, 0x33},
	{0xff, 0x33, 0x33},
	{0x66, 0x66, 0x66},
	{0xc0, 0xc0, 0xc0}, // vacant
};

class Piece {
public:
	Piece(char index) : _color(colors[index]) {
		memcpy(_data, figures[index], sizeof(_data));
		Update();
	}

	void Rotate() {
		for (char n=0; n<piece_count; n++) {
			char temp = _data[n][0];
			_data[n][0] = -_data[n][1];
			_data[n][1] = temp;
		}
		Update();
	}

	void Update() {
		char x1, y1, x2, y2;
		x1 = y1 = piece_count;
		x2 = y2 = 0;

		for (char n=0; n<piece_count; n++) {
			if (x1 > _data[n][0]) x1 = _data[n][0];
			if (x2 < _data[n][0]) x2 = _data[n][0];
			if (y1 > _data[n][1]) y1 = _data[n][1];
			if (y2 < _data[n][1]) y2 = _data[n][1];
		}

		for (char n=0; n<piece_count; n++) {
			_data[n][0] -= x1;
			_data[n][1] -= y1;
		}

		_w = x2 - x1 + 1;
		_h = y2 - y1 + 1;
	}

	char _w, _h;
	char _data[piece_count][2];
	Color _color;
};

typedef char Field[6][10];

struct Context {
	DC* pDC;
	Brush* brushes;
	int index;
	int count;
	Field solution;

	void Draw(Field const& field) {
		int y = w1;
		for (char j=0; j<6; j++) {
			int x = w1;
			for (char i=0; i<10; i++) {
				Select sb(pDC, brushes[field[j][i]]);
				pDC->Rect(x, y, x + w1 + 1, y + w1 + 1);
				x += w1;
			}
			y += w1;
		}
	}
};

class Figure {
	Field _data;

public:
	Figure() {
		for (char j=0; j<6; j++) {
			for (char i=0; i<10; i++)
				_data[j][i] = figure_count;
		}
	}

	Figure(const Field& data) {
		memcpy(_data, data, sizeof(Field));
	}

	bool Solve(Context& cntx) {
		cntx.Draw(_data);

		if (cntx.index == figure_count) {
			memcpy(cntx.solution, _data, sizeof(Field));
			return true;
		}

		Piece piece(cntx.index);

		for (char rot=0; rot<4; rot++) {
			int cj = 6 - piece._h + 1;
			int ci = 10 - piece._w + 1;
			for (char j=0; j<cj; j++) {
				for (char i=0; i<ci; i++) {
					if (_data[j][i] == figure_count) {
						bool fit = true;
						for (char n=0; n<piece_count; n++) {
							if (_data[piece._data[n][1]+j][piece._data[n][0]+i] != figure_count) {
								fit = false;
								break;
							}
						}
						if (fit) {
							cntx.count++;
							Figure figure(_data);
							for (char n=0; n<piece_count; n++)
								figure._data[piece._data[n][1]+j][piece._data[n][0]+i] = cntx.index;
							cntx.index++;
							bool res = figure.Solve(cntx);
							cntx.index--;
							if (res)
								return true;
						}
					}
				}
			}
			piece.Rotate();
		}

		return false;
	}
};

bool MainWnd::OnPaint(DC* pDC) {
	Pen pen(2);
	Select sp(pDC, pen);

	Brush brushes[figure_count+1];
	for (int i=0; i<=figure_count; i++)
		brushes[i].Create(colors[i]);

	Context cntx{pDC, brushes};

	Figure fig;
	if (fig.Solve(cntx))
		cntx.Draw(cntx.solution);

	return false;
}
