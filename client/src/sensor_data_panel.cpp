#include "client/sensor_data_panel.h"
#include <wx/datetime.h>

SensorDataFrame::SensorDataFrame(wxWindow* parent, const wxArrayString& sensorNames)
    : wxPanel(parent, wxID_ANY), 
      sensorNames_(sensorNames) {
    
    // Create grid
    grid_ = new wxGrid(this, wxID_ANY);
    grid_->CreateGrid(sensorNames.GetCount(), 3);
    
    // Hide row labels to maximize space for data
    grid_->SetRowLabelSize(0);
    
    grid_->SetColLabelValue(0, "Sensor");
    grid_->SetColLabelValue(1, "Value");
    grid_->SetColLabelValue(2, "Last Updated");
 
    // Make cells read-only
    grid_->EnableEditing(false);
 
    // Disable column and row resizing
    grid_->EnableDragColSize(false);  
    grid_->EnableDragRowSize(false);  
    grid_->EnableDragGridSize(false); 
 
    wxFont gridFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    grid_->SetDefaultCellFont(gridFont);
    
    InitializeGrid();
    
    // Use sizer to make grid fill the entire panel
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(grid_, 1, wxEXPAND);
    SetSizer(sizer);
    
    // Bind size event to adjust columns dynamically
    Bind(wxEVT_SIZE, &SensorDataFrame::OnSize, this);
    
    // Trigger initial column sizing after a short delay
    CallAfter([this]() {
      wxSizeEvent evt;
      OnSize(evt);
    });
}

void SensorDataFrame::OnSize(wxSizeEvent& event) {
    event.Skip();  // Let default handler process first
    
    if (!grid_) return;
    
    // Get available width from the panel
    int totalWidth = GetClientSize().GetWidth();
    
    // Account for row labels
    int rowLabelWidth = grid_->GetRowLabelSize();
    
    // Calculate available width (subtract row label, leave small margin for borders)
    int availableWidth = totalWidth - rowLabelWidth - 5;  // Reduced margin from 20 to 5
    
    if (availableWidth < 100) return;
    
    // Distribute width proportionally to fill entire space: 40%, 25%, 35%
    int col0Width = static_cast<int>(availableWidth * 0.40);
    int col1Width = static_cast<int>(availableWidth * 0.25);
    int col2Width = availableWidth - col0Width - col1Width;  // Use remaining space to avoid gaps
    
    grid_->SetColSize(0, col0Width);
    grid_->SetColSize(1, col1Width);
    grid_->SetColSize(2, col2Width);
    
    grid_->ForceRefresh();
}

void SensorDataFrame::InitializeGrid() {
    for (size_t i = 0; i < sensorNames_.GetCount(); ++i) {
        grid_->SetCellValue(i, 0, sensorNames_[i]);
        grid_->SetCellValue(i, 1, "--");
        grid_->SetCellValue(i, 2, "No data");
    
        //align cells and sh
        grid_->SetCellAlignment(i, 1, wxALIGN_CENTER, wxALIGN_CENTER);
        grid_->SetCellAlignment(i, 2, wxALIGN_CENTER, wxALIGN_CENTER);
    }
}

void SensorDataFrame::UpdateReading(const std::string& sensorName, double value, const std::string& unit) {
    int row = FindSensorRow(sensorName);
    if (row == -1) return;
    
    
    wxString valueStr = wxString::Format("%.2f %s", value, unit);
    grid_->SetCellValue(row, 1, valueStr);
    
    
    wxString timestamp = wxDateTime::Now().FormatISOTime();
    grid_->SetCellValue(row, 2, timestamp);

    std::cout << "Updated " << sensorName << " with value: " << valueStr.ToStdString() << " at " << timestamp.ToStdString() << std::endl;
    
    // Color 
    if (value > 50.0) {
        grid_->SetCellBackgroundColour(row, 1, wxColour(255, 200, 200)); // Light red
    } else if (value > 25.0) {
        grid_->SetCellBackgroundColour(row, 1, wxColour(255, 255, 200)); // Light yellow
    } else {
        grid_->SetCellBackgroundColour(row, 1, wxColour(200, 255, 200)); // Light green
    }
    
    grid_->ForceRefresh();
}

void SensorDataFrame::ClearReadings() {
    InitializeGrid();
}

void SensorDataFrame::SetActiveSensors(const std::vector<std::string>& sensors) {
    for (int i = 0; i < grid_->GetNumberRows(); ++i) {
        wxString sensorName = grid_->GetCellValue(i, 0);
        bool isActive = std::find(sensors.begin(), sensors.end(), 
            sensorName.ToStdString()) != sensors.end();

    if (!isActive) {
            grid_->SetCellTextColour(i, 0, *wxLIGHT_GREY);
        grid_->SetCellTextColour(i, 1, *wxLIGHT_GREY);
            grid_->SetCellTextColour(i, 2, *wxLIGHT_GREY);
    } else {
      grid_->SetCellTextColour(i, 0, *wxBLACK);
            grid_->SetCellTextColour(i, 1, *wxBLACK);
          grid_->SetCellTextColour(i, 2, *wxBLACK);
     }
    }
    grid_->ForceRefresh();
}

int SensorDataFrame::FindSensorRow(const std::string& sensorName) {
    for (int i = 0; i < grid_->GetNumberRows(); ++i) {
   if (grid_->GetCellValue(i, 0).ToStdString() == sensorName) {
   return i;
        }
    }
    return -1;
}
