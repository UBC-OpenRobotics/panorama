#include "client/graph_panel.hpp"
#include <algorithm>
#include <cmath>

wxBEGIN_EVENT_TABLE(GraphPanel, wxPanel)
	EVT_PAINT(GraphPanel::OnPaint)
	EVT_SIZE(GraphPanel::OnSize)
wxEND_EVENT_TABLE()

GraphPanel::GraphPanel(wxWindow* parent)
	: wxPanel(parent, wxID_ANY) {
	
	SetBackgroundColour(wxColour(255, 255, 255));
	SetBackgroundStyle(wxBG_STYLE_PAINT); // double buffering so smoother
}

//  redraw the panel
void GraphPanel::OnPaint(wxPaintEvent& event) {
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	DrawBackground(dc);
	DrawGrid(dc);
	DrawAxes(dc);
}

// Function: used when the panel is resized to redraw
void GraphPanel::OnSize(wxSizeEvent& event) {
	Refresh();
	Update();

	event.Skip();
}

// Drawing functions
void GraphPanel::DrawBackground(wxDC& dc) {
	wxSize size = GetClientSize();
	
	dc.SetBrush(wxBrush(wxColour(220, 220, 220))); 

	dc.SetPen(*wxTRANSPARENT_PEN);

	dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
}

// Function: to draw grid lines
void GraphPanel::DrawGrid(wxDC& dc) {
	wxSize size = GetClientSize();

	int leftMargin = 60;
	int bottomMargin = 40; 
	int topMargin = 20;
	int graphHeight = size.GetHeight() - topMargin - bottomMargin;
	int graphWidth = size.GetWidth() - leftMargin - 20;

	dc.SetPen(wxPen(wxColour(90, 90, 90), 1));

	// horizontal grid lines
	for (int i = 0; i <= 4; i++) {
		int y = topMargin + (graphHeight * i / 4);
		dc.DrawLine(leftMargin, y, leftMargin + graphWidth, y);
	}

	// vertical grid lines
	for (int i = 0; i <=6; i++) {
		int x = leftMargin + (graphWidth * i / 6);
		dc.DrawLine(x, topMargin, x, topMargin + graphHeight);
	}
}


void GraphPanel::DrawAxes(wxDC& dc){
	wxSize size = GetClientSize();

	int leftMargin = 60;
	int bottomMargin = 40;
	int topMargin = 20;
	int graphHeight = size.GetHeight() - topMargin - bottomMargin;
	int graphWidth = size.GetWidth() - leftMargin - 20;

	dc.SetPen(wxPen(wxColour(0, 0, 0), 2));

	// draw y-axis
	dc.DrawLine(leftMargin, topMargin, leftMargin, topMargin + graphHeight);

	// draw x-axis
	dc.DrawLine(leftMargin, topMargin + graphHeight, leftMargin + graphWidth, topMargin + graphHeight);

	//
	//x axis label
	for (int i = 0; i < 6; i++) {
		int x = leftMargin + (graphWidth * i / 6);
		int value = i;
		wxString label = wxString::Format("%d", value);
		wxSize textSize = dc.GetTextExtent(label);
		dc.DrawText(label, x - textSize.GetWidth() / 2, size.GetHeight() - bottomMargin + 5);
	}

	//y axis label
	for (int i = 0; i <= 4; i++) {
		int y = (topMargin + graphHeight) - (graphHeight * i / 4);
		int value = i * 25;
		wxString label = wxString::Format("%d", value);
		wxSize textSize = dc.GetTextExtent(label);
		dc.DrawText(label, leftMargin - textSize.GetWidth() - 5, y - textSize.GetHeight() / 2);
	}

}

