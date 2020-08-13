#include "ExtensionSearcher.h"

#include "wx/dialog.h"
#include "wx/dataview.h"
#include "wx/filepicker.h"

class DirChoiceDialog : public wxDialog
{
public:
    DirChoiceDialog();

    void on_ok(wxCommandEvent & evt);
    void on_cancel(wxCommandEvent & evt);

private:
    void init_layout();

    void update_experiments_list(wxFileDirPickerEvent & evt);

    void on_size_update(wxSizeEvent & evt);

private:
    wxDataViewListCtrl * m_experiments_list;
    wxBoxSizer * m_main_sizer;
    ExtensionSearcherRef m_searcher;
};