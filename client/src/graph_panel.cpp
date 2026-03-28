#include "client/graph_panel.hpp"
#include "config/panorama_config.hpp"

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

	m_plot->SetMargins(config::TOP_MARGIN,config::RIGHT_MARGIN, config::BOTTOM_MARGIN, config::LEFT_MARGIN);
	m_plot->Fit();

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_plot, 1, wxEXPAND);
	SetSizer(sizer);
}

void GraphPanel::AddDataPoint(const std::string& sensorName, double value, double timestamp){
	sensorData_[sensorName].push_back({timestamp, value});

	// FIFO buffer to keep up to NUM_DATA_POINTS samples at a time
	if (sensorData_[sensorName].size() > config::NUM_DATA_POINTS) {
		sensorData_[sensorName].erase(sensorData_[sensorName].begin());
	}
	UpdateGraph();
} 

void GraphPanel::UpdateGraph(){
	for (auto& pair : sensorLayers_){
		m_plot->DelLayer(pair.second,true);
	}
	sensorLayers_.clear(); 

	// colours for the graph
	wxColour colours[] = {
		wxColour(255, 0, 0),		// red 
		wxColour(0, 255, 0), 		// green
		wxColour(0, 0, 255), 		// blue
		wxColour(255, 102, 178),	// pink
		wxColour(178, 102, 255)		// purple
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

		wxPen pen(colours[colourIndex % config::NUM_COLOURS], config::BRUSH_WIDTH);
		layer->SetPen(pen);
		
		m_plot->AddLayer(layer);
		sensorLayers_[sensorName] = layer;

		colourIndex++;
	}

	m_plot->Fit();
	m_plot->Refresh();
	m_plot->Update();
}

void GraphPanel::SetVisibleSensors(const std::set<std::string>& visible) {
	visibleSensors_ = visible; 
	UpdateGraph();
}
