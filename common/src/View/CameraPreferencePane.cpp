/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CameraPreferencePane.h"

#include "StringUtils.h"
#include "PreferenceManager.h"
#include "Preferences.h"
#include "View/BorderLine.h"
#include "View/KeyboardShortcutEditor.h"
#include "View/KeyboardShortcutEvent.h"
#include "View/ViewConstants.h"

#include <wx/checkbox.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

namespace TrenchBroom {
    namespace View {
        CameraPreferencePane::CameraPreferencePane(wxWindow* parent) :
        PreferencePane(parent) {
            createGui();
            bindEvents();
        }
        
        
        void CameraPreferencePane::OnLookSpeedChanged(wxScrollEvent& event) {
            const float value = m_lookSpeedSlider->GetValue() / 100.0f;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraLookSpeed, value);
        }
        
        void CameraPreferencePane::OnInvertLookHAxisChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraLookInvertH, value);
        }
        
        void CameraPreferencePane::OnInvertLookVAxisChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraLookInvertV, value);
        }
        
        void CameraPreferencePane::OnPanSpeedChanged(wxScrollEvent& event) {
            const float value = m_panSpeedSlider->GetValue() / 100.0f;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraPanSpeed, value);
        }
        
        void CameraPreferencePane::OnInvertPanHAxisChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraPanInvertH, value);
        }
        
        void CameraPreferencePane::OnInvertPanVAxisChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraPanInvertV, value);
        }
        
        void CameraPreferencePane::OnMoveSpeedChanged(wxScrollEvent& event) {
            const float value = m_moveSpeedSlider->GetValue() / 100.0f;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraMoveSpeed, value);
        }
        
        void CameraPreferencePane::OnEnableAltMoveChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraEnableAltMove, value);
        }
        
        void CameraPreferencePane::OnInvertAltMoveAxisChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraAltMoveInvert, value);
        }
        
        void CameraPreferencePane::OnMoveCameraInCursorDirChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraMoveInCursorDir, value);
        }
        
        void CameraPreferencePane::OnFlySpeedChanged(wxScrollEvent& event) {
            const float value = m_flySpeedSlider->GetValue() / 100.0f;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraFlySpeed, value);
        }
        
        void CameraPreferencePane::OnInvertFlyVAxisChanged(wxCommandEvent& event) {
            const bool value = event.GetInt() != 0;
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.set(Preferences::CameraFlyInvertV, value);
        }
        
        void CameraPreferencePane::OnForwardKeyChanged(KeyboardShortcutEvent& event) {
            PreferenceManager& prefs = PreferenceManager::instance();
            
            const KeyboardShortcut shortcut(event.key(), event.modifier1(), event.modifier2(), event.modifier3());
            if (prefs.get(Preferences::CameraFlyBackward).hasKey() && prefs.get(Preferences::CameraFlyBackward) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyLeft).hasKey() && prefs.get(Preferences::CameraFlyLeft) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyRight).hasKey() && prefs.get(Preferences::CameraFlyRight) == shortcut)
                event.Veto();
            else
                prefs.set(Preferences::CameraFlyForward, shortcut);
        }
        
        void CameraPreferencePane::OnBackwardKeyChanged(KeyboardShortcutEvent& event) {
            PreferenceManager& prefs = PreferenceManager::instance();

            const KeyboardShortcut shortcut(event.key(), event.modifier1(), event.modifier2(), event.modifier3());
            if (prefs.get(Preferences::CameraFlyForward).hasKey() && prefs.get(Preferences::CameraFlyForward) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyLeft).hasKey() && prefs.get(Preferences::CameraFlyLeft) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyRight).hasKey() && prefs.get(Preferences::CameraFlyRight) == shortcut)
                event.Veto();
            else
                prefs.set(Preferences::CameraFlyBackward, shortcut);
        }
        
        void CameraPreferencePane::OnLeftKeyChanged(KeyboardShortcutEvent& event) {
            PreferenceManager& prefs = PreferenceManager::instance();

            const KeyboardShortcut shortcut(event.key(), event.modifier1(), event.modifier2(), event.modifier3());
            if (prefs.get(Preferences::CameraFlyForward).hasKey() && prefs.get(Preferences::CameraFlyForward) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyBackward).hasKey() && prefs.get(Preferences::CameraFlyBackward) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyRight).hasKey() && prefs.get(Preferences::CameraFlyRight) == shortcut)
                event.Veto();
            else
                prefs.set(Preferences::CameraFlyLeft, shortcut);
        }
        
        void CameraPreferencePane::OnRightKeyChanged(KeyboardShortcutEvent& event) {
            PreferenceManager& prefs = PreferenceManager::instance();

            const KeyboardShortcut shortcut(event.key(), event.modifier1(), event.modifier2(), event.modifier3());
            if (prefs.get(Preferences::CameraFlyForward).hasKey() && prefs.get(Preferences::CameraFlyForward) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyBackward).hasKey() && prefs.get(Preferences::CameraFlyBackward) == shortcut)
                event.Veto();
            else if (prefs.get(Preferences::CameraFlyLeft).hasKey() && prefs.get(Preferences::CameraFlyLeft) == shortcut)
                event.Veto();
            else
                prefs.set(Preferences::CameraFlyRight, shortcut);
        }
        
        void CameraPreferencePane::createGui() {
            wxWindow* mousePreferences = createCameraPreferences();

            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->AddSpacer(LayoutConstants::NarrowVMargin);
            sizer->Add(mousePreferences, 1, wxEXPAND);
            sizer->AddSpacer(LayoutConstants::WideVMargin);
            
            SetMinSize(sizer->GetMinSize());
            SetSizer(sizer);

            SetBackgroundColour(*wxWHITE);
        }
        
        wxWindow* CameraPreferencePane::createCameraPreferences() {
            wxPanel* box = new wxPanel(this);
            box->SetBackgroundColour(*wxWHITE);
            
            wxStaticText* lookPrefsHeader = new wxStaticText(box, wxID_ANY, "Mouse Look");
            lookPrefsHeader->SetFont(lookPrefsHeader->GetFont().Bold());
            wxStaticText* lookSpeedLabel = new wxStaticText(box, wxID_ANY, "Sensitivity");
            m_lookSpeedSlider = new wxSlider(box, wxID_ANY, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_BOTTOM);
            m_invertLookHAxisCheckBox = new wxCheckBox(box, wxID_ANY, "Invert X Axis");
            m_invertLookVAxisCheckBox = new wxCheckBox(box, wxID_ANY, "Invert Y Axis");
            
            wxStaticText* panPrefsHeader = new wxStaticText(box, wxID_ANY, "Mouse Pan");
            panPrefsHeader->SetFont(panPrefsHeader->GetFont().Bold());
            wxStaticText* panSpeedLabel = new wxStaticText(box, wxID_ANY, "Sensitivity");
            m_panSpeedSlider = new wxSlider(box, wxID_ANY, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_BOTTOM);
            
            m_invertPanHAxisCheckBox = new wxCheckBox(box, wxID_ANY, "Invert X Axis");
            m_invertPanVAxisCheckBox = new wxCheckBox(box, wxID_ANY, "Invert Y Axis");
            wxStaticText* movePrefsHeader = new wxStaticText(box, wxID_ANY, "Mouse Move");
            movePrefsHeader->SetFont(movePrefsHeader->GetFont().Bold());
            
            wxStaticText* moveSpeedLabel = new wxStaticText(box, wxID_ANY, "Sensitivity");
            m_moveSpeedSlider = new wxSlider(box, wxID_ANY, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_BOTTOM);
            m_enableAltMoveCheckBox = new wxCheckBox(box, wxID_ANY, "Alt+MMB drag to move camera");
            m_invertAltMoveAxisCheckBox = new wxCheckBox(box, wxID_ANY, "Invert Z axis in Alt+MMB drag");
            m_moveInCursorDirCheckBox = new wxCheckBox(box, wxID_ANY, "Move camera towards cursor");
            
            wxStaticText* flyPrefsHeader = new wxStaticText(box, wxID_ANY, "Fly Mode");
            flyPrefsHeader->SetFont(lookPrefsHeader->GetFont().Bold());
            wxStaticText* flySpeedLabel = new wxStaticText(box, wxID_ANY, "Sensitivity");
            m_flySpeedSlider = new wxSlider(box, wxID_ANY, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_BOTTOM);
            m_invertFlyVAxisCheckBox = new wxCheckBox(box, wxID_ANY, "Invert Y Axis");
            
            wxStaticText* forwardKeyLabel = new wxStaticText(box, wxID_ANY, "Forward");
            m_forwardKeyEditor = new KeyboardShortcutEditor(box, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME);
            m_forwardKeyEditor->SetMinSize(wxSize(80, wxDefaultCoord));
            wxStaticText* backwardKeyLabel = new wxStaticText(box, wxID_ANY, "Backward");
            m_backwardKeyEditor = new KeyboardShortcutEditor(box, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME);
            m_backwardKeyEditor->SetMinSize(wxSize(80, wxDefaultCoord));
            wxStaticText* leftKeyLabel = new wxStaticText(box, wxID_ANY, "Left");
            m_leftKeyEditor = new KeyboardShortcutEditor(box, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME);
            m_leftKeyEditor->SetMinSize(wxSize(80, wxDefaultCoord));
            wxStaticText* rightKeyLabel = new wxStaticText(box, wxID_ANY, "Right");
            m_rightKeyEditor = new KeyboardShortcutEditor(box, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME);
            m_rightKeyEditor->SetMinSize(wxSize(80, wxDefaultCoord));
            
            const int HMargin           = LayoutConstants::WideHMargin;
            const int LMargin           = LayoutConstants::WideVMargin;
            const int HeaderFlags       = wxLEFT;
            const int LabelFlags        = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxLEFT;
            const int SliderFlags       = wxEXPAND | wxRIGHT;
            const int CheckBoxFlags     = wxRIGHT;
            const int KeyEditorFlags    = wxRIGHT;
            const int LineFlags         = wxEXPAND | wxTOP;
            
            wxGridBagSizer* sizer = new wxGridBagSizer(LayoutConstants::NarrowVMargin, LayoutConstants::WideHMargin);
            sizer->Add(lookPrefsHeader,             wxGBPosition( 0, 0), wxGBSpan(1,2), HeaderFlags, HMargin);
            sizer->Add(lookSpeedLabel,              wxGBPosition( 1, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_lookSpeedSlider,           wxGBPosition( 1, 1), wxDefaultSpan, SliderFlags, HMargin);
            sizer->Add(m_invertLookHAxisCheckBox,   wxGBPosition( 2, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            sizer->Add(m_invertLookVAxisCheckBox,   wxGBPosition( 3, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            sizer->Add(new BorderLine(box),         wxGBPosition( 4, 0), wxGBSpan(1,2), LineFlags, LMargin);
            
            sizer->Add(panPrefsHeader,              wxGBPosition( 5, 0), wxGBSpan(1,2), HeaderFlags, HMargin);
            sizer->Add(panSpeedLabel,               wxGBPosition( 6, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_panSpeedSlider,            wxGBPosition( 6, 1), wxDefaultSpan, SliderFlags, HMargin);
            sizer->Add(m_invertPanHAxisCheckBox,    wxGBPosition( 7, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            sizer->Add(m_invertPanVAxisCheckBox,    wxGBPosition( 8, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            sizer->Add(new BorderLine(box),         wxGBPosition( 9, 0), wxGBSpan(1,2), LineFlags, LMargin);
            
            sizer->Add(movePrefsHeader,             wxGBPosition(10, 0), wxGBSpan(1,2), HeaderFlags, HMargin);
            sizer->Add(moveSpeedLabel,              wxGBPosition(11, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_moveSpeedSlider,           wxGBPosition(11, 1), wxDefaultSpan, SliderFlags, HMargin);
            sizer->Add(m_enableAltMoveCheckBox,     wxGBPosition(12, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            sizer->Add(m_invertAltMoveAxisCheckBox, wxGBPosition(13, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            sizer->Add(m_moveInCursorDirCheckBox,   wxGBPosition(14, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            sizer->Add(new BorderLine(box),         wxGBPosition(15, 0), wxGBSpan(1,2), LineFlags, LMargin);
            
            sizer->Add(flyPrefsHeader,              wxGBPosition(16, 0), wxGBSpan(1,2), HeaderFlags, HMargin);
            sizer->Add(flySpeedLabel,               wxGBPosition(17, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_flySpeedSlider,            wxGBPosition(17, 1), wxDefaultSpan, SliderFlags, HMargin);
            sizer->Add(m_invertFlyVAxisCheckBox,    wxGBPosition(18, 1), wxDefaultSpan, CheckBoxFlags, HMargin);
            
            sizer->Add(forwardKeyLabel,             wxGBPosition(19, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_forwardKeyEditor,          wxGBPosition(19, 1), wxDefaultSpan, KeyEditorFlags, HMargin);
            sizer->Add(backwardKeyLabel,            wxGBPosition(20, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_backwardKeyEditor,         wxGBPosition(20, 1), wxDefaultSpan, KeyEditorFlags, HMargin);
            sizer->Add(leftKeyLabel,                wxGBPosition(21, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_leftKeyEditor,             wxGBPosition(21, 1), wxDefaultSpan, KeyEditorFlags, HMargin);
            sizer->Add(rightKeyLabel,               wxGBPosition(22, 0), wxDefaultSpan, LabelFlags, HMargin);
            sizer->Add(m_rightKeyEditor,            wxGBPosition(22, 1), wxDefaultSpan, KeyEditorFlags, HMargin);
            
            sizer->AddGrowableCol(1);
            sizer->SetMinSize(500, wxDefaultCoord);
            box->SetSizer(sizer);
            return box;
        }
        
        void CameraPreferencePane::bindEvents() {
            m_invertLookHAxisCheckBox->Bind(wxEVT_CHECKBOX, &CameraPreferencePane::OnInvertLookHAxisChanged, this);
            m_invertLookVAxisCheckBox->Bind(wxEVT_CHECKBOX, &CameraPreferencePane::OnInvertLookVAxisChanged, this);
            m_invertPanHAxisCheckBox->Bind(wxEVT_CHECKBOX, &CameraPreferencePane::OnInvertPanHAxisChanged, this);
            m_invertPanVAxisCheckBox->Bind(wxEVT_CHECKBOX, &CameraPreferencePane::OnInvertPanVAxisChanged, this);
            m_enableAltMoveCheckBox->Bind(wxEVT_CHECKBOX, &CameraPreferencePane::OnEnableAltMoveChanged, this);
            m_moveInCursorDirCheckBox->Bind(wxEVT_CHECKBOX, &CameraPreferencePane::OnMoveCameraInCursorDirChanged, this);
            m_invertFlyVAxisCheckBox->Bind(wxEVT_CHECKBOX, &CameraPreferencePane::OnInvertFlyVAxisChanged, this);
            
            bindSliderEvents(m_lookSpeedSlider, &CameraPreferencePane::OnLookSpeedChanged, this);
            bindSliderEvents(m_panSpeedSlider, &CameraPreferencePane::OnPanSpeedChanged, this);
            bindSliderEvents(m_moveSpeedSlider, &CameraPreferencePane::OnMoveSpeedChanged, this);
            bindSliderEvents(m_flySpeedSlider, &CameraPreferencePane::OnFlySpeedChanged, this);
            
            m_forwardKeyEditor->Bind(KEYBOARD_SHORTCUT_EVENT, &CameraPreferencePane::OnForwardKeyChanged, this);
            m_backwardKeyEditor->Bind(KEYBOARD_SHORTCUT_EVENT, &CameraPreferencePane::OnBackwardKeyChanged, this);
            m_leftKeyEditor->Bind(KEYBOARD_SHORTCUT_EVENT, &CameraPreferencePane::OnLeftKeyChanged, this);
            m_rightKeyEditor->Bind(KEYBOARD_SHORTCUT_EVENT, &CameraPreferencePane::OnRightKeyChanged, this);
        }
        
        bool CameraPreferencePane::doCanResetToDefaults() {
            return true;
        }
        
        void CameraPreferencePane::doResetToDefaults() {
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.resetToDefault(Preferences::CameraLookSpeed);
            prefs.resetToDefault(Preferences::CameraLookInvertH);
            prefs.resetToDefault(Preferences::CameraLookInvertV);
            
            prefs.resetToDefault(Preferences::CameraPanSpeed);
            prefs.resetToDefault(Preferences::CameraPanInvertH);
            prefs.resetToDefault(Preferences::CameraPanInvertV);
            
            prefs.resetToDefault(Preferences::CameraMoveSpeed);
            prefs.resetToDefault(Preferences::CameraEnableAltMove);
            prefs.resetToDefault(Preferences::CameraAltMoveInvert);
            prefs.resetToDefault(Preferences::CameraMoveInCursorDir);
            
            prefs.resetToDefault(Preferences::CameraFlySpeed);
            prefs.resetToDefault(Preferences::CameraFlyInvertV);
            
            prefs.resetToDefault(Preferences::CameraFlyForward);
            prefs.resetToDefault(Preferences::CameraFlyBackward);
            prefs.resetToDefault(Preferences::CameraFlyLeft);
            prefs.resetToDefault(Preferences::CameraLookSpeed);
            prefs.resetToDefault(Preferences::CameraFlyRight);
        }

        void CameraPreferencePane::doUpdateControls() {
            PreferenceManager& prefs = PreferenceManager::instance();
            
            m_lookSpeedSlider->SetValue(static_cast<int>(prefs.get(Preferences::CameraLookSpeed) * m_lookSpeedSlider->GetMax()));
            m_invertLookHAxisCheckBox->SetValue(prefs.get(Preferences::CameraLookInvertH));
            m_invertLookVAxisCheckBox->SetValue(prefs.get(Preferences::CameraLookInvertV));
            
            m_panSpeedSlider->SetValue(static_cast<int>(prefs.get(Preferences::CameraPanSpeed) * m_panSpeedSlider->GetMax()));
            m_invertPanHAxisCheckBox->SetValue(prefs.get(Preferences::CameraPanInvertH));
            m_invertPanVAxisCheckBox->SetValue(prefs.get(Preferences::CameraPanInvertV));
            
            m_moveSpeedSlider->SetValue(static_cast<int>(prefs.get(Preferences::CameraMoveSpeed) * m_moveSpeedSlider->GetMax()));
            m_enableAltMoveCheckBox->SetValue(prefs.get(Preferences::CameraEnableAltMove));
            m_invertAltMoveAxisCheckBox->SetValue(prefs.get(Preferences::CameraAltMoveInvert));
            m_moveInCursorDirCheckBox->SetValue(prefs.get(Preferences::CameraMoveInCursorDir));
            
            m_flySpeedSlider->SetValue(static_cast<int>(prefs.get(Preferences::CameraFlySpeed) * m_flySpeedSlider->GetMax()));
            m_invertFlyVAxisCheckBox->SetValue(prefs.get(Preferences::CameraFlyInvertV));
            
            m_forwardKeyEditor->SetShortcut(prefs.get(Preferences::CameraFlyForward));
            m_backwardKeyEditor->SetShortcut(prefs.get(Preferences::CameraFlyBackward));
            m_leftKeyEditor->SetShortcut(prefs.get(Preferences::CameraFlyLeft));
            m_rightKeyEditor->SetShortcut(prefs.get(Preferences::CameraFlyRight));
        }
        
        bool CameraPreferencePane::doValidate() {
            return true;
        }
	}
}
