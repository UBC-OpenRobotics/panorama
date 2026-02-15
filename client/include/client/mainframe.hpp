#ifndef __MAINFRAME__
#define __MAINFRAME__
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/timer.h>
#include <wx/socket.h>
#include <vector>
#include <memory>
#include <map>
#include "client/sensor_data_panel.h"


class MessageModel;
class DataBuffer;
//class GraphPanel;
class SensorDataManager;
//class SensorDataFrame;  // Add this

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
        std::shared_ptr<DataBuffer> dataBuffer,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(1200, 800));

private:
    void updateMessageDisplay();
    void updateDataPanel();
    SensorDataFrame* sensorDataGrid;

    std::shared_ptr<MessageModel> model_;
    std::shared_ptr<DataBuffer> dataBuffer_;
    wxTextCtrl* messageDisplay_;

};

#endif // __MAINFRAME__