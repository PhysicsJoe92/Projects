#include <iostream>
#include <thread>
#include <cstdlib>
#include <time.h>
#include <vector>

using namespace std;

#include <stdio.h>
#include <windows.h>

int nScreenWidth = 160;
int nScreenHeight = 100;
wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;


int Rotate(int px, int py, int r) {
	int pi = 0;
	switch (r % 4) {
		case 0: // 0 deg					//	0	1	2	3
			pi = py * 4 + px;				//	4	5	6	7
			break;							//	8	9	10	11
											//	12	13	14	15

		case 1: // 90 deg					//	12	8	4	0
			pi = 12 + py - (px * 4);		//	13	9	5	1
			break;							//	14	10	6	2
											//	15	11	7	3


		case 2: // 180 deg					//	15	14	13	12
			pi = 15 - (py * 4) - px;		//	11	10	9	8
			break;							//	7	6	5	4
											//	3	2	1	0


		case 3: // 270 deg					//	3	7	11	15
			pi = 3 - py + (px * 4);			//	2	6	10	14
			break;							//	1	5	9	13
											//	0	4	8	12
	}
	return pi;
}

bool DoesPeiceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
	for (int px = 0; px < 4; px++) {
		for (int py = 0; py < 4; py++) {
			// Get index into piece
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);


			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
					if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0) return false;
				}
			}
		}
	}

	return true;
}

int main() {
	// Seed
	srand((unsigned int)time(0));

	//Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;


	//Create assets
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L"..X.");
	tetromino[2].append(L"..XX");
	tetromino[2].append(L"...X");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L"..XX");
	tetromino[3].append(L"..XX");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L"..XX");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");

	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++) {
		for (int y = 0; y < nFieldHeight; y++) {
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
		}
	}

	// Game Logic Stuff
	int nCurrentPiece = rand() % 7;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;

	bool bGameOver = false;


	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;


	vector<int> vLines;

	while (!bGameOver) {

		// GAME TIMING ============================================
		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = ((nSpeedCounter % 20 + 1) == nSpeed);

		// INPUT ==================================================
		for (int k = 0; k < 4; k++) {
			bKey[k] = (GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
		}

		// GAME LOGIC =============================================
		nCurrentX += (bKey[0] && DoesPeiceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && DoesPeiceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && DoesPeiceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		if (bKey[3]) {
			nCurrentRotation += (!bRotateHold && DoesPeiceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
			bRotateHold = false;


		if (bForceDown) {
			if (DoesPeiceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) nCurrentY++;
			else {
				// Lock the current piece on the field
				for (int px = 0; px < 4; px++) {
					for (int py = 0; py < 4; py++) {
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
							nSpeedCounter = 0;
						}
					}
				}

				nPieceCount++;
				if (nPieceCount % 1 == 0) {
					if (nSpeed >= 10)nSpeed--;
				}

				// Check have we got any lines
				for (int py = 0; py < 4; py++) {
					if (nCurrentY + py < nFieldHeight - 1) {
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++) {
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
						}
						if (bLine) {
							for (int px = 1; px < nFieldWidth - 1; px++) {
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							}
							vLines.push_back(nCurrentY + py);
						}
					}
				}

				nScore += 25;
				if (!vLines.empty())nScore += (1 << vLines.size()) * 100;

				// Choose next piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				// if piece does not fit
				bGameOver = !DoesPeiceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
		}
		// RENDER OUTPUT ==========================================


		//Draw Field
		for (int x = 0; x < nFieldWidth; x++) {
			for (int y = 0; y < nFieldHeight; y++) {
				screen[(y + 10) * nScreenWidth + (x + 32)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
			}
		}

		// Draw Current Piece
		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					screen[(nCurrentY + py + 10) * nScreenWidth + (nCurrentX + px + 32)] = nCurrentPiece + 65;
			}
		}

		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		if (!vLines.empty()) {
			//Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms);	// Delay a bit

			for (int v : vLines) {
				for (int px = 1; px < nFieldWidth - 1; px++) {
					for (int py = v; py > 0; py--) {
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					}
					pField[px] = 0;
				}
			}
			vLines.clear();
		}
		// Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	CloseHandle(hConsole);
	cout << "Game Over!! Score: " << nScore << endl;
	system("pause");

	return 0;
}