#include "client/graph_panel.hpp"

GraphPanel::GraphPanel(wxWindow* parent, std::shared_ptr<PostProcessing> postProcessor)
	: wxPanel(parent, wxID_ANY), postProcessor_(postProcessor) {
	
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
}

void GraphPanel::AddDataPoint(const std::string& sensorName, double value, double timestamp){
	sensorData_[sensorName].push_back({timestamp, value});

	// Keeps the first 100 data points
	if (sensorData_[sensorName].size() > 100) {
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

			//add necessary offset and scaling to the data point before plotting
			auto value = point.second;
			value = postProcessor_->processData(value);
			ys.push_back(value);
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

void GraphPanel::SetVisibleSensors(const std::set<std::string>& visible) {
	visibleSensors_ = visible; 
	UpdateGraph();
}
