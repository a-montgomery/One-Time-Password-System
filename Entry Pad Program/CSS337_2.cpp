/*
Ulysess Steenmeyer and Andrew Montgomery.
CSS337, Assignment 2, SHA2 Program. 
//Entry pad varient. 

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

//Used for Writing "ERROR" to the display area.
void ShowError(HWND hwnd)
{
	static const TCHAR szERROR[] = { 'E','R','R','O','R',0 };
	SetDlgItemText(hwnd, NULL, szERROR);
}

//Used for Writing "MATCH" to the display area.
void ShowMatch(HWND hwnd)
{
	static const TCHAR szMATCH[] = { 'M','A','T','C','H',0 };
	SetDlgItemText(hwnd, NULL, szMATCH);
}

//Main logic loop.------------------------------------------------------------------WndProc
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static UINT  iNumber;//Number in display.
	static unsigned char chCurrentHashIndex = 0;//Current index for an array of hashes.
	static bool bFirstTime = true;//flag to prevent regenerating the hash array.

	static const std::string HASH_SEED = "810770FF00FF07012";//Initial hash key.
	static const unsigned char SYNC_SIZE = 20;//Number of OTPs to look ahead for.
	 
	HWND         hButton;//used for the visual button effect.


	//Set hashset[0] to the first hash, given a seed.
	static std::string hashSet[SYNC_SIZE]{ sha256(HASH_SEED) };

	//Calculate the first 20 hashes for resync/look ahead.
	if (bFirstTime) //Done once, and never again.
	{
		for (int i = 0; i < SYNC_SIZE - 1; ++i)
		{
			hashSet[i + 1] = sha256(hashSet[i]);
		}
	}
	bFirstTime = false;


	switch (message)
	{


	case WM_KEYDOWN:
		break;//essential, prevents odd, double press keyboard behavior.



	case WM_CHAR:
		//Raise all lowercase input from keyboard to uppercase.
		wParam = (WPARAM)CharUpper((TCHAR*)wParam);


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

		//Pressing backspace...Remove least value Hexadecimal character from entry.
		if (LOWORD(wParam) == VK_BACK) ShowNumber(hwnd, iNumber /= 16); 

		//Escape press...Clears the display.
		else if (LOWORD(wParam) == VK_ESCAPE) ShowNumber(hwnd, iNumber = 0); // escape resets entry number.

		else if (isxdigit(LOWORD(wParam)))//If entry was a hexadecimal character...
		{//...Attempt to add number.

			if (iNumber <= 0xFFFFF)//if space is still available...(input is limited to 6, base 16 digits)
			{//Add new digit as least significant figure.
				iNumber = iNumber << 4;//shift display one hexadecimal unit.
				iNumber += (isdigit(wParam) ? wParam - '0' : wParam - 'A' + 10);//add entry as least significant digit.
				ShowNumber(hwnd, iNumber);//Update display.
			}

		}

		else if (LOWORD(wParam) == VK_RETURN) // If enter was pressed.
		{
			//Check for matching values in the hash array.
			for (int i = 0; i < SYNC_SIZE; ++i)
			{
				int index = (i + chCurrentHashIndex) % 20; //% wraps around for array bounds.
				UINT iChecker = Sha256toOTP6( hashSet[ index ] ); //get OTP from examining hash.
				if (iChecker == iNumber)//if match discovered.
				{//...Confirm match and update hash array.
					ShowMatch(hwnd);//confirm match.
					for (; i >= 0; --i)
					{//update hash array based on resync needs.
						hashSet[chCurrentHashIndex] = sha256(hashSet[ (chCurrentHashIndex + 19) %20] );
						chCurrentHashIndex = (chCurrentHashIndex + 1) % 20; //increment hashIndex.
					}

					goto MATCHSKIP;//avoid displaying error to display.
				}

			}
			ShowError(hwnd);

MATCHSKIP:
			iNumber = 0;//clear the entered value, prevents use of backspace after match/mismatch.
		}
		return 0;//default, repeat logic loop.

		//Closure of window, using header button.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0; //Program return to system.
	}
	return DefWindowProc(hwnd, message, wParam, lParam);//windows message loop.
}


