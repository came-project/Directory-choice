#include "DirChoiceDialog.h"

#include <iterator>

#include "ExtensionSearcher.h"
#include "wx/button.h"

enum
{
    id_dialog = 0,
    id_dir_picker = 1,
    id_ok = 2,
    id_cancel = 3,
};

DirChoiceDialog::DirChoiceDialog()
    : wxDialog(nullptr, id_dialog, "Pick directory", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    this->init_layout();

    Bind(wxEVT_DIRPICKER_CHANGED, &DirChoiceDialog::update_experiments_list, this, id_dir_picker);
    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent & evt) { this->Destroy(); });
    Bind(wxEVT_BUTTON, &DirChoiceDialog::on_ok, this, id_ok);
    Bind(wxEVT_BUTTON, &DirChoiceDialog::on_cancel, this, id_cancel);
    Bind(wxEVT_SIZE, &DirChoiceDialog::on_size_update, this, id_dialog);
}

void DirChoiceDialog::init_layout()
{
    m_main_sizer = new wxBoxSizer(wxVERTICAL);

    m_experiments_list = new wxDataViewListCtrl(this, wxID_ANY);
    m_experiments_list->AppendTextColumn("Experiment name");
    m_experiments_list->AppendTextColumn("Images count")->SetWidth(100);

    m_main_sizer->Add(m_experiments_list, 1, wxEXPAND | wxALL);
    m_main_sizer->SetMinSize(640, 480);
    m_main_sizer->AddSpacer(1);

    auto buttons = new wxBoxSizer(wxHORIZONTAL);
    buttons->SetMinSize(640, 30);

    auto * dir_picker = new wxDirPickerCtrl(this, id_dir_picker, "", "&Chose directory where experiments folders are contained");

    buttons->Add(dir_picker, 3, wxEXPAND | wxLEFT | wxRIGHT);
    buttons->AddStretchSpacer(1);

    auto * ok_button = new wxButton(this, id_ok, "&Ok");
    auto * cancel_button = new wxButton(this, id_cancel, "&Cancel");
    auto max_button_size = wxSize(120, 30);
    ok_button->SetMaxSize(max_button_size);
    cancel_button->SetMaxSize(max_button_size);

    buttons->Add(ok_button, 1, wxLEFT | wxRIGHT);
    buttons->Add(cancel_button, 1, wxLEFT | wxRIGHT);

    m_main_sizer->Add(buttons, 0, wxLEFT | wxRIGHT | wxEXPAND);

    this->SetSizerAndFit(m_main_sizer);
}

void DirChoiceDialog::update_experiments_list(wxFileDirPickerEvent & evt)
{
    std::cout << "update experiments\n";
    std::basic_string<wchar_t> path(evt.GetPath());
    std::cout << "path string got\n";
    m_searcher = ExtensionSearcherRef::search_in_dir(std::filesystem::path(path), ".cpp", ".h");
    std::cout << "search ended\n";

    m_experiments_list->DeleteAllItems();
    wxVector<wxVariant> item;
    item.resize(2);

    const auto & experiments = m_searcher.names();
    if (experiments.empty()) {
        item[0] = "(No experiments found in the directory)";
        item[1] = "";
        m_experiments_list->AppendItem(item);
        return;
    }
    for (const auto & exp_name : experiments) {
        item[0] = exp_name;
        item[1] = std::to_string(m_searcher[exp_name].cnt);
        m_experiments_list->AppendItem(item);
    }
}

void DirChoiceDialog::on_ok(wxCommandEvent & evt)
{
    int selected = m_experiments_list->GetSelectedRow();
    if (selected != wxNOT_FOUND) {
        namespace fs = std::filesystem;
        std::cout << "Chosen dir: ";
        std::cout << m_searcher[selected].entry.path() << '\n';
        std::filesystem::directory_iterator dir_iter(m_searcher[m_searcher.names()[selected]].entry);
        for (const auto & sub_element : dir_iter) {
            if (sub_element.is_regular_file() && m_searcher.has_extension(sub_element.path().extension())) {
                std::cout << sub_element.path().string() << '\n';
            }
        }
        this->Destroy();
    }
}

void DirChoiceDialog::on_cancel(wxCommandEvent & evt)
{
    if (evt.GetId() == id_cancel) {
        this->Destroy();
    }
}
void DirChoiceDialog::on_size_update(wxSizeEvent & evt)
{
    this->Layout();
    m_experiments_list->GetColumn(0)->SetWidth(m_experiments_list->GetSize().x - 100);
}
