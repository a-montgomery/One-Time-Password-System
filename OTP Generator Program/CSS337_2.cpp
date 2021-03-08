/*
Ulysess Steenmeyer and Andrew Montgomery.
CSS337, Assignment 2, SHA2 Program.
OTP Generator varient.


Framed after Charles Petzold's, 1998 Hexadecimal
Calculator example.

!!!IMPORTANT!!!
Compiles in Visual Studio 2019.
Make sure Project Properties>Linker>system>subsystem: is set to Windows, not console.
All files should be added to the solution explorer.
*/

#include <windows.h>
#include "sha256.h"

//Needs:
//afxres.h
//CSS337_2.dlg
//CSS337_2.ico
//CSS337_2.rc
//RESOURCE.h


//Function for convertering the SHA256 hashes into an OTP stored in a
//32bit container.
unsigned long Sha256toOTP6(const std::string& input)
{
	unsigned long output = 0;
	for (int i = 58; i < 64; ++i)//Access the last 6 chars in the full hash.
	{
		output = output << 4;//shift over hexadecimal characters.
		output += (input[i] < 'a' ? input[i] - '0' : input[i] - 'a' + 10);//add hexadecimal character with correct value.
	}

	return output;
}



//win32 setup.
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("CSS337_2");
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;    // Note!
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, szAppName);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateDialog(hInstance, szAppName, 0, NULL);

	ShowWindow(hwnd, iCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}



//responsible for displaying the current entered number.
void ShowNumber(HWND hwnd, UINT iNumber)
{
	TCHAR szBuffer[20];
	wsprintf(szBuffer, TEXT("%X"), iNumber);
	SetDlgItemText(hwnd, NULL, szBuffer);
}


//Main logic loop.------------------------------------------------------------------WndProc
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static UINT  iNumber;//Number in display.
	static const std::string HASH_SEED = "810770FF00FF07012";//Initial hash key.
	 
	HWND         hButton;//used for the visual button effect.


	//Set hashset[0] to the first hash, given a seed.
	static std::string hashCode{ sha256(HASH_SEED) };


	switch (message)
	{
	case WM_KEYDOWN:
		break;//essential, prevents odd, double press keyboard behavior.
//fall through.


	case WM_CHAR:


		//Provides a visual button effect when the keyboard is used.
		if (hButton = GetDlgItem(hwnd, wParam))
		{
			SendMessage(hButton, BM_SETSTATE, 1, 0);//press down visual.
			Sleep(100);
			SendMessage(hButton, BM_SETSTATE, 0, 0);//release visual.
		}
		else
		{//when an invalid key is pressed.
			break;
		}

	case WM_COMMAND:
		SetFocus(hwnd);



		if (LOWORD(wParam) == VK_RETURN) // If enter was pressed.
		{
		 ShowNumber(hwnd, (UINT)Sha256toOTP6(hashCode));//Diplay OTP.
		 hashCode = sha256(hashCode);//Generate a new hash.

		}
		return 0;//default, repeat logic loop.

		//Closure of window, using header button.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0; //Program return to system.
	}
	return DefWindowProc(hwnd, message, wParam, lParam);//windows message loop.
}


