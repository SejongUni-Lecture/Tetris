#include<stdio.h>
#include<windows.h>
#include<time.h>
#include"block.h"

#define LEFT 75
#define RIGHT 77
#define UP 72

//Size of gameboard
#define GBOARD_WIDTH 10
#define GBOARD_HEIGHT 20

//Starting point of gameboard
#define GBOARD_ORIGIN_X 5
#define GBOARD_ORIGIN_Y 2

int block_id;
int speed = 10;
int gameBoardInfo[GBOARD_HEIGHT + 1][GBOARD_WIDTH + 2];

COORD curPos = { 10,0 };

void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetCurrentCursorPos(void)
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;
	return curPoint;
}

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void ShowBlock(char blockInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			if (blockInfo[y][x] == 1)
				printf("■");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DeleteBlock(char blockInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
			if (blockInfo[y][x] == 1)
				printf(" ");
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

int DetectCollision(int posX, int posY)   //1:Yes 0: No
{
	int x, y;
	int arrX = (posX- GBOARD_ORIGIN_X) / 2; //gameboard만의 위치를 갖게 하기 위해 평행이동한 만큼의 수를 뺀다.
											//(0으로 바짝 붙게 만든다고 생각하자)
											//2로 나누는 이유는?-> 각 배열 인덱스를 표준화?하기 위해서, 왜냐하면 ShowBlock합수에서 확인할 수 있듯, x값을 2번 잡아먹기 때문에 나누기 2를 해주는 것 
	int arrY = posY - GBOARD_ORIGIN_Y;		//얘도 마찬가지	

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			if (blockModel[block_id][y][x] == 1 && gameBoardInfo[arrY+y][arrX+x] == 1) return 0;//게임 블록이, 게임 보드와 충돌할 때
			//왜 gameBoardInfo속 arrY가 아닌, arrY+y일까?: +2를 해보았지만 긴 막대기 모양같은 경우 그 이전에 멈추어야 하며 이 때 y=4이다.
			//즉 +y,+x의 의미는 현재 커서 포인터에서 블록 모델의 크기만큼 확인하는 것이라고 보면 된다
		}
	}
	return 1;
}

void ShiftRight()
{
	DeleteBlock(blockModel[block_id]);
	curPos.X += 2;
	SetCurrentCursorPos(curPos.X, curPos.Y);
	ShowBlock(blockModel[block_id]);
}

void ShiftLeft()
{
	DeleteBlock(blockModel[block_id]);
	curPos.X -= 2;
	SetCurrentCursorPos(curPos.X, curPos.Y);
	ShowBlock(blockModel[block_id]);
}

void ShiftUp()
{
	DeleteBlock(blockModel[block_id]);
	curPos.Y -= 2;
	SetCurrentCursorPos(curPos.X, curPos.Y);
	ShowBlock(blockModel[block_id]);
}

int BlockDown()
{
	if (DetectCollision(curPos.X, curPos.Y + 1, blockModel[block_id])==0) return 0;
	//curPos.Y+1 이유: 위에 쓴 코드는 만났을 때 충돌 감지지만 그 이전에 충돌 감지하기 위해 값을 미리 땡겨준 것임
	DeleteBlock(blockModel[block_id]);
	curPos.Y++;
	SetCurrentCursorPos(curPos.X, curPos.Y);
	ShowBlock(blockModel[block_id]);
	return 1;
}

void RotateBlock()
{
	int block_base = block_id - block_id % 4;
	DeleteBlock(blockModel[block_id]);
	block_id = abs(block_base + (block_id - 1) % 4);
	ShowBlock(blockModel[block_id]);
}

void ProcessKeyInput()
{
	int i, key;
	for (i = 0; i < 20; i++)//왜 blockDown하고 i를 20번 기다려야 하는가?
	{						//->1000ms를 20번으로 나눈 것을 의미함
		if (_kbhit() != 0)  //굳이 나눈 이유는?: 정지해 있는 그 1초간 발생해도 처리해야 하므로
		{
			key = _getch();
			switch (key)
			{
				case LEFT:
					ShiftLeft();
					break;
				case RIGHT:
					ShiftRight();
					break;
				case UP:
					RotateBlock();
					break;
			}
		}
		Sleep(speed);//입력을 받지 않게된다면 Sleep(50);, 근데 여기서는 속도 조절을 사용하고 있다.
	}
}

void DrawGameBoard()
{
	int x, y;

	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT) printf("└");
		else printf("│");
	}
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X+(GBOARD_WIDTH+1)*2, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT) printf("┘");
		else printf("│");
	}

	for (x = 1; x < GBOARD_WIDTH + 1; x ++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x*2, GBOARD_ORIGIN_Y+GBOARD_HEIGHT);
		//왜 이렇게 설정하는 것일까?: 일단 위에서 ㄴ자 모형 입력으로 x=1부터 시작하게 된다.
		//현재 커서의 상태는 ORIGIN_X로 부터 떨어져있으며, ─는 두 칸을 차지하기 때문이다.
		printf("─");
	}

	SetCurrentCursorPos(GBOARD_ORIGIN_X+1,GBOARD_ORIGIN_Y);

	//Collision Check

	for (y = 0; y < GBOARD_HEIGHT ; y++)
	{
		gameBoardInfo[y][0] = 1;
		gameBoardInfo[y][GBOARD_WIDTH + 1]=1;
	}

	for (x = 0; x < GBOARD_WIDTH + 2; x++)
	{
		gameBoardInfo[GBOARD_HEIGHT][x] = 1;
	}
}

int main()
{
	int i;
	int key;
	srand((unsigned int)time(NULL));
	block_id = (rand() % 7) * 4;
	RemoveCursor();
	DrawGameBoard();

	while (1)
	{
		BlockDown();
		ProcessKeyInput();
	}

	return 0;
}