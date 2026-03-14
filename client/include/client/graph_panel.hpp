#pragma once
#include <wx/wx.h>
#include <mathplot.h>

class GraphPanel : public wxPanel {
public:
    GraphPanel(wxWindow* parent);

private:
    mpWindow* m_plot;
};