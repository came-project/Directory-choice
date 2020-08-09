#pragma once

#include "wx/app.h"

#include "DirChoiceDialog.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;

private:
    DirChoiceDialog * m_dc_dialog;
};