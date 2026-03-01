#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/dirdlg.h>
#include <wx/stdpaths.h>
#include <memory>
#include <filesystem>
#include "client/mainframe.hpp"
#include "client/argparser.hpp"
#include "client/message_model.hpp"
#include "client/tcp_client.hpp"
#include "client/DataBuffer.hpp"
#include "client/json_reader.hpp"
#include "client/config_manager.hpp"
#include "client/data_logger.hpp"
#include "client/command_processor.hpp"
#include <iostream>
using namespace std;

class PanoramaClient : public wxApp {
public:
    virtual bool OnInit() override {
        ArgParser parser(argc, argv);

        // --- Runtime Directory Selection ---
        ConfigManager& config = ConfigManager::getInstance();

        // Check if runtime directory is already set from previous launch
        std::string runtimeDir = config.getConfig("runtime_directory");

        if (runtimeDir.empty()) {
            // TODO: To be implemented later:
            // if (!parser.isNoGuiMode()) {
            //     // GUI mode: Show directory selection dialog
            //     wxDirDialog dirDialog(nullptr, "Select Panorama Runtime Directory",
            //                          wxStandardPaths::Get().GetDocumentsDir(),
            //                          wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

            //     if (dirDialog.ShowModal() == wxID_CANCEL) {
            //         wxMessageBox("Runtime directory is required to start Panorama.",
            //                     "Directory Required", wxOK | wxICON_ERROR);
            //         return false;
            //     }

            //     runtimeDir = dirDialog.GetPath().ToStdString();
            // } else {
            //     // This is for testing (No-GUI mode): Use command-line argument or default
            //     runtimeDir = parser.getRuntimeDirectory();
            //     if (runtimeDir.empty()) {
            //         // Use default directory
            //         wxString homeDir = wxStandardPaths::Get().GetDocumentsDir();
            //         runtimeDir = homeDir.ToStdString() + "/panorama_data";

            //         // Create default directory if it doesn't exist
            //         try {
            //             std::filesystem::create_directories(runtimeDir);
            //         } catch (const std::exception& e) {
            //             std::cerr << "Failed to create default runtime directory: " << runtimeDir << std::endl;
            //             std::cerr << "Error: " << e.what() << std::endl;
            //             return false;
            //         }
            //     }

            //     std::cout << "Using runtime directory: " << runtimeDir << std::endl;
            // }
            
            // ASSUME panorama/rundir FOR NOW
            runtimeDir = parser.getRuntimeDirectory();
            if (runtimeDir.empty()) {

                // wxString homeDir = wxStandardPaths::Get().GetDocumentsDir();
                // runtimeDir = homeDir.ToStdString() + "/rundir"; // <=== THIS IS HARDCODED FOR NOW
                std::filesystem::path sourceDir = std::filesystem::path(__FILE__).parent_path();
                runtimeDir = (sourceDir / ".." / ".." / "rundir").lexically_normal().string();

                try {
                    std::filesystem::create_directories(runtimeDir);
                } catch (const std::exception& e) {
                    std::cerr << "Failed to create default runtime directory: " << runtimeDir << std::endl;
                    std::cerr << "Error: " << e.what() << std::endl;
                    return false;
                }
            }

            std::cout << "Using runtime directory: " << runtimeDir << std::endl;

            // Set runtime directory in ConfigManager
            if (!config.setRuntimeDirectory(runtimeDir)) {
                if (!parser.isNoGuiMode()) {
                    wxMessageBox("Failed to initialize runtime directory.",
                                "Error", wxOK | wxICON_ERROR);
                } else {
                    std::cerr << "Failed to initialize runtime directory: " << runtimeDir << std::endl;
                }
                return false;
            }

            // Init config
            if (!config.initializeConfig()) {
                if (!parser.isNoGuiMode()) {
                    wxMessageBox("Failed to initialize configuration.",
                                "Error", wxOK | wxICON_ERROR);
                } else {
                    std::cerr << "Failed to initialize configuration" << std::endl;
                }
                return false;
            }
        } else {
            // Runtime directory was saved, use it
            if (!config.setRuntimeDirectory(runtimeDir)) {
                // Saved directory is no longer valid, clear it and restart
                config.saveConfig("runtime_directory", "");
                if (!parser.isNoGuiMode()) {
                    wxMessageBox("Saved runtime directory is no longer accessible. Please select a new directory.",
                                "Directory Not Accessible", wxOK | wxICON_WARNING);
                }
                return false;
            }

            // Open existing config
            if (!config.initializeConfig()) {
                std::cerr << "Failed to open configuration" << std::endl;
                return false;
            }
        }

        // --- Create model (shared between view and controller) ---
        model_ = std::make_shared<MessageModel>();

        // --- Create DataLogger ---
        dataLogger_ = std::make_shared<DataLogger>(config.getDataLogPath());
        if (!dataLogger_->isOpen()) {
            std::cerr << "Warning: Data logger failed to initialize. Data will not be persisted." << std::endl;
        }

        // --- Create DataBuffer ---
        dataBuffer_ = std::make_shared<DataBuffer>(runtimeDir + "/data");
        

        // --- Create and start TCP client on separate thread ---
        tcpClient_ = std::make_unique<TcpClient>("127.0.0.1", 3000, model_, dataBuffer_, dataLogger_);
        tcpClient_->start();

        // For running without a gui
        if (parser.isNoGuiMode()) {
            cout << "Running in console mode (no GUI). Press Ctrl+C to exit.\n";
            cout << "Listening for JSON stream...\n\n";

            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Run forever in console
            }
            return true;
        }

        // --- Create CommandProcessor on a separate thread---
        cmdProcessor_ = std::make_shared<CommandProcessor>(dataBuffer_);
        cmdThread_ = std::make_unique<std::thread>(&CommandProcessor::start, cmdProcessor_);

        // --- Create view ---
        MainFrame* w = new MainFrame("Panorama Client", model_, dataBuffer_);
        w->Show();

 
        // --- If test mode, quit after 3 seconds ---
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

    virtual int OnExit() override {
        // Clean shutdown of TCP client
        if (tcpClient_) {
            tcpClient_->stop();
        }

        if (cmdProcessor_) {
            cmdProcessor_->stop();
        }

        if (cmdThread_ && cmdThread_->joinable()) {
            cmdThread_->join();
        }

        return wxApp::OnExit();
    }

private:
    std::shared_ptr<MessageModel> model_;
    std::shared_ptr<DataLogger> dataLogger_;
    std::shared_ptr<DataBuffer> dataBuffer_;
    std::unique_ptr<TcpClient> tcpClient_;
    std::shared_ptr<CommandProcessor> cmdProcessor_;
    std::unique_ptr<std::thread> cmdThread_;
};

wxIMPLEMENT_APP(PanoramaClient);
