
#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/window.h>
#include <vector>

class GraphPanel : public wxPanel {
public:
    GraphPanel(wxWindow* parent);

private:
    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

    // Drawing functions
    void DrawBackground(wxDC& dc);
    void DrawGrid(wxDC& dc);
    void DrawAxes(wxDC& dc);

    wxDECLARE_EVENT_TABLE();
};