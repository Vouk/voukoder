#include "wxVoukoderDialog.h"
#include <wx/clipbrd.h>
#include <wx/msw/registry.h>
#include "EncoderUtils.h"
#include "LanguageUtils.h"
#include "PluginUpdate.h"
#include "Images.h"
#include "Voukoder.h"
#include "Log.h"
#include "wxConfigurationDialog.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxVoukoderDialog::wxVoukoderDialog(wxWindow *parent, ExportInfo &exportInfo) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(600, 640), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	exportInfo(exportInfo)
{
	SetTitle(VKDR_APPNAME);
	SetSizeHints(wxDefaultSize, wxDefaultSize);

	// Translate boolean options
	wxPGGlobalVars->m_boolChoices[0].SetText(Trans("ui.encoderconfig.false"));
	wxPGGlobalVars->m_boolChoices[1].SetText(Trans("ui.encoderconfig.true"));

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);

	// Init image handlers
	wxInitAllImageHandlers();

	// Categories
	wxImageList* m_listbook1Images = new wxImageList(48, 48);
	{
		wxLogNull logNo;
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_HOME));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_VIDEO));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_AUDIO));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_SETTINGS));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_UPDATE));
	}
	
	size_t imageIdx = 0;

	m_Categories = new wxListbook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT);
	m_Categories->AssignImageList(m_listbook1Images);

	// Categories > General

	m_genPanel = new wxPanel(m_Categories, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bGenSizer;
	bGenSizer = new wxBoxSizer(wxVERTICAL);

	m_genNotebook = new wxNotebook(m_genPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_genTab1 = new wxPanel(m_genNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bGenTab1Sizer;
	bGenTab1Sizer = new wxBoxSizer(wxVERTICAL);

	m_genEncPanel = new wxPanel(m_genTab1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbGenEncSizer;
	sbGenEncSizer = new wxStaticBoxSizer(new wxStaticBox(m_genEncPanel, wxID_ANY, Trans("ui.encoderconfig.general.encoders")), wxVERTICAL);

	m_genEncFormPanel = new wxPanel(sbGenEncSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridBagSizer* gbGenEncFormSizer;
	gbGenEncFormSizer = new wxGridBagSizer(0, 0);
	gbGenEncFormSizer->SetFlexibleDirection(wxHORIZONTAL);
	gbGenEncFormSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_genEncVideoLabel = new wxStaticText(m_genEncFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.encoders.video"), wxDefaultPosition, wxDefaultSize, 0);
	m_genEncVideoLabel->Wrap(-1);
	m_genEncVideoLabel->SetMinSize(wxSize(100, -1));

	gbGenEncFormSizer->Add(m_genEncVideoLabel, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxArrayString m_genEncVideoChoiceChoices;
	m_genEncVideoChoice = new wxChoice(m_genEncFormPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_genEncVideoChoiceChoices, wxCB_SORT);
	m_genEncVideoChoice->Enable(exportInfo.video.enabled);
	m_genEncVideoChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &wxVoukoderDialog::OnVideoEncoderChanged, this);
	gbGenEncFormSizer->Add(m_genEncVideoChoice, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALL | wxEXPAND, 5);

	// General > Video > Side data

	m_genEncVideoConfig = new wxButton(m_genEncFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.encoders.video.configure"), wxDefaultPosition, wxDefaultSize, 0);
	m_genEncVideoConfig->Enable(exportInfo.video.enabled);
	m_genEncVideoConfig->Bind(wxEVT_BUTTON, [=](wxCommandEvent&)
		{
			OptionContainer* encoderOptions = &this->exportInfo.video.options;
			OptionContainer* sideData = &this->exportInfo.video.sideData;
			FilterConfig* filterConfig = &this->exportInfo.video.filters;

			std::vector<FilterInfo> filters;

			wxConfigurationDialog dialog(this, *GetSelectedEncoder(m_genEncVideoChoice), Voukoder::Config::Get().videoSideData, filters, &encoderOptions, &sideData, &filterConfig);
			dialog.SetTitle(m_genEncVideoConfig->GetLabelText());
			dialog.ShowModal();
		});
	gbGenEncFormSizer->Add(m_genEncVideoConfig, wxGBPosition(0, 2), wxGBSpan(1, 1), wxALL, 5);

	m_genEncAudioLabel = new wxStaticText(m_genEncFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.encoders.audio"), wxDefaultPosition, wxDefaultSize, 0);
	m_genEncAudioLabel->Wrap(-1);
	m_genEncAudioLabel->SetMinSize(wxSize(100, -1));

	gbGenEncFormSizer->Add(m_genEncAudioLabel, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxArrayString m_genEncAudioChoiceChoices;
	m_genEncAudioChoice = new wxChoice(m_genEncFormPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_genEncAudioChoiceChoices, wxCB_SORT);
	m_genEncAudioChoice->Enable(exportInfo.audio.enabled);
	m_genEncAudioChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &wxVoukoderDialog::OnAudioEncoderChanged, this);
	gbGenEncFormSizer->Add(m_genEncAudioChoice, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALL | wxEXPAND, 5);

	// General > Audio > Side data

	m_genEncAudioConfig = new wxButton(m_genEncFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.encoders.audio.configure"), wxDefaultPosition, wxDefaultSize, 0);
	m_genEncAudioConfig->Enable(exportInfo.audio.enabled);
	m_genEncAudioConfig->Bind(wxEVT_BUTTON, [=](wxCommandEvent&)
		{
			OptionContainer* encoderOptions = &this->exportInfo.audio.options;
			OptionContainer* sideData = &this->exportInfo.audio.sideData;
			FilterConfig* filterConfig = &this->exportInfo.audio.filters;

			std::vector<FilterInfo> filters;

			wxConfigurationDialog dialog(this, *GetSelectedEncoder(m_genEncAudioChoice), Voukoder::Config::Get().audioSideData, filters, &encoderOptions, &sideData, &filterConfig);
			dialog.SetTitle(m_genEncAudioConfig->GetLabelText());
			dialog.ShowModal();
		});
	gbGenEncFormSizer->Add(m_genEncAudioConfig, wxGBPosition(1, 2), wxGBSpan(1, 1), wxALL, 5);


	gbGenEncFormSizer->AddGrowableCol(1);

	m_genEncFormPanel->SetSizer(gbGenEncFormSizer);
	m_genEncFormPanel->Layout();
	gbGenEncFormSizer->Fit(m_genEncFormPanel);
	sbGenEncSizer->Add(m_genEncFormPanel, 1, wxEXPAND | wxALL, 5);


	m_genEncPanel->SetSizer(sbGenEncSizer);
	m_genEncPanel->Layout();
	sbGenEncSizer->Fit(m_genEncPanel);
	bGenTab1Sizer->Add(m_genEncPanel, 0, wxEXPAND | wxALL, 5);

	// General > Muxer

	m_genMuxPanel = new wxPanel(m_genTab1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbGenMuxSizer;
	sbGenMuxSizer = new wxStaticBoxSizer(new wxStaticBox(m_genMuxPanel, wxID_ANY, Trans("ui.encoderconfig.general.muxers")), wxVERTICAL);

	m_genMuxFormPanel = new wxPanel(sbGenMuxSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridBagSizer* gbGenMuxFormSizer;
	gbGenMuxFormSizer = new wxGridBagSizer(0, 0);
	gbGenMuxFormSizer->SetFlexibleDirection(wxHORIZONTAL);
	gbGenMuxFormSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_genMuxFormatLabel = new wxStaticText(m_genMuxFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.muxers.container"), wxDefaultPosition, wxDefaultSize, 0);
	m_genMuxFormatLabel->Wrap(-1);
	m_genMuxFormatLabel->SetMinSize(wxSize(100, -1));

	gbGenMuxFormSizer->Add(m_genMuxFormatLabel, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER | wxALL, 5);

	wxArrayString m_genMuxFormatChoiceChoices;
	m_genMuxFormatChoice = new wxChoice(m_genMuxFormPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_genMuxFormatChoiceChoices, wxCB_SORT);
	m_genMuxFormatChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &wxVoukoderDialog::OnMuxerChanged, this);
	gbGenMuxFormSizer->Add(m_genMuxFormatChoice, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALL | wxEXPAND, 5);

	m_genMuxFaststartCheck = new wxCheckBox(m_genMuxFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.muxers.faststart"), wxDefaultPosition, wxDefaultSize, 0);
	m_genMuxFaststartCheck->Bind(wxEVT_CHECKBOX, &wxVoukoderDialog::OnFaststartChanged, this);
	gbGenMuxFormSizer->Add(m_genMuxFaststartCheck, wxGBPosition(1, 0), wxGBSpan(1, 2), wxALL, 5);
	
	
	gbGenMuxFormSizer->AddGrowableCol(1);

	m_genMuxFormPanel->SetSizer(gbGenMuxFormSizer);
	m_genMuxFormPanel->Layout();
	gbGenMuxFormSizer->Fit(m_genMuxFormPanel);
	sbGenMuxSizer->Add(m_genMuxFormPanel, 1, wxEXPAND | wxALL, 5);


	m_genMuxPanel->SetSizer(sbGenMuxSizer);
	m_genMuxPanel->Layout();
	sbGenMuxSizer->Fit(m_genMuxPanel);
	bGenTab1Sizer->Add(m_genMuxPanel, 0, wxEXPAND | wxALL, 5);

	// General > ...

	m_generalOtherPanel = new wxPanel(m_genTab1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	bGenTab1Sizer->Add(m_generalOtherPanel, 1, wxEXPAND | wxALL, 5);

	m_genTab1->SetSizer(bGenTab1Sizer);
	m_genTab1->Layout();
	bGenTab1Sizer->Fit(m_genTab1);
	m_genNotebook->AddPage(m_genTab1, Trans("ui.encoderconfig.general"), false);

	// General > About

	m_generalAboutPanel = new wxPanel(m_genNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* aboutSizer = new wxBoxSizer(wxVERTICAL);

	// General > About > Header

	m_headerPanel = new wxPanel(m_generalAboutPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBitmap* m_logo = new wxStaticBitmap(m_headerPanel, wxID_ANY, wxBITMAP_PNG_FROM_DATA(IMG_LOGO), wxDefaultPosition, wxDefaultSize, 0);
	headerSizer->Add(m_logo, 0, wxALIGN_LEFT | wxALL, 5);

	m_infoPanel = new wxPanel(m_headerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* infoSizer = new wxBoxSizer(wxVERTICAL);
	headerSizer->Add(m_infoPanel, 0, wxALIGN_CENTER | wxALL, 0);

	wxStaticText* m_aboutVoukoder = new wxStaticText(m_infoPanel, wxID_ANY, wxT("Voukoder"), wxDefaultPosition, wxDefaultSize, 0);
	m_aboutVoukoder->SetFont(wxFont(16, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	infoSizer->Add(m_aboutVoukoder, 0, wxALIGN_CENTER | wxALL, 5);

	wxString version = wxString::Format(Trans("ui.encoderconfig.general.about.version") + " %d.%d.%d %s", VKDR_VERSION_MAJOR, VKDR_VERSION_MINOR, VKDR_VERSION_PATCH, VKDR_VERSION_SUFFIX);
	wxStaticText* m_aboutVersion = new wxStaticText(m_infoPanel, wxID_ANY, version, wxDefaultPosition, wxDefaultSize, 0);
	infoSizer->Add(m_aboutVersion, 0, wxALIGN_CENTER | wxALL, 0);
	
	m_infoPanel->SetSizer(infoSizer);
	m_infoPanel->Layout();
	m_headerPanel->SetSizer(headerSizer);
	m_headerPanel->Layout();

	aboutSizer->Add(m_headerPanel, 0, wxALIGN_CENTER | wxALL, 10);

	//
	aboutSizer->Add(CreateCenteredText(m_generalAboutPanel, Trans("ui.encoderconfig.general.about.author"), wxT("Daniel Stankewitz")), 0, wxALIGN_CENTER | wxALL, 0);
	aboutSizer->Add(CreateCenteredText(m_generalAboutPanel, Trans("ui.encoderconfig.general.about.transmaint"), wxT("Bruno T. \"MyPOV\", Cedric R.")), 0, wxALIGN_CENTER | wxALL, 0);
	aboutSizer->Add(CreateCenteredText(m_generalAboutPanel, Trans("ui.encoderconfig.general.about.logo"), wxT("Noar")), 0, wxALIGN_CENTER | wxALL, 0);
	aboutSizer->Add(CreateCenteredText(m_generalAboutPanel, Trans("ui.encoderconfig.general.about.awesomefont"), wxT("Dave Gandy / CC 3.0 BY")), 0, wxALIGN_CENTER | wxALL, 0);
	aboutSizer->Add(CreateTopPatrons(m_generalAboutPanel), 1, wxEXPAND | wxALL, 20);

	wxHyperlinkCtrl *m_hyperlink1 = new wxHyperlinkCtrl(m_generalAboutPanel, wxID_ANY, wxT("Support Voukoder on patreon.com"), wxT("https://www.patreon.com/voukoder"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	aboutSizer->Add(m_hyperlink1, 0, wxALIGN_CENTER | wxALL, 5);

	m_generalAboutPanel->SetSizer(aboutSizer);
	m_generalAboutPanel->Layout();

	m_genNotebook->AddPage(m_generalAboutPanel, Trans("ui.encoderconfig.general.about"), false);

	//

	bGenSizer->Add(m_genNotebook, 1, wxEXPAND | wxALL, 0);

	m_genPanel->SetSizer(bGenSizer);
	m_genPanel->Layout();
	bGenSizer->Fit(m_genPanel);
	m_Categories->AddPage(m_genPanel, Trans("ui.encoderconfig.general"), true);
	m_Categories->SetPageImage(imageIdx++, 0);

	// Settings

	m_settingsCategory = new wxPanel(m_Categories, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSettingsCategorySizer = new wxBoxSizer(wxVERTICAL);

	m_settingsNotebook = new wxNotebook(m_settingsCategory, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	// Settings > Basic

	m_settingsBasicPanel = new wxPanel(m_settingsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSetBasicSizer = new wxBoxSizer(wxVERTICAL);

	m_genLocPanel = new wxPanel(m_settingsBasicPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbGenLocSizer = new wxStaticBoxSizer(new wxStaticBox(m_genLocPanel, wxID_ANY, Trans("ui.encoderconfig.settings.localization")), wxVERTICAL);

	m_genLocFormPanel = new wxPanel(sbGenLocSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridBagSizer* gbGenLocFormSizer = new wxGridBagSizer(0, 0);
	gbGenLocFormSizer->SetFlexibleDirection(wxHORIZONTAL);
	gbGenLocFormSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_genLocFormatLabel = new wxStaticText(m_genLocFormPanel, wxID_ANY, Trans("ui.encoderconfig.settings.localization.language"), wxDefaultPosition, wxDefaultSize, 0);
	m_genLocFormatLabel->Wrap(-1);
	m_genLocFormatLabel->SetMinSize(wxSize(100, -1));
	gbGenLocFormSizer->Add(m_genLocFormatLabel, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER | wxALL, 5);

	wxArrayString m_genLocLanguageChoices;
	m_genLocLanguageChoice = new wxChoice(m_genLocFormPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_genLocLanguageChoices, wxCB_SORT);
	gbGenLocFormSizer->Add(m_genLocLanguageChoice, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALL | wxEXPAND, 5);

	gbGenLocFormSizer->AddGrowableCol(1);
	m_genLocFormPanel->SetSizer(gbGenLocFormSizer);
	m_genLocFormPanel->Layout();
	gbGenLocFormSizer->Fit(m_genLocFormPanel);
	sbGenLocSizer->Add(m_genLocFormPanel, 1, wxEXPAND | wxALL, 5);

	m_genLocPanel->SetSizer(sbGenLocSizer);
	m_genLocPanel->Layout();
	sbGenLocSizer->Fit(m_genLocPanel);
	bSetBasicSizer->Add(m_genLocPanel, 0, wxEXPAND | wxALL, 5);

	m_settingsBasicPanel->SetSizer(bSetBasicSizer);
	m_settingsBasicPanel->Layout();
	m_settingsNotebook->AddPage(m_settingsBasicPanel, Trans("ui.encoderconfig.settings"), false);

	// Settings > Log

	m_settingsLogPanel = new wxPanel(m_settingsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* logSizer = new wxBoxSizer(wxVERTICAL);
	
	m_generalLogText = new wxTextCtrl(m_settingsLogPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL | wxTE_RICH2);
	m_generalLogText->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));
	logSizer->Add(m_generalLogText, 1, wxALL | wxEXPAND, 5);

	// Settings > Log > Buttons

	m_genLogButtonsPanel = new wxPanel(m_settingsLogPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* btnSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_genLogClear = new wxButton(m_genLogButtonsPanel, wxID_ANY, Trans("ui.encoderconfig.settings.log.clear"), wxDefaultPosition, wxDefaultSize, 0);
	m_genLogClear->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
		Log::instance()->Clear();
		m_generalLogText->Clear();
		m_generalLogText->SetFocus();
	});
	btnSizer2->Add(m_genLogClear, 0, wxALL, 5);
	m_genLogCopyToClipboard = new wxButton(m_genLogButtonsPanel, wxID_ANY, Trans("ui.encoderconfig.settings.log.copyToClipboard"), wxDefaultPosition, wxDefaultSize, 0);
	m_genLogCopyToClipboard->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
		wxClipboard wxClip;
		if (wxClip.Open())
		{
			wxClip.Clear();
			wxClip.SetData(new wxTextDataObject(m_generalLogText->GetValue()));
			wxClip.Flush();
			wxClip.Close();
		}
	});
	btnSizer2->Add(m_genLogCopyToClipboard, 0, wxALL, 5);
	m_genLogButtonsPanel->SetSizer(btnSizer2);
	m_genLogButtonsPanel->Layout();
	btnSizer2->Fit(m_genLogButtonsPanel);
	logSizer->Add(m_genLogButtonsPanel, 0, wxEXPAND | wxALL);

	m_settingsLogPanel->SetSizer(logSizer);
	m_settingsLogPanel->Layout();
	m_settingsNotebook->AddPage(m_settingsLogPanel, Trans("ui.encoderconfig.settings.log"), false);

	bSettingsCategorySizer->Add(m_settingsNotebook, 1, wxEXPAND | wxALL, 0);

	m_settingsCategory->SetSizer(bSettingsCategorySizer);
	m_settingsCategory->Layout();
	bSettingsCategorySizer->Fit(m_settingsCategory);
	m_Categories->AddPage(m_settingsCategory, Trans("ui.encoderconfig.settings"), false);
	m_Categories->SetPageImage(imageIdx++, 3);

	// Set current version
	Version curVersion;
	curVersion.number.major = VKDR_VERSION_MAJOR;
	curVersion.number.minor = VKDR_VERSION_MINOR;
	curVersion.number.patch = VKDR_VERSION_PATCH;

	// Check for update
	PluginUpdate update;
	if (CheckForUpdate(curVersion, &update))
	{
		m_updateCategory = new wxPanel(m_Categories, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		wxBoxSizer* bUpdateCategorySizer = new wxBoxSizer(wxVERTICAL);

		m_updateNotebook = new wxNotebook(m_updateCategory, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
		m_updatePanel = new wxPanel(m_updateNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		wxBoxSizer* bUpdateLayout = new wxBoxSizer(wxVERTICAL);

		// Show the update message
		if (!update.message.IsEmpty())
		{
			m_updateText = new wxTextCtrl(m_updatePanel, wxID_ANY, update.message, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
			bUpdateLayout->Add(m_updateText, 1, wxALL | wxEXPAND, 5);
		}

		// Show the download button
		if (!update.url.IsEmpty())
		{
			m_updateButton = new wxButton(m_updatePanel, wxID_ANY, Trans("ui.encoderconfig.update.button"), wxDefaultPosition, wxDefaultSize, 0);
			m_updateButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent&)
			{
				wxLaunchDefaultBrowser(update.url, wxBROWSER_NEW_WINDOW);
			});
			bUpdateLayout->Add(m_updateButton, 0, wxALIGN_CENTER | wxALL, 5);
		}

		m_updatePanel->SetSizer(bUpdateLayout);
		m_updatePanel->Layout();
		bUpdateLayout->Fit(m_updatePanel);
		m_updateNotebook->AddPage(m_updatePanel, update.headline, false);

		bUpdateCategorySizer->Add(m_updateNotebook, 1, wxEXPAND | wxALL, 0);

		m_updateCategory->SetSizer(bUpdateCategorySizer);
		m_updateCategory->Layout();
		bUpdateCategorySizer->Fit(m_updateCategory);
		m_Categories->AddPage(m_updateCategory, Trans("ui.encoderconfig.update"), false);
		m_Categories->SetPageImage(imageIdx++, 4);
	}

	//

	bDialogLayout->Add(m_Categories, 1, wxEXPAND | wxALL, 5);

	// Button panel
	m_btnPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	btnSizer->Add(0, 0, 1, wxEXPAND, 5);
	m_btnOK = new wxButton(m_btnPanel, wxID_OK, Trans("ui.encoderconfig.buttonOkay"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnOK->Bind(wxEVT_BUTTON, &wxVoukoderDialog::OnOkayClick, this);
	btnSizer->Add(m_btnOK, 0, wxALL, 5);
	m_btnCancel = new wxButton(m_btnPanel, wxID_CANCEL, Trans("ui.encoderconfig.buttonCancel"), wxDefaultPosition, wxDefaultSize, 0);
	btnSizer->Add(m_btnCancel, 0, wxALL, 5);
	m_btnPanel->SetSizer(btnSizer);
	m_btnPanel->Layout();
	btnSizer->Fit(m_btnPanel);

	bDialogLayout->Add(m_btnPanel, 0, wxEXPAND | wxALL, 5);
	   
	this->SetSizer(bDialogLayout);
	this->Layout();

	SetMinSize({ 512, 380 });

	this->Centre(wxBOTH);

	m_generalLogText->ChangeValue(Log::instance()->GetAsString());
}

wxRichTextCtrl* wxVoukoderDialog::CreateTopPatrons(wxPanel* parent)
{
	wxRichTextCtrl* richText = new wxRichTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxWANTS_CHARS);
	richText->BeginAlignment(wxTextAttrAlignment::wxTEXT_ALIGNMENT_CENTER);

	wxHTTP get;
	get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
	get.SetHeader(_T("User-Agent"), wxString::Format(_T("voukoder/%d.%d.%d"), VKDR_VERSION_MAJOR, VKDR_VERSION_MINOR, VKDR_VERSION_PATCH));
	get.SetTimeout(2);

	if (get.Connect(VKDR_UPDATE_CHECK_HOST))
	{
		wxApp::IsMainLoopRunning();

		wxInputStream* httpStream = get.GetInputStream("/patreon/patrons.php");

		if (get.GetError() == wxPROTO_NOERR)
		{
			wxString res;
			wxStringOutputStream out_stream(&res);
			httpStream->Read(out_stream);

			richText->BeginBold();
			richText->BeginFontSize(11);
			richText->BeginUnderline();
			richText->WriteText(Trans("ui.encoderconfig.general.about.toppatrons") + "\n");
			richText->EndUnderline();
			richText->EndFontSize();
			richText->EndBold();

			int i = 0;

			wxStringTokenizer tokenizer(res, "|");
			while (tokenizer.HasMoreTokens())
			{
				wxString patron = tokenizer.GetNextToken();
				if (i < 3)
				{
					richText->BeginBold();
					richText->BeginFontSize(13);
				}
				else
				{
					richText->BeginFontSize(10);
				}

				richText->WriteText(patron + "\n");
				richText->EndFontSize();

				if (i < 3)
				{
					richText->EndBold();
				}
				i++;
			}
		}

		wxDELETE(httpStream);
		get.Close();
	}

	richText->EndAlignment();

	return richText;
}

wxPanel* wxVoukoderDialog::CreateCenteredText(wxPanel* parent, wxString label, wxString text)
{
	wxPanel* m_panel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer = new wxBoxSizer(wxHORIZONTAL);
	
	wxStaticText* m_label = new wxStaticText(m_panel, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_label->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	m_label->Wrap(-1);
	bSizer->Add(m_label, 1, wxALL, 5);

	wxStaticText* m_text = new wxStaticText(m_panel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
	m_text->Wrap(-1);
	bSizer->Add(m_text, 1, wxALL, 5);

	m_panel->SetSizer(bSizer);
	m_panel->Layout();
	bSizer->Fit(m_panel);

	return m_panel;
}

void wxVoukoderDialog::SetConfiguration()
{
	wxString vEncoder = DefaultVideoEncoder;
	wxString aEncoder = DefaultAudioEncoder;
	wxString format = DefaultMuxer;
	bool faststart = false;

	// Video encoder
	if (!exportInfo.video.id.IsEmpty())
	{
		vEncoder = exportInfo.video.id;
	}

	// Audio encoder
	if (!exportInfo.audio.id.IsEmpty())
	{
		aEncoder = exportInfo.audio.id;
	}

	// Format
	if (!exportInfo.format.id.IsEmpty())
	{
		format = exportInfo.format.id;
		faststart = exportInfo.format.faststart;
	}

	// Enable components
	m_genEncVideoLabel->Enable(exportInfo.video.enabled);
	m_genEncVideoChoice->Enable(exportInfo.video.enabled);
	m_genEncAudioLabel->Enable(exportInfo.audio.enabled);
	m_genEncAudioChoice->Enable(exportInfo.audio.enabled);

	// Populate encoders
	for (auto& encoder : Voukoder::Config::Get().encoderInfos)
	{
		if (encoder.type == AVMediaType::AVMEDIA_TYPE_VIDEO)
		{
			m_genEncVideoChoice->Append(encoder.name, (void*)&encoder);
			
			if (encoder.id == vEncoder)
				m_genEncVideoChoice->SetStringSelection(encoder.name);
		}
		else if (encoder.type == AVMediaType::AVMEDIA_TYPE_AUDIO)
		{
			m_genEncAudioChoice->Append(encoder.name, (void*)&encoder);

			if (encoder.id == aEncoder)
				m_genEncAudioChoice->SetStringSelection(encoder.name);
		}
	}

	// Check if stored video encoder is still available
	if (m_genEncVideoChoice->GetSelection() == wxNOT_FOUND)
		m_genEncVideoChoice->SetSelection(0);

	// Check if stored audio encoder is still available
	if (m_genEncAudioChoice->GetSelection() == wxNOT_FOUND)
		m_genEncAudioChoice->SetSelection(0);

	// Init encoder infos
	wxCommandEvent event;
	OnVideoEncoderChanged(event);
	OnAudioEncoderChanged(event);

	// Select the right format
	for (unsigned int i = 0; i < m_genMuxFormatChoice->GetCount(); i++)
	{
		MuxerInfo *muxerInfo = reinterpret_cast<MuxerInfo*>(m_genMuxFormatChoice->GetClientData(i));
		if (muxerInfo->id == format)
		{
			m_genMuxFormatChoice->Select(i);

			// Faststart
			m_genMuxFaststartCheck->Enable(muxerInfo->capabilities.faststart);
			m_genMuxFaststartCheck->SetValue(muxerInfo->capabilities.faststart ? faststart : false);

			OnMuxerChanged(event);

			break;
		}
	}

	Voukoder::Config &config = Voukoder::Config::Get();
	LANGID langId = LanguageUtils::GetLanguageId(config.languageInfos);

	// Populate available languages
	for (auto& languageInfo : config.languageInfos)
	{
		m_genLocLanguageChoice->Append(languageInfo.name, (void*)&languageInfo);

		if (languageInfo.langId == langId)
			m_genLocLanguageChoice->SetStringSelection(languageInfo.name);
	}
}

void wxVoukoderDialog::UpdateFormats()
{
	// Populate muxers
	for (auto& formatInfo : Voukoder::Config::Get().muxerInfos)
	{
		int index = m_genMuxFormatChoice->FindString(formatInfo.name);

		// Decide on what muxer to add or remove
		if ((!exportInfo.video.enabled || (find(formatInfo.videoCodecIds.begin(), formatInfo.videoCodecIds.end(), GetSelectedEncoder(m_genEncVideoChoice)->id) != formatInfo.videoCodecIds.end())) &&
			(!exportInfo.audio.enabled || (find(formatInfo.audioCodecIds.begin(), formatInfo.audioCodecIds.end(), GetSelectedEncoder(m_genEncAudioChoice)->id) != formatInfo.audioCodecIds.end())))
		{
			// Add supported format
			if (index == wxNOT_FOUND)
			{
				m_genMuxFormatChoice->Append(formatInfo.name, (void*)&formatInfo);
			}
		}
		else if(index != wxNOT_FOUND)
		{
			// Remove not supported format
			m_genMuxFormatChoice->Delete(index);
		}
	}

	// Select the first entry by default
	if (m_genMuxFormatChoice->GetSelection() == wxNOT_FOUND && m_genMuxFormatChoice->GetCount() > 0)
	{
		m_genMuxFormatChoice->SetSelection(0);
	}

	wxCommandEvent event;
	OnMuxerChanged(event);
}

void wxVoukoderDialog::OnVideoEncoderChanged(wxCommandEvent& event)
{
	EncoderInfo* encoderInfo = GetSelectedEncoder(m_genEncVideoChoice);
	exportInfo.video.id = encoderInfo->id;

	// Clear options when encoder changes (except on startup)
	if (event.GetId() != 0)
		exportInfo.video.options.clear();

	if (exportInfo.video.enabled)
	{
		UpdateFormats();

		// Unfold param groups
		for (auto& paramGroup : encoderInfo->paramGroups)
		{
			if (exportInfo.video.options.find(paramGroup.first) != exportInfo.video.options.end())
			{
				wxStringTokenizer tokens(exportInfo.video.options[paramGroup.first], ":");
				while (tokens.HasMoreTokens())
				{
					wxString token = tokens.GetNextToken();
					exportInfo.video.options.insert(
						std::make_pair(token.BeforeFirst('='), token.AfterFirst('=')));
				}

				exportInfo.video.options.erase(paramGroup.first);
			}
		}

		// Handle interlaced modes
		if (exportInfo.video.id == "libx264")
		{
			// Interlaced mode
			if (exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_BB ||
				exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_TT)
			{
                encoderInfo->defaults.insert(std::make_pair("interlaced", "1"));
			}

			// Field order
			if (exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_BB)
			{
                encoderInfo->defaults.insert(std::make_pair("bff", "1"));
			}
			else if (exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_TT)
			{
                encoderInfo->defaults.insert(std::make_pair("tff", "1"));
			}
		}
		else if (exportInfo.video.id == "libx265")
		{
			// Field order
			if (exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_BB)
			{
                encoderInfo->defaults.insert(std::make_pair("interlace", "bff"));
                encoderInfo->defaults.insert(std::make_pair("top", "0"));
			}
			else if (exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_TT)
			{
                encoderInfo->defaults.insert(std::make_pair("interlace", "tff"));
                encoderInfo->defaults.insert(std::make_pair("top", "1"));
			}
		}
	}
}

void wxVoukoderDialog::OnAudioEncoderChanged(wxCommandEvent& event)
{
	EncoderInfo* encoderInfo = GetSelectedEncoder(m_genEncAudioChoice);
	exportInfo.audio.id = encoderInfo->id;

	// Clear options when encoder changes (except on startup)
	if (event.GetId() != 0)
		exportInfo.video.options.clear();

	if (exportInfo.audio.enabled)
	{
		UpdateFormats();
	}
}

void wxVoukoderDialog::OnMuxerChanged(wxCommandEvent& event)
{
	MuxerInfo muxerInfo = *reinterpret_cast<MuxerInfo*>(m_genMuxFormatChoice->GetClientData(m_genMuxFormatChoice->GetSelection()));
	exportInfo.format.id = muxerInfo.id;

	if (!muxerInfo.capabilities.faststart)
	{
		m_genMuxFaststartCheck->SetValue(false);
	}

	m_genMuxFaststartCheck->Enable(muxerInfo.capabilities.faststart);

	OnFaststartChanged(event);
}

void wxVoukoderDialog::OnFaststartChanged(wxCommandEvent& event)
{
	exportInfo.format.faststart = m_genMuxFaststartCheck->GetValue();
}

void wxVoukoderDialog::OnOkayClick(wxCommandEvent& event)
{
	// Store language setting
	LanguageInfo *languageInfo = reinterpret_cast<LanguageInfo*>(m_genLocLanguageChoice->GetClientData(m_genLocLanguageChoice->GetSelection()));
	LanguageUtils::StoreLanguageId(languageInfo->langId);

	EndDialog(wxID_OK);
}

EncoderInfo* wxVoukoderDialog::GetSelectedEncoder(wxChoice* choice)
{
	return reinterpret_cast<EncoderInfo*>(choice->GetClientData(choice->GetSelection()));
}
