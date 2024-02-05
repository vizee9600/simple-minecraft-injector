
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
using namespace std;

void rndmTitle() {
	constexpr int length = 15;
	const auto characters = TEXT("abcdefghi9182345jklmnopqrstuv211935960473wxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	TCHAR title[length + 1]{};

	for (int j = 0; j != length; j++)
	{
		title[j] += characters[rand() % 80];
	}

	SetConsoleTitleW(title);
}

int main(int argc, char** argv) {

	string path;

	cout << "Path to .DLL: " << endl;

	cin >> path;

	const char* dll_path = path.c_str();

	HANDLE snapshot = 0;
	PROCESSENTRY32 pe32 = { 0 };

	DWORD exitCode = 0;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(snapshot, &pe32);

	do {
		if (wcscmp(pe32.szExeFile, L"javaw.exe") == 0) {
			HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, true, pe32.th32ProcessID);
			
			void* lpBaseAddress = VirtualAllocEx(process, NULL, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);

			WriteProcessMemory(process, lpBaseAddress, dll_path, strlen(dll_path) + 1, NULL);

			HMODULE kernel32base = GetModuleHandle(L"kernel32.dll");
			HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32base, "LoadLibraryA"), lpBaseAddress, 0, NULL);

			std::cout << "injected";

			WaitForSingleObject(thread, INFINITE);
			GetExitCodeThread(thread, &exitCode);


			VirtualFreeEx(process, lpBaseAddress, 0, MEM_RELEASE);
			CloseHandle(thread);
			CloseHandle(process);
			break;
		}
	} while (Process32Next(snapshot, &pe32));
	return 0;
}
