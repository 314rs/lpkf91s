#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <boost/asio.hpp>
#include <wx/filepicker.h>

using std::string;
namespace asio = boost::asio;


class MyApp : public wxApp
{
public:
    bool OnInit() override;
private:
    asio::io_context ioc;
    asio::serial_port* port = nullptr;
};
 
wxIMPLEMENT_APP(MyApp);
 
class MyFrame : public wxFrame
{
public:
    MyFrame(asio::io_context& ioc, asio::serial_port* port = nullptr);
 
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnCommands(wxCommandEvent& event);
    asio::serial_port* port = nullptr;
    asio::io_context& ioc;

};

class FrameCommands : public wxFrame
{
public:
    FrameCommands(wxWindow *parent, asio::io_context& ioc, asio::serial_port* port = nullptr);
 
private:
    void OnConnect(wxCommandEvent& event);
    asio::serial_port* port = nullptr;
    asio::io_context& ioc;
};

class SerialMenu : public wxMenu 
{
public:
    SerialMenu(asio::io_context& ioc, asio::serial_port* port);
private:
    asio::io_context& ioc;   
    asio::serial_port* port = nullptr;
};

 
enum
{
    ID_Hello = 1,
    ID_Connect,
    ID_Commands
};
 
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(ioc, port);
    frame->Show(true);
    return true;
}
 
MyFrame::MyFrame(asio::io_context& ioc, asio::serial_port* port) : wxFrame(nullptr, wxID_ANY, "LPKF91s Controller"), port(port), ioc(ioc)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
    menuFile->Append(wxID_PREFERENCES, "&Preferences\tCtrl-,", "Settings");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "&Quit\tCtrl-Q", "Exit program");
    menuFile->Append(ID_Connect, "Connect", "Connect to LPKF91s");
 
    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append(wxID_EDIT);

    wxMenu *menuTools = new wxMenu;
    menuTools->Append(wxID_ANY, "Commands", "send commands to LPKF91s manually");

    wxMenu *menuOptions = new wxMenu;
    menuOptions->Append(wxID_SETUP);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenu *menuSerial = new SerialMenu(ioc, port);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuEdit, "&Edit");
    menuBar->Append(menuSerial, "&Serial");
    menuBar->Append(menuTools, "&Tools");
    menuBar->Append(menuOptions, "&Options");
    menuBar->Append(menuHelp, "&Help");

 
    SetMenuBar(menuBar);
 
    CreateStatusBar(3);
    SetStatusText("not connected", 2);
    SetStatusText("Welcome to LPKF91s Controller!");
 
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnConnect, this, ID_Connect);
    Bind(wxEVT_MENU, &MyFrame::OnCommands, this, ID_Commands);

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, -1, "Send a HPGL file"), 0, wxEXPAND, 10);
    sizer->Add(new wxFilePickerCtrl(this, wxID_ANY, "File to send"), 0, wxEXPAND, 10);
    sizer->Add(new wxButton(this, wxID_ANY, "Send"), 0, wxEXPAND, 10);



    wxGridSizer *arrows_sizer = new wxGridSizer(3, 3, wxSize(10, 10));
    wxButton *up = new wxButton(this, wxID_ANY, "up");
    wxButton *left = new wxButton(this, wxID_ANY, "left");
    wxButton *right = new wxButton(this, wxID_ANY, "right");
    wxButton *down = new wxButton(this, wxID_ANY, "down");
    arrows_sizer->Add(new wxStaticText(this, -1, wxT("")), 0, wxEXPAND);
    arrows_sizer->Add(up, 0, wxEXPAND);
    arrows_sizer->Add(new wxStaticText(this, -1, wxT("")), 0, wxEXPAND);
    arrows_sizer->Add(left, 0, wxEXPAND);
    arrows_sizer->Add(new wxStaticText(this, -1, wxT("")), 0, wxEXPAND);
    arrows_sizer->Add(right, 0, wxEXPAND);
    arrows_sizer->Add(new wxStaticText(this, -1, wxT("")), 0, wxEXPAND);
    arrows_sizer->Add(down, 0, wxEXPAND);
    arrows_sizer->Add(new wxStaticText(this, -1, wxT("")), 0, wxEXPAND);

    sizer->Add(arrows_sizer, 0, wxEXPAND | wxALL, 10);

    SetSizer(sizer);

    Center();

    // SetIcon(wxIcon("../media/lpkf91s_controller_logo.png", wxBITMAP_TYPE_PNG));
    

}
 
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
 
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This application is developed by Lars Bornträger to control the LPKF91s PCB mill at the TUHH WorkINGLab",
                 "About LPKF91s Controller", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnConnect(wxCommandEvent &event)
{
    wxString user_port = wxEmptyString;
    wxTextEntryDialog dialog(this, wxT("Enter serial port name"), wxT("Connect"), user_port);
    dialog.ShowModal();
    user_port = dialog.GetValue();
    std::cout << user_port << std::endl;
    SetStatusText("connecting to " + user_port.ToStdString(), 2);
    try {
        if (this->port == nullptr) {
            this->port = new asio::serial_port{this->ioc, user_port.ToStdString()};
            this->port->set_option(asio::serial_port_base::baud_rate{9600});
            this->port->set_option(asio::serial_port_base::stop_bits{asio::serial_port_base::stop_bits::one});
            this->port->set_option(asio::serial_port_base::character_size{8});
            this->port->set_option(asio::serial_port_base::parity{asio::serial_port_base::parity::none});
            this->port->set_option(asio::serial_port_base::flow_control{asio::serial_port_base::flow_control::hardware});
            SetStatusText("connected to " + user_port.ToStdString(), 2);
            this->port->write_some(asio::buffer("!CT1;\n"));
            this->port->write_some(asio::buffer("IN;\n"));

        } else {
            SetStatusText("Already connected", 2);
        }
    } catch (const std::exception &e) {
        SetStatusText(e.what(), 2);
    }
    /// @todo boost asio serial connect
}

void MyFrame::OnCommands(wxCommandEvent &event)
{
    FrameCommands *frame = new FrameCommands(nullptr, ioc, port);
    frame->Show(true);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

FrameCommands::FrameCommands(wxWindow *parent, asio::io_context &ioc, asio::serial_port *port) : 
    wxFrame(parent, wxID_ANY, "Commands"), 
    ioc(ioc), 
    port(port)
{
    wxButton* btn_init = new wxButton(this, wxID_ANY, "Init");
    Center();
}



SerialMenu::SerialMenu(asio::io_context& ioc, asio::serial_port* port) : 
    wxMenu(),
    ioc(ioc),
    port(port)
{   
    Append(wxID_ANY, "port");
    Append(wxID_ANY, "baud rate");

    auto subm_stopbits = new wxMenu();
    subm_stopbits->AppendRadioItem(wxID_ANY, "one");
    subm_stopbits->AppendRadioItem(wxID_ANY, "one point five");
    subm_stopbits->AppendRadioItem(wxID_ANY, "two");
    AppendSubMenu(subm_stopbits, "stop bits");

    auto subm_charsize = new wxMenu();
    subm_charsize->AppendRadioItem(wxID_ANY, "5");
    subm_charsize->AppendRadioItem(wxID_ANY, "6");
    subm_charsize->AppendRadioItem(wxID_ANY, "7");
    subm_charsize->AppendRadioItem(wxID_ANY, "8");
    AppendSubMenu(subm_charsize, "character size");

    auto subm_parity = new wxMenu();
    subm_parity->AppendRadioItem(wxID_ANY, "none");
    subm_parity->AppendRadioItem(wxID_ANY, "odd");
    subm_parity->AppendRadioItem(wxID_ANY, "even");
    AppendSubMenu(subm_parity, "parity");

    auto subm_flow = new wxMenu();
    subm_flow->AppendRadioItem(wxID_ANY, "none");
    subm_flow->AppendRadioItem(wxID_ANY, "hardware");
    subm_flow->AppendRadioItem(wxID_ANY, "software");
    AppendSubMenu(subm_flow, "flow control");

    AppendSeparator();
    Append(wxID_ANY, "connect");

    if (port == nullptr) {
        this->port = new asio::serial_port{ioc, "COM4"};
        this->port->set_option(asio::serial_port_base::baud_rate{9600});
        this->port->set_option(asio::serial_port_base::stop_bits{asio::serial_port_base::stop_bits::one});
        this->port->set_option(asio::serial_port_base::character_size{8});
        this->port->set_option(asio::serial_port_base::parity{asio::serial_port_base::parity::none});
        this->port->set_option(asio::serial_port_base::flow_control{asio::serial_port_base::flow_control::hardware});
    }
}
