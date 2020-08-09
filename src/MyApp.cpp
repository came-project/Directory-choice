#include "MyApp.h"

wxIMPLEMENT_APP(MyApp); // NOLINT

bool MyApp::OnInit()
{
    m_dc_dialog = new DirChoiceDialog();
    m_dc_dialog->Show(true);

    return true;
}
