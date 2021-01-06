#include <windows.h>
#include <stdio.h>
#include <conio.h>

void fullscreen() {
	/*keybd_event(VK_MENU, 0x38, 0, 0);
	keybd_event(VK_RETURN, 0x1c, 0, 0);
	keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
	*/
	HWND handle = GetConsoleWindow();
	ShowWindow(handle, SW_SHOWMAXIMIZED);
}

void hideCursor()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void setCoordinates(int x, int y) {
	COORD cursorPos;
	cursorPos.X = x;
	cursorPos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
}

int calcObjectWidth(const unsigned char* object) {
	int size = 0;
	int i = 0;
	while (object[i] != '\n') {
		size++;
		i++;
	} size++;
	return size;
}

void getConsoleSize(int* width, int* height) {
	//int size = calcObjectWidth(object);
	COORD screenData = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));
	*width = screenData.X;
	*height = screenData.Y;
	//*x = (width - size) / 2;
	//*y = height / 2;
}

void printObject(const unsigned char* object, int x, int y, int* xOld, int* yOld) {

	int i = 0;
	*xOld = x;
	*yOld = y;
	setCoordinates(x, y);
	while (object[i] != '\0') {
		char r = object[i];
		putchar(object[i]);
		fflush(stdout);
		if (object[i] == '\n') {
			y++;
			setCoordinates(x, y);
		}
		i++;
	}
}

void clear(const unsigned char* object, int x, int y)
{
	setCoordinates(x, y);
	int i = 0;
	while (object[i] != '\0') {
		char r = object[i];
		putchar(object[i]);
		fflush(stdout);
		if (object[i] == '\n') {
			y++;
			setCoordinates(x, y);
		}
		i++;
	}
}

int getKeyboardInput() {
	if (_kbhit()) {
		switch (_getch()) {

			/*case 'w':
				//strzalka w gore
				return 3;
				break;
			case 's':
				//strzalka w dol
				return 4;
				break;
				*/

		case 'a':
			//strzalka w lewo
			return 1;
			break;
		case 'd':
			//strzalka w prawo
			return 2;
			break;
		case 'f':
			//spacja
			return 5;
			break;
		default:
			return 0;
			break;
		}
	}
}

void initializeConsole() {
	fullscreen();
	hideCursor();
	//centerConsoleBuffer(ship, x, y);
}