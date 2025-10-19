#include "client/mainframe.hpp"
#include <wx/stattext.h>

MainFrame::MainFrame(
    const wxString& title, 
    const wxPoint& pos, 
    const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size) {
    
    auto *label = new wxStaticText(this, wxID_ANY, "Panorama Client");
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddStretchSpacer();
    sizer->Add(label, 0, wxALIGN_CENTER);
    sizer->AddStretchSpacer();
    SetSizer(sizer);
}