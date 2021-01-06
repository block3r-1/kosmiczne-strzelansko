#include <conio.h>

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