#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include "Win32Utils.h"
#include <shobjidl.h>
#include <comdef.h>

class SaveFileDialog
{
public:

	SaveFileDialog()
	{
		// Initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		ThrowOnFailure(hr);

		// Create FileOpenDialog instance
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
		ThrowOnFailure(hr);

		// Configure dialog options
		FILEOPENDIALOGOPTIONS dwFlags;

		// Get current options in order not to override existing options.
		hr = pFileSave->GetOptions(&dwFlags);
		ThrowOnFailure(hr);

		// Restrict results to file system items
		hr = pFileSave->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
		ThrowOnFailure(hr);

		//TODO: need to populate extension box
		//fix the code below doesn't work
		LPCWSTR pszName = NULL;
		COMDLG_FILTERSPEC fileExtensions[] =
		{
			{ pszName, L"*.*" },
			{ pszName, L"*.txt}" }
		};
		pFileSave->SetFileTypes(2, fileExtensions);
		pFileSave->SetDefaultExtension(NULL);
	}

	~SaveFileDialog()
	{
		// Clean up FileOpenDialog instance
		pFileSave->Release();

		// Uninitialize COM
		CoUninitialize();
	}

	void Show()
	{
		// Show the Open dialog box.
		HRESULT hr = pFileSave->Show(NULL);
		ThrowOnFailure(hr);

		// Get selected item from the dialog box
		IShellItem *pItem;
		hr = pFileSave->GetResult(&pItem);
		ThrowOnFailure(hr);

		// Get filename from the selected item 
		PWSTR pszFilePath;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
		ThrowOnFailure(hr);

		// Convert filename to string and set it as a dialog result
		Result = Win32Utils::WideStringToString(pszFilePath);

		pItem->Release();
	}

	static string SaveFile()
	{
		try
		{
			SaveFileDialog dialog;
			dialog.Show();

			return dialog.Result;
		}
		catch (...)
		{
			return "";
		}
	}

	std::string Result;

private:

	void ThrowOnFailure(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw _com_error(hr);
		}
	}

	IFileSaveDialog *pFileSave;
};