// DeltaWxWidgetsLib.cpp
// Wx Widgets library for Delta.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#include "DeltaWxWidgetsLib.h"
#include "DeltaWxWidgets.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
//

////////////////////////////////////////////////////////////////

static DeltaObject* api = (DeltaObject*) 0;
DELTA_LIBRARY_GETAPI_IMPL(wx_getapi,api)

////////////////////////////////////////////////////////////////

DWXWIDGETS_CFUNC void* Install (void) {
	wxWidgets::DefineInit();

	api = DeltaObject::NewObject();
	DPTR(api)->IncRefCounter((DeltaValue*) 0);
	DeltaLibFuncBinder::Add("wx_getapi", wx_getapi_LibFunc);

	Install_DeltaWxGlobalFunctions_Lib(api);
	Install_DeltaWxObject_Lib(api);
	Install_DeltaWxEvtHandler_Lib(api);
	Install_DeltaWxWindow_Lib(api);
	Install_DeltaWxAppAdapter_Lib(api);
	Install_DeltaWxTopLevelWindow_Lib(api);
	Install_DeltaWxFrame_Lib(api);
	Install_DeltaWxSize_Lib(api);
	Install_DeltaWxPoint_Lib(api);
	Install_DeltaWxAcceleratorTable_Lib(api);
	Install_DeltaWxCaret_Lib(api);
	Install_DeltaWxIndividualLayoutConstraint_Lib(api);
	Install_DeltaWxLayoutConstraints_Lib(api);
	Install_DeltaWxAcceleratorEntry_Lib(api);
	Install_DeltaWxColour_Lib(api);
	Install_DeltaWxFont_Lib(api);
	Install_DeltaWxDropTarget_Lib(api);
	Install_DeltaWxObjectRefData_Lib(api);
	Install_DeltaWxClassInfo_Lib(api);
	Install_DeltaWxEvent_Lib(api);
	Install_DeltaWxRect_Lib(api);
	Install_DeltaWxStatusBar_Lib(api);
	Install_DeltaWxMenuBar_Lib(api);
	Install_DeltaWxMenu_Lib(api);
	Install_DeltaWxMenuItem_Lib(api);
	Install_DeltaWxCommandEvent_Lib(api);
	Install_DeltaWxNotifyEvent_Lib(api);
	Install_DeltaWxControl_Lib(api);
	Install_DeltaWxButton_Lib(api);
	Install_DeltaWxSizerFlags_Lib(api);
	Install_DeltaWxSizerItem_Lib(api);
	Install_DeltaWxSizer_Lib(api);
	Install_DeltaWxBoxSizer_Lib(api);
	Install_DeltaWxDialog_Lib(api);
	Install_DeltaWxProgressDialog_Lib(api);
	Install_DeltaWxNotebook_Lib(api);
	Install_DeltaWxNotebookEvent_Lib(api);
	Install_DeltaWxUpdateUIEvent_Lib(api);
	Install_DeltaWxVisualAttributes_Lib(api);
	Install_DeltaWxRegion_Lib(api);
	Install_DeltaWxControlWithItems_Lib(api);
	Install_DeltaWxComboBox_Lib(api);
	Install_DeltaWxToolTip_Lib(api);
	Install_DeltaWxRadioButton_Lib(api);
	Install_DeltaWxScrollEvent_Lib(api);
	Install_DeltaWxScrollWinEvent_Lib(api);
	Install_DeltaWxStaticBox_Lib(api);
	Install_DeltaWxRadioBox_Lib(api);
	Install_DeltaWxFileDialog_Lib(api);
	Install_DeltaWxLog_Lib(api);
	Install_DeltaWxLogChain_Lib(api);
	Install_DeltaWxLogWindow_Lib(api);
	Install_DeltaWxMessageDialog_Lib(api);
	Install_DeltaWxTextEntryDialog_Lib(api);
	Install_DeltaWxKeyEvent_Lib(api);
	Install_DeltaWxDropFilesEvent_Lib(api);
	Install_DeltaWxTextAttr_Lib(api);
	Install_DeltaWxTextCtrl_Lib(api);
	Install_DeltaWxSysColourChangedEvent_Lib(api);
	Install_DeltaWxPanel_Lib(api);
	Install_DeltaWxDC_Lib(api);
	Install_DeltaWxPalette_Lib(api);
	Install_DeltaWxBitmap_Lib(api);
	Install_DeltaWxToolBar_Lib(api);
	Install_DeltaWxToolBarToolBase_Lib(api);
	Install_DeltaWxIconLocation_Lib(api);
	Install_DeltaWxIcon_Lib(api);
	Install_DeltaWxBitmapButton_Lib(api);
	Install_DeltaWxWizard_Lib(api);
	Install_DeltaWxWizardPage_Lib(api);
	Install_DeltaWxWizardPageSimple_Lib(api);
	Install_DeltaWxWizardEvent_Lib(api);
	Install_DeltaWxFontData_Lib(api);
	Install_DeltaWxFontDialog_Lib(api);
	Install_DeltaWxColourData_Lib(api);
	Install_DeltaWxColourDialog_Lib(api);
	Install_DeltaWxCheckBox_Lib(api);
	Install_DeltaWxGauge_Lib(api);
	Install_DeltaWxImageList_Lib(api);
	Install_DeltaWxScrolledWindow_Lib(api);
	Install_DeltaWxChoice_Lib(api);
	Install_DeltaWxListBox_Lib(api);
	Install_DeltaWxCheckListBox_Lib(api);
	Install_DeltaWxSplashScreen_Lib(api);
	Install_DeltaWxCloseEvent_Lib(api);
	Install_DeltaWxBrush_Lib(api);
	Install_DeltaWxCursor_Lib(api);
	Install_DeltaWxPen_Lib(api);
	Install_DeltaWxFindReplaceData_Lib(api);
	Install_DeltaWxFindReplaceDialog_Lib(api);
	Install_DeltaWxPrintData_Lib(api);
	Install_DeltaWxPageSetupDialogData_Lib(api);
	Install_DeltaWxFindDialogEvent_Lib(api);
	Install_DeltaWxPageSetupDialog_Lib(api);
	Install_DeltaWxPrintDialogData_Lib(api);
	Install_DeltaWxPrintDialog_Lib(api);
	Install_DeltaWxPasswordEntryDialog_Lib(api);
	Install_DeltaWxDirDialog_Lib(api);
	Install_DeltaWxMultiChoiceDialog_Lib(api);
	Install_DeltaWxSingleChoiceDialog_Lib(api);
	Install_DeltaWxCalendarDateAttr_Lib(api);
	Install_DeltaWxDateTime_Lib(api);
	Install_DeltaWxCalendarCtrl_Lib(api);
	Install_DeltaWxDateEvent_Lib(api);
	Install_DeltaWxCalendarEvent_Lib(api);
	Install_DeltaWxDatePickerCtrl_Lib(api);
	Install_DeltaWxGenericDirCtrl_Lib(api);
	Install_DeltaWxDirFilterListCtrl_Lib(api);
	Install_DeltaWxListbook_Lib(api);
	Install_DeltaWxListbookEvent_Lib(api);
	Install_DeltaWxTreebook_Lib(api);
	Install_DeltaWxTreebookEvent_Lib(api);
	Install_DeltaWxStaticText_Lib(api);
	Install_DeltaWxSlider_Lib(api);
	Install_DeltaWxStaticLine_Lib(api);
	Install_DeltaWxStaticBoxSizer_Lib(api);
	Install_DeltaWxChoicebook_Lib(api);
	Install_DeltaWxChoicebookEvent_Lib(api);
	Install_DeltaWxTreeItemId_Lib(api);
	Install_DeltaWxTreeItemData_Lib(api);
	Install_DeltaWxTreeCtrl_Lib(api);
	Install_DeltaWxTreeEvent_Lib(api);
	Install_DeltaWxToggleButton_Lib(api);
	Install_DeltaWxSpinButton_Lib(api);
	Install_DeltaWxSpinCtrl_Lib(api);
	Install_DeltaWxSpinEvent_Lib(api);
	Install_DeltaWxDateSpan_Lib(api);
	Install_DeltaWxTimeSpan_Lib(api);
	Install_DeltaWxListItemAttr_Lib(api);
	Install_DeltaWxListItem_Lib(api);
	Install_DeltaWxListCtrl_Lib(api);
	Install_DeltaWxAboutDialogInfo_Lib(api);
	Install_DeltaWxValidator_Lib(api);
	Install_DeltaWxGenericValidator_Lib(api);
	Install_DeltaWxTextValidator_Lib(api);
	Install_DeltaWxNumberEntryDialog_Lib(api);
	Install_DeltaWxListView_Lib(api);
	Install_DeltaWxIconBundle_Lib(api);
	Install_DeltaWxMask_Lib(api);
	Install_DeltaWxImage_Lib(api);
	Install_DeltaWxStdDialogButtonSizer_Lib(api);
	Install_DeltaWxWindowDC_Lib(api);
	Install_DeltaWxClientDC_Lib(api);
	Install_DeltaWxPaintDC_Lib(api);
	Install_DeltaWxSplitterWindow_Lib(api);
	Install_DeltaWxSplitterEvent_Lib(api);
	Install_DeltaWxGridSizer_Lib(api);
	Install_DeltaWxFlexGridSizer_Lib(api);
	Install_DeltaWxSearchCtrl_Lib(api);
	Install_DeltaWxPickerBase_Lib(api);
	Install_DeltaWxColourPickerCtrl_Lib(api);
	Install_DeltaWxColourPickerEvent_Lib(api);
	Install_DeltaWxDirPickerCtrl_Lib(api);
	Install_DeltaWxFilePickerCtrl_Lib(api);
	Install_DeltaWxFileDirPickerEvent_Lib(api);
	Install_DeltaWxFontPickerCtrl_Lib(api);
	Install_DeltaWxFontPickerEvent_Lib(api);
	Install_DeltaWxMenuEvent_Lib(api);
	Install_DeltaWxMouseEvent_Lib(api);
	Install_DeltaWxMoveEvent_Lib(api);
	Install_DeltaWxSizeEvent_Lib(api);
	Install_DeltaWxWindowCreateEvent_Lib(api);
	Install_DeltaWxWindowDestroyEvent_Lib(api);
	Install_DeltaWxCollapsiblePane_Lib(api);
	Install_DeltaWxCollapsiblePaneEvent_Lib(api);
	Install_DeltaWxSetCursorEvent_Lib(api);
	Install_DeltaWxFocusEvent_Lib(api);
	Install_DeltaWxChildFocusEvent_Lib(api);
	Install_DeltaWxActivateEvent_Lib(api);
	Install_DeltaWxInitDialogEvent_Lib(api);
	Install_DeltaWxShowEvent_Lib(api);
	Install_DeltaWxIconizeEvent_Lib(api);
	Install_DeltaWxMaximizeEvent_Lib(api);
	Install_DeltaWxMouseCaptureChangedEvent_Lib(api);
	Install_DeltaWxMouseCaptureLostEvent_Lib(api);
	Install_DeltaWxDisplayChangedEvent_Lib(api);
	Install_DeltaWxPaletteChangedEvent_Lib(api);
	Install_DeltaWxQueryNewPaletteEvent_Lib(api);
	Install_DeltaWxJoystickEvent_Lib(api);
	Install_DeltaWxNavigationKeyEvent_Lib(api);
	Install_DeltaWxHelpEvent_Lib(api);
	Install_DeltaWxClipboardTextEvent_Lib(api);
	Install_DeltaWxContextMenuEvent_Lib(api);
	Install_DeltaWxIdleEvent_Lib(api);
	Install_DeltaWxJoystick_Lib(api);
	Install_DeltaWxMemoryDC_Lib(api);
	Install_DeltaWxBufferedDC_Lib(api);
	Install_DeltaWxEraseEvent_Lib(api);
	Install_DeltaWxTimer_Lib(api);
	Install_DeltaWxTimerEvent_Lib(api);
	Install_DeltaWxToolbook_Lib(api);
	Install_DeltaWxToolbookEvent_Lib(api);
	return udynamiclibloader::New("CleanUp", true);	 // Explicitly supply a cleaner.
}

DWXWIDGETS_CFUNC void* CleanUp (void) {
	wxWidgets::DefineExit();

	CleanUp_DeltaWxGlobalFunctions_Lib();
	CleanUp_DeltaWxFrame_Lib();
	CleanUp_DeltaWxAppAdapter_Lib();
	CleanUp_DeltaWxWindow_Lib();
	CleanUp_DeltaWxSize_Lib();
	CleanUp_DeltaWxPoint_Lib();
	CleanUp_DeltaWxAcceleratorTable_Lib();
	CleanUp_DeltaWxCaret_Lib();
	CleanUp_DeltaWxIndividualLayoutConstraint_Lib();
	CleanUp_DeltaWxLayoutConstraints_Lib();
	CleanUp_DeltaWxAcceleratorEntry_Lib();
	CleanUp_DeltaWxFont_Lib();
	CleanUp_DeltaWxDropTarget_Lib();
	CleanUp_DeltaWxObject_Lib();
	CleanUp_DeltaWxObjectRefData_Lib();
	CleanUp_DeltaWxClassInfo_Lib();
	CleanUp_DeltaWxEvent_Lib();
	CleanUp_DeltaWxRect_Lib();
	CleanUp_DeltaWxStatusBar_Lib();
	CleanUp_DeltaWxMenuBar_Lib();
	CleanUp_DeltaWxMenu_Lib();
	CleanUp_DeltaWxMenuItem_Lib();
	CleanUp_DeltaWxEvtHandler_Lib();
	CleanUp_DeltaWxCommandEvent_Lib();
	CleanUp_DeltaWxColour_Lib();
	CleanUp_DeltaWxNotifyEvent_Lib();
	CleanUp_DeltaWxControl_Lib();
	CleanUp_DeltaWxButton_Lib();
	CleanUp_DeltaWxSizerFlags_Lib();
	CleanUp_DeltaWxSizerItem_Lib();
	CleanUp_DeltaWxSizer_Lib();
	CleanUp_DeltaWxBoxSizer_Lib();
	CleanUp_DeltaWxTopLevelWindow_Lib();
	CleanUp_DeltaWxDialog_Lib();
	CleanUp_DeltaWxProgressDialog_Lib();
	CleanUp_DeltaWxNotebook_Lib();
	CleanUp_DeltaWxNotebookEvent_Lib();
	CleanUp_DeltaWxUpdateUIEvent_Lib();
	CleanUp_DeltaWxVisualAttributes_Lib();
	CleanUp_DeltaWxRegion_Lib();
	CleanUp_DeltaWxComboBox_Lib();
	CleanUp_DeltaWxToolTip_Lib();
	CleanUp_DeltaWxRadioButton_Lib();
	CleanUp_DeltaWxScrollEvent_Lib();
	CleanUp_DeltaWxScrollWinEvent_Lib();
	CleanUp_DeltaWxRadioBox_Lib();
	CleanUp_DeltaWxFileDialog_Lib();
	CleanUp_DeltaWxLog_Lib();
	CleanUp_DeltaWxLogChain_Lib();
	CleanUp_DeltaWxLogWindow_Lib();
	CleanUp_DeltaWxMessageDialog_Lib();
	CleanUp_DeltaWxTextEntryDialog_Lib();
	CleanUp_DeltaWxKeyEvent_Lib();
	CleanUp_DeltaWxDropFilesEvent_Lib();
	CleanUp_DeltaWxTextAttr_Lib();
	CleanUp_DeltaWxTextCtrl_Lib();
	CleanUp_DeltaWxSysColourChangedEvent_Lib();
	CleanUp_DeltaWxPanel_Lib();
	CleanUp_DeltaWxDC_Lib();
	CleanUp_DeltaWxPalette_Lib();
	CleanUp_DeltaWxBitmap_Lib();
	CleanUp_DeltaWxToolBar_Lib();
	CleanUp_DeltaWxToolBarToolBase_Lib();
	CleanUp_DeltaWxIconLocation_Lib();
	CleanUp_DeltaWxIcon_Lib();
	CleanUp_DeltaWxBitmapButton_Lib();
	CleanUp_DeltaWxWizard_Lib();
	CleanUp_DeltaWxWizardPage_Lib();
	CleanUp_DeltaWxWizardPageSimple_Lib();
	CleanUp_DeltaWxWizardEvent_Lib();
	CleanUp_DeltaWxFontData_Lib();
	CleanUp_DeltaWxFontDialog_Lib();
	CleanUp_DeltaWxColourData_Lib();
	CleanUp_DeltaWxColourDialog_Lib();
	CleanUp_DeltaWxCheckBox_Lib();
	CleanUp_DeltaWxGauge_Lib();
	CleanUp_DeltaWxImageList_Lib();
	CleanUp_DeltaWxScrolledWindow_Lib();
	CleanUp_DeltaWxControlWithItems_Lib();
	CleanUp_DeltaWxChoice_Lib();
	CleanUp_DeltaWxListBox_Lib();
	CleanUp_DeltaWxCheckListBox_Lib();
	CleanUp_DeltaWxSplashScreen_Lib();
	CleanUp_DeltaWxCloseEvent_Lib();
	CleanUp_DeltaWxBrush_Lib();
	CleanUp_DeltaWxCursor_Lib();
	CleanUp_DeltaWxPen_Lib();
	CleanUp_DeltaWxFindReplaceData_Lib();
	CleanUp_DeltaWxFindReplaceDialog_Lib();
	CleanUp_DeltaWxPrintData_Lib();
	CleanUp_DeltaWxPageSetupDialogData_Lib();
	CleanUp_DeltaWxFindDialogEvent_Lib();
	CleanUp_DeltaWxPageSetupDialog_Lib();
	CleanUp_DeltaWxPrintDialogData_Lib();
	CleanUp_DeltaWxPrintDialog_Lib();
	CleanUp_DeltaWxPasswordEntryDialog_Lib();
	CleanUp_DeltaWxDirDialog_Lib();
	CleanUp_DeltaWxMultiChoiceDialog_Lib();
	CleanUp_DeltaWxSingleChoiceDialog_Lib();
	CleanUp_DeltaWxCalendarDateAttr_Lib();
	CleanUp_DeltaWxDateTime_Lib();
	CleanUp_DeltaWxCalendarCtrl_Lib();
	CleanUp_DeltaWxDateEvent_Lib();
	CleanUp_DeltaWxCalendarEvent_Lib();
	CleanUp_DeltaWxDatePickerCtrl_Lib();
	CleanUp_DeltaWxGenericDirCtrl_Lib();
	CleanUp_DeltaWxDirFilterListCtrl_Lib();
	CleanUp_DeltaWxListbook_Lib();
	CleanUp_DeltaWxListbookEvent_Lib();
	CleanUp_DeltaWxTreebook_Lib();
	CleanUp_DeltaWxTreebookEvent_Lib();
	CleanUp_DeltaWxStaticText_Lib();
	CleanUp_DeltaWxSlider_Lib();
	CleanUp_DeltaWxStaticBox_Lib();
	CleanUp_DeltaWxStaticLine_Lib();
	CleanUp_DeltaWxStaticBoxSizer_Lib();
	CleanUp_DeltaWxChoicebook_Lib();
	CleanUp_DeltaWxChoicebookEvent_Lib();
	CleanUp_DeltaWxTreeItemId_Lib();
	CleanUp_DeltaWxTreeItemData_Lib();
	CleanUp_DeltaWxTreeCtrl_Lib();
	CleanUp_DeltaWxTreeEvent_Lib();
	CleanUp_DeltaWxToggleButton_Lib();
	CleanUp_DeltaWxSpinButton_Lib();
	CleanUp_DeltaWxSpinCtrl_Lib();
	CleanUp_DeltaWxSpinEvent_Lib();
	CleanUp_DeltaWxDateSpan_Lib();
	CleanUp_DeltaWxTimeSpan_Lib();
	CleanUp_DeltaWxListItemAttr_Lib();
	CleanUp_DeltaWxListItem_Lib();
	CleanUp_DeltaWxListCtrl_Lib();
	CleanUp_DeltaWxAboutDialogInfo_Lib();
	CleanUp_DeltaWxValidator_Lib();
	CleanUp_DeltaWxGenericValidator_Lib();
	CleanUp_DeltaWxTextValidator_Lib();
	CleanUp_DeltaWxNumberEntryDialog_Lib();
	CleanUp_DeltaWxListView_Lib();
	CleanUp_DeltaWxIconBundle_Lib();
	CleanUp_DeltaWxMask_Lib();
	CleanUp_DeltaWxImage_Lib();
	CleanUp_DeltaWxStdDialogButtonSizer_Lib();
	CleanUp_DeltaWxWindowDC_Lib();
	CleanUp_DeltaWxClientDC_Lib();
	CleanUp_DeltaWxPaintDC_Lib();
	CleanUp_DeltaWxSplitterWindow_Lib();
	CleanUp_DeltaWxSplitterEvent_Lib();
	CleanUp_DeltaWxGridSizer_Lib();
	CleanUp_DeltaWxFlexGridSizer_Lib();
	CleanUp_DeltaWxSearchCtrl_Lib();
	CleanUp_DeltaWxPickerBase_Lib();
	CleanUp_DeltaWxColourPickerCtrl_Lib();
	CleanUp_DeltaWxColourPickerEvent_Lib();
	CleanUp_DeltaWxDirPickerCtrl_Lib();
	CleanUp_DeltaWxFilePickerCtrl_Lib();
	CleanUp_DeltaWxFileDirPickerEvent_Lib();
	CleanUp_DeltaWxFontPickerCtrl_Lib();
	CleanUp_DeltaWxFontPickerEvent_Lib();
	CleanUp_DeltaWxMenuEvent_Lib();
	CleanUp_DeltaWxMouseEvent_Lib();
	CleanUp_DeltaWxMoveEvent_Lib();
	CleanUp_DeltaWxSizeEvent_Lib();
	CleanUp_DeltaWxWindowCreateEvent_Lib();
	CleanUp_DeltaWxWindowDestroyEvent_Lib();
	CleanUp_DeltaWxCollapsiblePane_Lib();
	CleanUp_DeltaWxCollapsiblePaneEvent_Lib();
	CleanUp_DeltaWxSetCursorEvent_Lib();
	CleanUp_DeltaWxFocusEvent_Lib();
	CleanUp_DeltaWxChildFocusEvent_Lib();
	CleanUp_DeltaWxActivateEvent_Lib();
	CleanUp_DeltaWxInitDialogEvent_Lib();
	CleanUp_DeltaWxShowEvent_Lib();
	CleanUp_DeltaWxIconizeEvent_Lib();
	CleanUp_DeltaWxMaximizeEvent_Lib();
	CleanUp_DeltaWxMouseCaptureChangedEvent_Lib();
	CleanUp_DeltaWxMouseCaptureLostEvent_Lib();
	CleanUp_DeltaWxDisplayChangedEvent_Lib();
	CleanUp_DeltaWxPaletteChangedEvent_Lib();
	CleanUp_DeltaWxQueryNewPaletteEvent_Lib();
	CleanUp_DeltaWxJoystickEvent_Lib();
	CleanUp_DeltaWxNavigationKeyEvent_Lib();
	CleanUp_DeltaWxHelpEvent_Lib();
	CleanUp_DeltaWxClipboardTextEvent_Lib();
	CleanUp_DeltaWxContextMenuEvent_Lib();
	CleanUp_DeltaWxIdleEvent_Lib();
	CleanUp_DeltaWxJoystick_Lib();
	CleanUp_DeltaWxMemoryDC_Lib();
	CleanUp_DeltaWxBufferedDC_Lib();
	CleanUp_DeltaWxEraseEvent_Lib();
	CleanUp_DeltaWxTimer_Lib();
	CleanUp_DeltaWxTimerEvent_Lib();
	CleanUp_DeltaWxToolbook_Lib();
	CleanUp_DeltaWxToolbookEvent_Lib();

	DeltaLibraryObjectCreator::DestroyAPI(api);
	return udynamiclibloader::New("dll cleared");
}

////////////////////////////////////////////////////////////////
