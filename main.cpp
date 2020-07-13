// Jonathan Claire
// Version 1.0
// 5/20/20

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

using namespace std;
#include <vector>
#include <algorithm>
#include "board.h"


//----------- Edit This -----------
#include "player.h" // to your ai class
#include "RachAI.h"
typedef RachAI P1;  // same here
typedef RachAI P2;

bool p1IsHuman = true;
bool p2IsHuman = false;
//----------------------------------

bool p1Turn = true;
Move lastMove = Move(0, 0, 0, 0);
P1 p1;
P2 p2;


class MyFrame : public wxFrame {
public:
    MyFrame();
    void SetBoard(Board b);

private:
    void OnPaint(wxPaintEvent& event);
    void SelectPiece(wxMouseEvent& event);
    void DropPiece(wxMouseEvent& event);
    void OnIdle(wxIdleEvent& event);

    wxPanel* m_board;
    wxBitmap board_bitmap;
    wxBitmap sprites;
    Board board;

    int selectx;
    int selecty;
};

MyFrame::MyFrame()
    :wxFrame(NULL, wxID_ANY, "Chess", wxDefaultPosition, wxSize(528, 552))
{
    m_board = new wxPanel(this, wxID_ANY);
    m_board->Bind(wxEVT_PAINT, &MyFrame::OnPaint, this);
    m_board->Bind(wxEVT_LEFT_DOWN, &MyFrame::SelectPiece, this);
    m_board->Bind(wxEVT_LEFT_UP, &MyFrame::DropPiece, this);

    Bind(wxEVT_IDLE, &MyFrame::OnIdle, this);

    board_bitmap = wxBitmap(wxImage("woodchessboard.png", wxBITMAP_TYPE_PNG));
    sprites = wxBitmap(wxImage("chesspieces.png", wxBITMAP_TYPE_PNG));

    p1.Init(true);
    p2.Init(false);
}

void MyFrame::SetBoard(Board b) {
    board = b;
}

void MyFrame::OnPaint(wxPaintEvent&) {
    wxPaintDC dc(m_board);
    dc.Clear();

    dc.DrawBitmap(board_bitmap, 0, 0, true);

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int space = board.getPiece(x, y);
            if (space != 0) {
                int piece = space % 8;
                int team = space / 8;
                    dc.DrawBitmap(sprites.GetSubBitmap(wxRect((piece - 1) * 64, !team * 64, 64, 64)), x * 64, 448 - y * 64, true);
            }
        }
    }
}

void MyFrame::SelectPiece(wxMouseEvent& event) {
    if ((p1Turn && p1IsHuman) || (!p1Turn && p2IsHuman)) {
        selectx = event.GetX() / 64;
        selecty = 7 - event.GetY() / 64;
    }
}

void MyFrame::DropPiece(wxMouseEvent& event) {
    if ((p1Turn && p1IsHuman) || (!p1Turn && p2IsHuman)) {
        int destx = event.GetX() / 64;
        int desty = 7 - event.GetY() / 64;
        lastMove = Move::Move(selectx, selecty, destx, desty);
        board.makeMove(lastMove);
        p1Turn = !p1Turn;
        wxWindow::Refresh();
    }
}

void MyFrame::OnIdle(wxIdleEvent& event) {
    if (p1Turn && !p1IsHuman) {
        lastMove = p1.GetMove(lastMove);
        board.makeMove(lastMove);
        p1Turn = !p1Turn;
        wxWindow::Refresh();

        bool whiteKing = false;
        bool blackKing = false;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (board.getPiece(i, j) == W + K)
                    whiteKing = true;
                else if (board.getPiece(i, j) == B + K)
                    blackKing = true;
        if (!(whiteKing && blackKing))
            wxMessageBox("Game over", "Endgame");
    }
    else if (!p1Turn && !p2IsHuman) {
        lastMove = p2.GetMove(lastMove);
        board.makeMove(lastMove);
        p1Turn = !p1Turn;
        wxWindow::Refresh();
    }

}


class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        ::wxInitAllImageHandlers();
        frame = new MyFrame();
        frame->Show();
        return true;
    }
private:
    MyFrame* frame;
};

wxIMPLEMENT_APP(MyApp);