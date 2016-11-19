#pragma once

#include "Win32Utils.h"
#include <shobjidl.h>
#include <comdef.h>
#include <string>

using std::string;

class OpenFileDialog
{
public:

	OpenFileDialog()
	{
		// Initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		ThrowOnFailure(hr);

		// Create FileOpenDialog instance
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		ThrowOnFailure(hr);

		// Configure dialog options

		FILEOPENDIALOGOPTIONS dwFlags;

		// Get current options in order not to override existing options.
		hr = pFileOpen->GetOptions(&dwFlags);
		ThrowOnFailure(hr);

		// Restrict results to file system items
		hr = pFileOpen->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
		ThrowOnFailure(hr);
	}

	char currentDirectory[100];

	~OpenFileDialog()
	{
		// Clean up FileOpenDialog instance
		pFileOpen->Release();

		// Uninitialize COM
		CoUninitialize();
	}

	void Show()
	{
		// Show the Open dialog box.
		HRESULT hr = pFileOpen->Show(NULL);
		ThrowOnFailure(hr);

		// Get selected item from the dialog box
		IShellItem *pItem;
		hr = pFileOpen->GetResult(&pItem);
		ThrowOnFailure(hr);

		// Get filename from the selected item 
		PWSTR pszFilePath;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
		ThrowOnFailure(hr);

		// Convert filename to string and set it as a dialog result
		Result = Win32Utils::WideStringToString(pszFilePath);

		pItem->Release();
	}

	static string SelectFile()
	{
		try
		{
			OpenFileDialog dialog;
			dialog.Show();

			return dialog.Result;
		}
		catch (...)
		{			
			return "";
		}
	}

	string Result;

private:

	void ThrowOnFailure(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw _com_error(hr);
		}
	}

	IFileOpenDialog *pFileOpen;
};

