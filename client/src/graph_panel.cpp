#include "client/graph_panel.hpp"
#include "common/panorama_colours.hpp"
#include <wx/dcbuffer.h>
#include <algorithm>
#include <cmath>

wxBEGIN_EVENT_TABLE(GraphPanel, wxPanel)
	EVT_PAINT(GraphPanel::OnPaint)
	EVT_SIZE(GraphPanel::OnSize)
wxEND_EVENT_TABLE()

GraphPanel::GraphPanel(wxWindow* parent)
	: wxPanel(parent, wxID_ANY) {
	
	m_plot = new mpWindow(this, wxID_ANY);
	m_plot-> EnableDoubleBuffer(true);
	m_plot-> EnableMousePanZoom(true);

	// Create X & Y axes
	mpScaleX* xAxis = new mpScaleX(wxT("Time (seconds)"), mpALIGN_BORDER_BOTTOM, true);
	mpScaleY* yAxis = new mpScaleY(wxT("value"), mpALIGN_BORDER_LEFT, true);

	xAxis->SetDrawOutsideMargins(false);
	yAxis->SetDrawOutsideMargins(false);

	m_plot->AddLayer(yAxis);
	m_plot->AddLayer(xAxis);

	m_plot->SetMargins(30, 30, 85, 60);
	m_plot->Fit();

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_plot, 1, wxEXPAND);
	SetSizer(sizer);
	SetBackgroundColour(PCOLOUR_WHITE);
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
	
	dc.SetBrush(wxBrush(PCOLOUR_LIGHT_GREY)); 

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

	dc.SetPen(wxPen(PCOLOUR_GREY, 1));

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

	dc.SetPen(wxPen(PCOLOUR_BLACK, 2));

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

void GraphPanel::AddDataPoint(const std::string& sensorName, double value, double timestamp, bool refresh){
	sensorData_[sensorName].push_back({timestamp, value});

	// Keeps the first 100 data points
	if (sensorData_[sensorName].size() > 100) {
		sensorData_[sensorName].erase(sensorData_[sensorName].begin());
	}

	if (refresh) {
		UpdateGraph();
	}
}

void GraphPanel::UpdateGraph(){
	for (auto& pair : sensorLayers_){
		m_plot->DelLayer(pair.second,true);
	}
	sensorLayers_.clear(); 

	// colours for the graph
	wxColour colours[] = {
		wxColour(255, 0, 0), 
		wxColour(0, 255, 0), 
		wxColour(0, 0, 255), 
		wxColour(255, 102, 178),
		wxColour(178, 102, 255)
	};
	int colourIndex = 0; 

	for(auto& sensorPair : sensorData_){
		const std::string& sensorName = sensorPair.first;
		auto& data = sensorPair.second;

		if(!visibleSensors_.empty() && visibleSensors_.count(sensorName) == 0)
			continue;
		
		if (data.empty()) continue; 

		std::vector<double> xs, ys;
		for(const auto& point : data){
			xs.push_back(point.first); // timestamp
			ys.push_back(point.second); // value
		}

		mpFXYVector* layer = new mpFXYVector(wxString(sensorName));
		layer->SetData(xs, ys);
		layer->SetContinuity(true); 

		wxPen pen(colours[colourIndex % 5], 2);
		layer->SetPen(pen);
		
		m_plot->AddLayer(layer);
		sensorLayers_[sensorName] = layer;

		colourIndex++;
	}

	m_plot->Fit();
	m_plot->Refresh();
	m_plot->Update();
}

void GraphPanel::SetVisibleSensors(const std::set<std::string>& visible, bool refresh) {
	visibleSensors_ = visible;
	if (refresh) {
		UpdateGraph();
	}
}
