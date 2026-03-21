#include "client/graph_panel.hpp"

GraphPanel::GraphPanel(wxWindow* parent)
	: wxPanel(parent, wxID_ANY) {
	
	m_plot = new mpWindow(this, wxID_ANY);
	m_plot-> EnableDoubleBuffer(true);
	m_plot-> EnableMousePanZoom(true);

	// Create X & Y axes
	mpScaleX* xAxis = new mpScaleX(wxT("Time (seconds)"), mpALIGN_BORDER_BOTTOM, true);
	mpScaleY* yAxis = new mpScaleY(wxT("y"), mpALIGN_BORDER_LEFT, true);

	xAxis->SetDrawOutsideMargins(false);
	yAxis->SetDrawOutsideMargins(false);

	m_plot->AddLayer(yAxis);
	m_plot->AddLayer(xAxis);

	m_plot->SetMargins(30, 30, 85, 60);
	m_plot->Fit();

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_plot, 1, wxEXPAND);
	SetSizer(sizer);
}
