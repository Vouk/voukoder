#include "wxVoukoderDialog.h"
#include <map>
#include <wx/clipbrd.h>
#include <wx/msw/registry.h>
#include "EncoderUtils.h"
#include "LanguageUtils.h"
#include "RegistryUtils.h"
#include "PluginUpdate.h"
#include "Images.h"
#include "Voukoder.h"
#include "wxEncoderPage.h"
#include "Log.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxVoukoderDialog::wxVoukoderDialog(wxWindow *parent, ExportInfo &exportInfo) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	exportInfo(exportInfo)
{
	// Configure dialog window
	SetTitle(Voukoder::GetApplicationName());
	SetSize(wxDLG_UNIT(this, wxSize(340, 340)));
	SetSizeHints(wxDefaultSize, wxDefaultSize);

	// Video settings
	videoSettings.options.insert(exportInfo.video.options.begin(), exportInfo.video.options.end());
	videoSettings.sideData.insert(exportInfo.video.sideData.begin(), exportInfo.video.sideData.end());
	videoSettings.filters.Deserialize(exportInfo.video.filters.Serialize());

	// Audio settings
	audioSettings.options.insert(exportInfo.audio.options.begin(), exportInfo.audio.options.end());
	audioSettings.sideData.insert(exportInfo.audio.sideData.begin(), exportInfo.audio.sideData.end());
	audioSettings.filters.Deserialize(exportInfo.audio.filters.Serialize());

	// Build up list of supported muxers
	for (auto muxer : Voukoder::Config::Get().muxerInfos)
	{
		if (exportInfo.video.enabled != muxer.videoCodecIds.empty() &&
			exportInfo.audio.enabled != muxer.audioCodecIds.empty())
		{
			// Build up list of supported video encoders
			for (auto encoder : Voukoder::Config::Get().videoEncoderInfos)
			{
				if (find(muxer.videoCodecIds.begin(), muxer.videoCodecIds.end(), encoder.id) != muxer.videoCodecIds.end())
					videoEncoders.insert(encoder);
			}

			// Build up list of supported audio encoders
			for (auto& encoder : Voukoder::Config::Get().audioEncoderInfos)
			{
				if (find(muxer.audioCodecIds.begin(), muxer.audioCodecIds.end(), encoder.id) != muxer.audioCodecIds.end())
					audioEncoders.insert(encoder);
			}

			muxers.insert(muxer);
		}
	}

	InitGUI();
}

wxVoukoderDialog::~wxVoukoderDialog()
{
	if (m_voukoderTaskBarIcon)
		delete m_voukoderTaskBarIcon;
}

void wxVoukoderDialog::InitGUI()
{
	minLabelWidth = wxDLG_UNIT(this, wxSize(56, -1));

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);

	// Init image handlers
	wxInitAllImageHandlers();

	// Taskbar icon
	if (wxTaskBarIcon::IsAvailable())
	{
		// Load icon
		wxIcon icon;
		icon.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(IMG_LOGO));

		// Show taskbar icon
		m_voukoderTaskBarIcon = new wxVoukoderTaskBarIcon();
		m_voukoderTaskBarIcon->SetIcon(icon, Voukoder::GetApplicationName());
		m_voukoderTaskBarIcon->CheckForUpdate();
	}

	// Categories
	wxImageList* m_listbook1Images = new wxImageList(48, 48);
	{
		wxLogNull logNo;
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_OUTPUT));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_VIDEO));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_AUDIO));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_SETTINGS));
		m_listbook1Images->Add(wxBITMAP_PNG_FROM_DATA(IMG_ICON_ABOUT));
	}
	
	size_t imageIdx = 0;

	wxListbook* m_Categories = new wxListbook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT);
	m_Categories->AssignImageList(m_listbook1Images);

	// Translate boolean options
	wxPGGlobalVars->m_boolChoices[0].SetText(Trans("ui.encoderconfig.false"));
	wxPGGlobalVars->m_boolChoices[1].SetText(Trans("ui.encoderconfig.true"));

	// Video panel
	if (exportInfo.video.enabled)
	{
		m_videoPanel = new wxEncoderPage(m_Categories, videoEncoders, Voukoder::Config::Get().videoSideData, Voukoder::Config::Get().videoFilterInfos, videoSettings);
		m_videoPanel->Bind(wxEVT_ENCODER_CHANGED, &wxVoukoderDialog::OnEncoderChanged, this);
		if (!m_videoPanel->SetEncoder(exportInfo.video.id))
			m_videoPanel->SetEncoder(DefaultVideoEncoder);
		m_Categories->AddPage(m_videoPanel, Trans("ui.encoderconfig.video"), true);
		m_Categories->SetPageImage(imageIdx++, 1);
	}

	// Audio panel
	if (exportInfo.audio.enabled)
	{
		m_audioPanel = new wxEncoderPage(m_Categories, audioEncoders, Voukoder::Config::Get().audioSideData, Voukoder::Config::Get().audioFilterInfos, audioSettings);
		m_audioPanel->Bind(wxEVT_ENCODER_CHANGED, &wxVoukoderDialog::OnEncoderChanged, this);
		if (!m_audioPanel->SetEncoder(exportInfo.audio.id))
			m_audioPanel->SetEncoder(DefaultAudioEncoder);
		m_Categories->AddPage(m_audioPanel, Trans("ui.encoderconfig.audio"), !exportInfo.video.enabled);
		m_Categories->SetPageImage(imageIdx++, 2);
	}

	// General panel
	m_Categories->AddPage(CreateGeneralPanel(m_Categories), Trans("ui.encoderconfig.general"), false);
	m_Categories->SetPageImage(imageIdx++, 0);
	
	// Settings panel
	m_Categories->AddPage(CreateSettingsPanel(m_Categories), Trans("ui.encoderconfig.settings"), false);
	m_Categories->SetPageImage(imageIdx++, 3);

	// About panel
	m_Categories->AddPage(CreateAboutPanel(m_Categories), Trans("ui.encoderconfig.about"), false);
	m_Categories->SetPageImage(imageIdx++, 4);

	bDialogLayout->Add(m_Categories, 1, wxEXPAND | wxALL, 5);

	// Button panel
	wxPanel* m_btnPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	btnSizer->Add(0, 0, 1, wxEXPAND, 5);
	wxButton* m_btnOK = new wxButton(m_btnPanel, wxID_OK, Trans("ui.encoderconfig.buttonOkay"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnOK->Bind(wxEVT_BUTTON, &wxVoukoderDialog::OnOkayClick, this);
	btnSizer->Add(m_btnOK, 0, wxALL, 5);
	wxButton* m_btnCancel = new wxButton(m_btnPanel, wxID_CANCEL, Trans("ui.encoderconfig.buttonCancel"), wxDefaultPosition, wxDefaultSize, 0);
	btnSizer->Add(m_btnCancel, 0, wxALL, 5);
	m_btnPanel->SetSizer(btnSizer);
	m_btnPanel->Layout();
	btnSizer->Fit(m_btnPanel);

	bDialogLayout->Add(m_btnPanel, 0, wxEXPAND | wxALL, 5);
	   
	this->SetSizer(bDialogLayout);
	this->Layout();

	SetMinSize(wxDLG_UNIT(this, wxSize(256, 190)));

	this->Centre(wxBOTH);
  
	SetConfiguration();
}

wxControl* wxVoukoderDialog::CreateGeneralPanel(wxWindow* parent)
{
	wxNotebook* m_notebook = new wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	wxPanel* m_genPanel = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bGenSizer = new wxBoxSizer(wxVERTICAL);

	// General > Muxer

	wxPanel* m_genMuxPanel = new wxPanel(m_genPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbGenMuxSizer;
	sbGenMuxSizer = new wxStaticBoxSizer(new wxStaticBox(m_genMuxPanel, wxID_ANY, Trans("ui.encoderconfig.general.muxers")), wxVERTICAL);

	wxPanel* m_genMuxFormPanel = new wxPanel(sbGenMuxSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridBagSizer* gbGenMuxFormSizer = new wxGridBagSizer(0, 0);
	gbGenMuxFormSizer->SetFlexibleDirection(wxHORIZONTAL);
	gbGenMuxFormSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_genMuxFormatLabel = new wxStaticText(m_genMuxFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.muxers.container"), wxDefaultPosition, wxDefaultSize, 0);
	m_genMuxFormatLabel->Wrap(-1);
	m_genMuxFormatLabel->SetMinSize(minLabelWidth);
	gbGenMuxFormSizer->Add(m_genMuxFormatLabel, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER | wxALL, 5);

	wxArrayString m_genMuxFormatChoiceChoices;
	m_genMuxFormatChoice = new wxChoice(m_genMuxFormPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_genMuxFormatChoiceChoices, wxCB_SORT);
	m_genMuxFormatChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &wxVoukoderDialog::OnMuxerChanged, this);
	gbGenMuxFormSizer->Add(m_genMuxFormatChoice, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALL | wxEXPAND, 5);

	m_genMuxFaststartCheck = new wxCheckBox(m_genMuxFormPanel, wxID_ANY, Trans("ui.encoderconfig.general.muxers.faststart"), wxDefaultPosition, wxDefaultSize, 0);
	gbGenMuxFormSizer->Add(m_genMuxFaststartCheck, wxGBPosition(1, 0), wxGBSpan(1, 2), wxALL, 5);

	gbGenMuxFormSizer->AddGrowableCol(1);

	m_genMuxFormPanel->SetSizer(gbGenMuxFormSizer);
	m_genMuxFormPanel->Layout();
	gbGenMuxFormSizer->Fit(m_genMuxFormPanel);
	sbGenMuxSizer->Add(m_genMuxFormPanel, 1, wxEXPAND | wxALL, 5);


	m_genMuxPanel->SetSizer(sbGenMuxSizer);
	m_genMuxPanel->Layout();
	sbGenMuxSizer->Fit(m_genMuxPanel);
	bGenSizer->Add(m_genMuxPanel, 0, wxEXPAND | wxALL, 5);
		
	//
	
	m_genPanel->SetSizer(bGenSizer);
	m_genPanel->Layout();
	bGenSizer->Fit(m_genPanel);

	m_notebook->AddPage(m_genPanel, Trans("ui.encoderconfig.general"), true);

	return m_notebook;
}

wxControl* wxVoukoderDialog::CreateSettingsPanel(wxWindow* parent)
{
	wxNotebook* m_notebook = new wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	wxPanel* panel = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSettingsCategorySizer = new wxBoxSizer(wxVERTICAL);

	// Language

	wxPanel* m_genLocPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbGenLocSizer = new wxStaticBoxSizer(new wxStaticBox(m_genLocPanel, wxID_ANY, Trans("ui.encoderconfig.settings.localization")), wxVERTICAL);

	wxPanel* m_genLocFormPanel = new wxPanel(sbGenLocSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridBagSizer* gbGenLocFormSizer = new wxGridBagSizer(0, 0);
	gbGenLocFormSizer->SetFlexibleDirection(wxHORIZONTAL);
	gbGenLocFormSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_genLocFormatLabel = new wxStaticText(m_genLocFormPanel, wxID_ANY, Trans("ui.encoderconfig.settings.localization.language"), wxDefaultPosition, wxDefaultSize, 0);
	m_genLocFormatLabel->Wrap(-1);
	m_genLocFormatLabel->SetMinSize(minLabelWidth);
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

	bSettingsCategorySizer->Add(m_genLocPanel, 0, wxEXPAND | wxALL, 5);

	// Logging

	wxPanel* m_genLogPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbGenLogSizer = new wxStaticBoxSizer(new wxStaticBox(m_genLogPanel, wxID_ANY, Trans("ui.encoderconfig.settings.logging")), wxVERTICAL);

	m_checkBox1 = new wxCheckBox(sbGenLogSizer->GetStaticBox(), wxID_ANY, Trans("ui.encoderconfig.settings.logging.separate"), wxDefaultPosition, wxDefaultSize, 0);
	sbGenLogSizer->Add(m_checkBox1, 0, wxALL, 5);

	m_checkBox2 = new wxCheckBox(sbGenLogSizer->GetStaticBox(), wxID_ANY, Trans("ui.encoderconfig.settings.logging.lowlevel"), wxDefaultPosition, wxDefaultSize, 0);
	sbGenLogSizer->Add(m_checkBox2, 0, wxALL, 5);

	m_genLogPanel->SetSizer(sbGenLogSizer);
	m_genLogPanel->Layout();
	sbGenLogSizer->Fit(m_genLogPanel);

	bSettingsCategorySizer->Add(m_genLogPanel, 0, wxEXPAND | wxALL, 5);

	//

	panel->SetSizer(bSettingsCategorySizer);
	panel->Layout();
	bSettingsCategorySizer->Fit(panel);

	m_notebook->AddPage(panel, Trans("ui.encoderconfig.settings"), true);

	return m_notebook;
}

wxControl* wxVoukoderDialog::CreateAboutPanel(wxWindow* parent)
{
	wxNotebook* m_notebook = new wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	wxPanel* panel = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* aboutSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* m_headerPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBitmap* m_logo = new wxStaticBitmap(m_headerPanel, wxID_ANY, wxBITMAP_PNG_FROM_DATA(IMG_LOGO), wxDefaultPosition, wxDefaultSize, 0);
	headerSizer->Add(m_logo, 0, wxALIGN_LEFT | wxALL, 5);

	wxPanel* m_infoPanel = new wxPanel(m_headerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* infoSizer = new wxBoxSizer(wxVERTICAL);
	headerSizer->Add(m_infoPanel, 0, wxALIGN_CENTER | wxALL, 0);

	wxStaticText* m_aboutVoukoder = new wxStaticText(m_infoPanel, wxID_ANY, wxT("Voukoder"), wxDefaultPosition, wxDefaultSize, 0);
	m_aboutVoukoder->SetFont(wxFont(16, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	infoSizer->Add(m_aboutVoukoder, 0, wxALIGN_CENTER | wxALL, 5);

	wxString version = Trans("ui.encoderconfig.about.version") + " " + Voukoder::GetAppVersion();
	wxStaticText* m_aboutVersion = new wxStaticText(m_infoPanel, wxID_ANY, version, wxDefaultPosition, wxDefaultSize, 0);
	infoSizer->Add(m_aboutVersion, 0, wxALIGN_CENTER | wxALL, 0);

	m_infoPanel->SetSizer(infoSizer);
	m_infoPanel->Layout();
	m_headerPanel->SetSizer(headerSizer);
	m_headerPanel->Layout();

	aboutSizer->Add(m_headerPanel, 0, wxALIGN_CENTER | wxALL, 10);

	// Authors
	aboutSizer->Add(CreateCenteredText(panel, Trans("ui.encoderconfig.about.author"), wxT("Daniel Stankewitz - @LordVouk"), wxT("https://twitter.com/LordVouk")), 0, wxALIGN_CENTER | wxALL, 0);
	aboutSizer->Add(CreateCenteredText(panel, Trans("ui.encoderconfig.about.transmaint"), wxT("Bruno T. \"MyPOV\", Cedric R., Atreides")), 0, wxALIGN_CENTER | wxALL, 0);
	aboutSizer->Add(CreateCenteredText(panel, Trans("ui.encoderconfig.about.logo"), wxT("Noar")), 0, wxALIGN_CENTER | wxALL, 0);

	// (Top) patrons
	wxStaticText* m_label = new wxStaticText(panel, wxID_ANY, Trans("ui.encoderconfig.about.toppatrons"), wxDefaultPosition, wxDefaultSize, wxALL);
	m_label->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	m_label->Wrap(-1);
	aboutSizer->Add(m_label, 0, wxALIGN_LEFT | wxALL, 10);
	aboutSizer->Add(CreateTopPatrons(panel), 1, wxEXPAND | wxALL, 10);

	wxBoxSizer* bSizerSupport = new wxBoxSizer(wxHORIZONTAL);
	wxHyperlinkCtrl* m_hyperlink1 = new wxHyperlinkCtrl(panel, wxID_ANY, Trans("ui.encoderconfig.about.support.patreon"), wxT("https://www.patreon.com/voukoder"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	bSizerSupport->Add(m_hyperlink1, 0, wxALL, 5);
	wxHyperlinkCtrl* m_hyperlink2 = new wxHyperlinkCtrl(panel, wxID_ANY, Trans("ui.encoderconfig.about.support.paypal"), wxT("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=S6LGDW9QZYBTL&source=url"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	bSizerSupport->Add(m_hyperlink2, 0, wxALL, 5);
	aboutSizer->Add(bSizerSupport, 0, wxALIGN_CENTER, 0);

	panel->SetSizer(aboutSizer);
	panel->Layout();
	
	m_notebook->AddPage(panel, Trans("ui.encoderconfig.about"), true);

	return m_notebook;
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

wxPanel* wxVoukoderDialog::CreateCenteredText(wxPanel* parent, wxString label, wxString text, wxString link)
{
	wxPanel* m_panel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer = new wxBoxSizer(wxHORIZONTAL);
	
	wxStaticText* m_label = new wxStaticText(m_panel, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_label->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	m_label->Wrap(-1);
	bSizer->Add(m_label, 1, wxALL, 5);

	if (link.IsEmpty())
	{
		wxStaticText* m_text = new wxStaticText(m_panel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
		m_text->Wrap(-1);
		bSizer->Add(m_text, 1, wxALL, 5);
	}
	else
	{
		wxHyperlinkCtrl* m_hyperlink1 = new wxHyperlinkCtrl(m_panel, wxID_ANY, text, link, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
		m_hyperlink1->SetToolTip(link);
		bSizer->Add(m_hyperlink1, 1, wxALL, 5);
	}

	m_panel->SetSizer(bSizer);
	m_panel->Layout();
	bSizer->Fit(m_panel);

	return m_panel;
}

void wxVoukoderDialog::SetConfiguration()
{
	// Populate muxers
	wxEncoderChangedEvent event;
	OnEncoderChanged(event);

	// Get UI languages
	Voukoder::Config &config = Voukoder::Config::Get();
	LANGID langId = LanguageUtils::GetLanguageId(config.languageInfos);

	// Populate available languages
	for (auto& languageInfo : config.languageInfos)
	{
		m_genLocLanguageChoice->Append(languageInfo.name, (void*)&languageInfo);

		if (languageInfo.langId == langId)
			m_genLocLanguageChoice->SetStringSelection(languageInfo.name);
	}

	bool sepLogFiles = RegistryUtils::GetValue(VKDR_REG_SEP_LOG_FILES, false);
	m_checkBox1->SetValue(sepLogFiles);

	bool lowLevelLogging = RegistryUtils::GetValue(VKDR_REG_LOW_LEVEL_LOGGING, false);
	m_checkBox2->SetValue(lowLevelLogging);
}

void wxVoukoderDialog::OnEncoderChanged(wxEncoderChangedEvent& event)
{
	EncoderInfo* videoInfo = NULL;
	EncoderInfo* audioInfo = NULL;

	// Get selected encoders
	if (m_videoPanel)
	{
		videoInfo = m_videoPanel->GetSelectedEncoder();
		if (videoInfo == NULL)
		{
			vkLogError("Video info choice did not contain any data.");
			return;
		}
	}

	if (m_audioPanel)
	{
		audioInfo = m_audioPanel->GetSelectedEncoder();
		if (audioInfo == NULL)
		{
			vkLogError("Audio info choice did not contain any data.");
			return;
		}
	}

	// Refresh muxers
	for (auto& info : muxers)
	{
		int index = m_genMuxFormatChoice->FindString(info.name);
		bool videoSupported = videoInfo && find(info.videoCodecIds.begin(), info.videoCodecIds.end(), videoInfo->id) != info.videoCodecIds.end();
		bool audioSupported = audioInfo && find(info.audioCodecIds.begin(), info.audioCodecIds.end(), audioInfo->id) != info.audioCodecIds.end();

		// Decide on what muxer to add or remove
		if ((!exportInfo.video.enabled || videoSupported) && (!exportInfo.audio.enabled || audioSupported))
		{
			// Add supported format
			if (index == wxNOT_FOUND)
				m_genMuxFormatChoice->Append(info.name, (void*)&info);
		}
		else if (index != wxNOT_FOUND)
			m_genMuxFormatChoice->Delete(index);
	}

	// Select the right muxer
	if (SelectChoiceById<MuxerInfo*>(m_genMuxFormatChoice, exportInfo.format.id, DefaultMuxer))
	{
		wxCommandEvent event;
		OnMuxerChanged(event);
	}
}

void wxVoukoderDialog::OnMuxerChanged(wxCommandEvent& event)
{
	MuxerInfo* info = GetDataFromSelectedChoice<MuxerInfo*>(m_genMuxFormatChoice);
	if (info == NULL)
	{
		vkLogError("Muxer choice did not contain any data.");
		return;
	}

	exportInfo.format.id = info->id;

	// Faststart
	m_genMuxFaststartCheck->Enable(info->capabilities.faststart);

	// Disable faststart if it is not supported
	if (info->capabilities.faststart)
		m_genMuxFaststartCheck->SetValue(exportInfo.format.faststart);
	else
		m_genMuxFaststartCheck->SetValue(false);
}

void wxVoukoderDialog::OnOkayClick(wxCommandEvent& event)
{
	// Video
	if (m_videoPanel)
	{
		m_videoPanel->ApplyChanges();

		EncoderInfo* info = m_videoPanel->GetSelectedEncoder();
		if (info)
		{
			exportInfo.video.id = info->id;

			// Copy options
			exportInfo.video.options.clear();
			exportInfo.video.options.insert(info->defaults.begin(), info->defaults.end());
			for (auto item : videoSettings.options)
				exportInfo.video.options[item.first] = item.second;
		}

		// Copy side data
		exportInfo.video.sideData.clear();
		exportInfo.video.sideData.insert(videoSettings.sideData.begin(), videoSettings.sideData.end());

		// Copy filters
		exportInfo.video.filters.Deserialize(videoSettings.filters.Serialize());
	}

	// Audio
	if (m_audioPanel)
	{
		m_audioPanel->ApplyChanges();

		EncoderInfo* info = m_audioPanel->GetSelectedEncoder();
		if (info)
		{
			exportInfo.audio.id = info->id;

			// Copy options
			exportInfo.audio.options.clear();
			exportInfo.audio.options.insert(info->defaults.begin(), info->defaults.end());
			for (auto item : audioSettings.options)
				exportInfo.audio.options[item.first] = item.second;
		}

		// Copy side data
		exportInfo.audio.sideData.clear();
		exportInfo.audio.sideData.insert(audioSettings.sideData.begin(), audioSettings.sideData.end());

		// Copy filters
		exportInfo.audio.filters.Deserialize(audioSettings.filters.Serialize());
	}

	// Store muxer settings
	MuxerInfo* info = GetDataFromSelectedChoice<MuxerInfo*>(m_genMuxFormatChoice);
	exportInfo.format.id = info->id;
	exportInfo.format.faststart = m_genMuxFaststartCheck->IsEnabled() && m_genMuxFaststartCheck->IsChecked();

	// Store language setting
	LanguageInfo* languageInfo = GetDataFromSelectedChoice<LanguageInfo*>(m_genLocLanguageChoice);
	LanguageUtils::StoreLanguageId(languageInfo->langId);

	// Store logging settings
	RegistryUtils::SetValue(VKDR_REG_SEP_LOG_FILES, m_checkBox1->GetValue());

	// Store low level logging
	RegistryUtils::SetValue(VKDR_REG_LOW_LEVEL_LOGGING, m_checkBox2->GetValue());

	EndDialog(wxID_OK);
}

template <class T>
T wxVoukoderDialog::FindInfoById(wxChoice *choice, wxString id)
{
	for (unsigned int i = 0; i < choice->GetCount(); i++)
	{
		T info = reinterpret_cast<T>(choice->GetClientData(i));
		if (info->id == id)
			return info;
	}
	return NULL;
}

template <class T>
T wxVoukoderDialog::SelectChoiceById(wxChoice *choice, wxString id, wxString def)
{
	// If nothing is selected but choices are available
	if (choice->GetSelection() == wxNOT_FOUND &&
		choice->GetCount() > 0)
	{
		T info;
		if ((info = FindInfoById<T>(choice, id)) == NULL)
			info = FindInfoById<T>(choice, def);

		if (info == NULL)
			choice->SetSelection(0);
		else
			choice->SetStringSelection(info->name);

		return info;
	}

	return NULL;
}

template <class T>
T wxVoukoderDialog::GetDataFromSelectedChoice(wxChoice* choice)
{
	int sel = choice->GetSelection();

	// Find data from selected choice or NULL if nothing is selected
	if (sel != wxNOT_FOUND)
		return reinterpret_cast<T>(choice->GetClientData(sel));

	return NULL;
}
