#include <wx/wx.h>
#include <wx/timer.h>
#include "client/mainframe.hpp"
#include "client/argparser.hpp"

class PanoramaClient : public wxApp {
public:
    virtual bool OnInit() override {
        
        ArgParser parser(argc, argv); // Parse command line arguments (wxApp provides argc and argv as members)
        
        MainFrame* w = new MainFrame("Panorama Client");
        w->Show();
        
        // If test mode, quit after 3 seconds
        if (parser.isTestMode()) {
            wxTimer* timer = new wxTimer(this);
            
            Bind(wxEVT_TIMER, [this, timer](wxTimerEvent&) {

                timer->Stop();

                wxTheApp->ExitMainLoop(); 
            });
            timer->StartOnce(3000);
        }
        
        return true;
    }
};

wxIMPLEMENT_APP(PanoramaClient);
