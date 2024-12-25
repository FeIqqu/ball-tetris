//---------------------------------------------------------------------------

#include <vcl.h>
#include <random>
#include <vector>
#include <set>

#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGameForm *GameForm;

const TColor colors[] = { clRed, clLime, clBlue, clYellow }; // ����� �������
const int colorCount = sizeof(colors) / sizeof(colors[0]); // ���������� ������

std::mt19937 rng(std::random_device{}()); // ��������� ��������� �����
std::uniform_int_distribution<int> dist(0, colorCount - 1); // �������� ��� ������

bool isFastFall = false; // ���� ����������� �������
const int fastFallSpeed = 6; // �������� ����������� ������� ������

bool canRotateActiveBalls = true; // ���� ����������� �������� �������� �������

const int radius = 20; // ������ ������
const int fallSpeed = 1; // �������� ������� ������
int currentFallSpeed = fallSpeed; // ������� �������� ������� ������

const int width = 400; // ������ �������� ����
const int height = 550; // ������ �������� ����

const int baseScore = 10; // ������� ���� �� �����
int score = 0; // ����

struct Ball
{
	int x, y;
	bool isActive;
	TColor color;
};

std::vector<Ball> balls; // ������ ���� �������
std::vector<Ball*> activeBalls; // ������ ���������� �� �������� ������
std::vector<TColor> nextBallsCorols; // ������ ������ ��������� �������

//---------------------------------------------------------------------------
void __fastcall AddBall(int x, int y, TColor color)
{
	Ball newBall = { x, y, true, color };
	balls.push_back(newBall);
}
//---------------------------------------------------------------------------
void DrawBall(TCanvas* Canvas, const Ball& ball)
{
	Canvas->Brush->Color = ball.color;

	Canvas->Ellipse(ball.x - radius, ball.y - radius,
					ball.x + radius, ball.y + radius);
}
//---------------------------------------------------------------------------
void DrawNextBalls()
{
	GameForm->NextBalls->Invalidate();
}
//---------------------------------------------------------------------------
void DropBalls()
{
	AddBall(180, -radius, nextBallsCorols[0]);
	AddBall(220, -radius, nextBallsCorols[1]);
	AddBall(200, -radius - 35, nextBallsCorols[2]);

	isFastFall = false;

	// ��������� ����� ��������� �������
	nextBallsCorols.clear();
	for (int i = 0; i < 3; i++)
	{
		nextBallsCorols.push_back(colors[dist(rng)]);
	}

	DrawNextBalls();

	// ��������� � �������� ����� ������ �� ����� ����
	for (int i = balls.size() - 3; i < balls.size(); i++)
	{
		activeBalls.push_back(&balls[i]);
	}

	canRotateActiveBalls = true;
}
//---------------------------------------------------------------------------
__fastcall TGameForm::TGameForm(TComponent* Owner)
	: TForm(Owner)
{
	for (int i = 0; i < 3; ++i)
	{
		nextBallsCorols.push_back(colors[dist(rng)]);
	}

	DropBalls();
}
//---------------------------------------------------------------------------
void __fastcall TGameForm::GameAreaPaint(TObject *Sender)
{
	GameArea->Canvas->Brush->Color = clBlack;
	GameArea->Canvas->Brush->Style = bsClear;
	GameArea->Canvas->Rectangle(GameArea->ClientRect);

	for (Ball& ball : balls)
	{
		DrawBall(GameArea->Canvas, ball);
	}
}
//---------------------------------------------------------------------------
// ������� ��� ��������, ������ �� ���������� ������� �����
bool IsPositionOccupied(int x, int y)
{
	for (const Ball& ball : balls)
	{
		if (ball.x == x && ball.y == y)
		{
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void PhysicsProcess(std::vector<Ball>& balls)
{
	for (size_t i = 0; i < balls.size(); i++)
	{
		if (!balls[i].isActive) continue; // ������������ ������ �������� ����

		Ball& currentBall = balls[i];

		// ���������� ������� �������� �������
		int fallSpeed = isFastFall ? fastFallSpeed : currentFallSpeed;

		// ���������, ������ �� ��� ������ ������� ������
		if (currentBall.y + radius + fallSpeed > height)
		{
			currentBall.isActive = false;
			currentBall.y = height - radius;

			// ���������, ��������� �� ����� �� �� ������ �������
			if (currentBall.x % (2 * radius) == 0)
			{
				int offset = radius;
				Ball* leftNeighbor = nullptr;
				Ball* rightNeighbor = nullptr;

				// ���� ������� ����� � ������
				for (Ball& other : balls)
				{
					if (other.isActive) // ����� �� ��� �� ������
					{
						if (other.x == currentBall.x - 2 * radius) // ����� �����
							leftNeighbor = &other;
						else if (other.x == currentBall.x + 2 * radius) // ����� ������
							rightNeighbor = &other;
					}
				}

				// ���� ��� �������, ������� ����� �����
				if (!leftNeighbor && !rightNeighbor)
				{
					currentBall.x -= offset;
				}
				// ���� ���� �����, ������� ������� ����� � ������
				else if (leftNeighbor)
				{
					currentBall.x += offset; // ������� ������� ����� ������
					leftNeighbor->x -= offset; // ������� ������ �����
				}
				else if (rightNeighbor)
				{
					currentBall.x -= offset; // ������� ������� ����� �����
					rightNeighbor->x += offset; // ������� ������ ������
				}
			}

			continue;
		}

		// ���������, ������ �� ��� ����� ������� ������
		if (currentBall.x - radius < 0)
		{
			currentBall.isActive = false;
			currentBall.x = radius;
			continue;
		}

		// ���������, ������ �� ��� ������ ������� ������
		else if (currentBall.x + radius > width)
		{
			currentBall.isActive = false;
			currentBall.x = width - radius;
			continue;
		}

		// �������� �� ������������ ���� � ������� ������
		Ball* collidedBall = nullptr;
		for (Ball& otherBall : balls)
		{
			if (&currentBall != &otherBall &&
				hypot(currentBall.x - otherBall.x, currentBall.y + fallSpeed - otherBall.y - 1) < 2 * radius)
			{
				collidedBall = &otherBall;
				isFastFall = false;
				break;
			}
		}

		// ���� �� ��������� ������������ � ������� ������ � �� ������
		if (collidedBall == nullptr)
		{
			currentBall.y += fallSpeed;
			continue;
		}

		canRotateActiveBalls = false;

		// ���������� ����������� �������: -1 �����, 1 ������
		int fallSide;

		if (currentBall.x == radius && collidedBall->x == radius) // ������ � ����� �������
		{
			fallSide = 1; // ������� ������
		}
		else if (currentBall.x == width - radius && collidedBall->x == width - radius) // ������ � ������ �������
		{
			fallSide = -1; // ������� �����
		}
		else
		{
			fallSide = (currentBall.x <= collidedBall->x) ? -1 : 1;
		}

		// ���� ������������� ��� ������������ � ������� ������� ���� ������/�����
		if (IsPositionOccupied(collidedBall->x + 2 * radius * fallSide, collidedBall->y) &&
			hypot(currentBall.x - (collidedBall->x + 2 * radius * fallSide), currentBall.y - collidedBall->y) < 2 * radius)
		{
			currentBall.isActive = false;
		}
		else
		{
			currentBall.x += fallSide;
		}
	}
}
//---------------------------------------------------------------------------
// ������� ��� ���������� ������ ������� ����������� �����
void FindGroupIndices(int currentIndex, std::set<int>& groupIndices)
{
	if (groupIndices.count(currentIndex)) return;

    groupIndices.insert(currentIndex);
	Ball& current = balls[currentIndex];

	const int directions[6][2] = {
		{-2 * radius, 0}, // �����
		{-radius, -34}, // �����-�����
		{radius, -34}, // �����-������
		{2 * radius, 0},  // ������
		{radius, 34},  // ����-������
		{-radius, 34}  // ����-�����
	};

	// �������� 6 �����������
    for (int i = 0; i < 6; i++)
    {
        int newX = current.x + directions[i][0];
        int newY = current.y + directions[i][1];

        for (size_t j = 0; j < balls.size(); ++j)
        {
			if (!groupIndices.count(j) &&
				current.color == balls[j].color &&
				balls[j].x == newX && balls[j].y == newY)
			{
				FindGroupIndices(j, groupIndices);
			}
		}
	}
}
//---------------------------------------------------------------------------
void ProcessGroups()
{
	std::set<int> toDeleteIndices;

	for (size_t i = 0; i < balls.size(); i++)
	{
		std::set<int> groupIndices;
		FindGroupIndices(i, groupIndices);

		if (groupIndices.size() >= 4)
		{
			toDeleteIndices.insert(groupIndices.begin(), groupIndices.end());
		}
	}

	if(toDeleteIndices.size() >= 4)
	{
		int bonus = (toDeleteIndices.size() - 4) * baseScore; // ����� �� ������ ������
		score += baseScore * toDeleteIndices.size() + bonus;
	}

	// ������� ��������� ������
	std::vector<Ball> newBalls;
	for (size_t i = 0; i < balls.size(); ++i)
	{
		if (!toDeleteIndices.count(i))
		{
			newBalls.push_back(balls[i]);
		}
	}

	balls = std::move(newBalls);

	// ������ ��� ���� ��������� ������ ���� ���� ��������� ������
	if (!toDeleteIndices.empty())
	{
		for (Ball& ball : balls)
		{
			ball.isActive = true;
		}
	}

	GameForm->ScoreLabel->Caption = "Score: " + IntToStr(score);
}
//---------------------------------------------------------------------------
void TGameForm::ResetGame()
{
	balls.clear();
	activeBalls.clear();
	nextBallsCorols.clear();
	score = 0;

	for (int i = 0; i < 3; ++i)
	{
		nextBallsCorols.push_back(colors[dist(rng)]);
	}

	DropBalls();
	GameForm->ScoreLabel->Caption = "Score: 0";
}
//---------------------------------------------------------------------------
// �������� ��������� ����
bool CheckGameOver()
{
	for (const Ball& ball : balls)
	{
		// ���� ��������������� ����� ���� �������� ����
		if (!ball.isActive && ball.y - radius < 0)
		{
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void CheckAndFixActiveBalls()
{
	bool allBallsFixed = true;

	// ���������, ��� �� �������� ���� �������������
	for (Ball* ball : activeBalls)
	{
		if (ball->isActive)
		{
			allBallsFixed = false;
			break;
		}
	}

	// ��������� ��������� ����
	for (const Ball& ball : balls)
	{
		if (ball.isActive)
		{
			allBallsFixed = false;
			break;
		}
	}

	// ���� ��� ���� �������������, ������� ������ activeBalls, ��������� �� ������� ����� � ���������� ����� ������
	if (allBallsFixed)
	{
		activeBalls.clear();
		ProcessGroups();

		// ����� ��������� ����� ���������, ���� �� �������� ����
		bool hasActiveBalls = false;
		for (const Ball& ball : balls)
		{
			if (ball.isActive)
			{
				hasActiveBalls = true;
				break;
			}
		}

		// ���� �������� ����� ������ ���, ���������� �����
		if (!hasActiveBalls)
		{
			// ���������, ��������� �� ����
			if (CheckGameOver())
			{
				ShowMessage("Game Over! Your score: " + IntToStr(score));
				GameForm->ResetGame();
				return;
			}

			DropBalls();
		}
	}
}
//---------------------------------------------------------------------------
void UpdateFallSpeed()
{
	const int scoreThreshold = 100; // ����, ����� ������� ������������� ��������
	const int maxSpeed = 10; // ������������ �������� �������

	// ����������� �������� ������ 'scoreThreshold' �����
	currentFallSpeed = fallSpeed + score / scoreThreshold;
	if (currentFallSpeed > maxSpeed)
		currentFallSpeed = maxSpeed;
}
//---------------------------------------------------------------------------
void __fastcall TGameForm::GameTimer(TObject *Sender)
{
	UpdateFallSpeed();
	PhysicsProcess(balls);
	CheckAndFixActiveBalls();
	GameArea->Invalidate();
}
//---------------------------------------------------------------------------
// �������� ����������� ����������� �������� ����� �����/������
bool CanMoveActiveBalls(int dx)
{
	for (Ball* ball : activeBalls)
	{
		int newX = ball->x + dx;

		// �������� �� ������� ������
		if (newX - radius < 0 || newX + radius > width)
		{
			return false;
		}

		// �������� �� ������������ � ���������������� ������
		for (const Ball& fixedBall : balls)
		{
			if (fixedBall.isActive == false && hypot(newX - fixedBall.x, ball->y - fixedBall.y) < 2 * radius)
			{
				return false; // �������� � ������������� �����
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void RotateActiveBalls(bool clockwise)
{
	if (activeBalls.size() < 3 || !canRotateActiveBalls) return;

	// ��������� ���������� ��� ������ �������
	TColor color1 = activeBalls[0]->color;
	TColor color2 = activeBalls[1]->color;
	TColor color3 = activeBalls[2]->color;

	if (clockwise)
	{
		// �������� �� ������� �������
		activeBalls[0]->color = color2;
		activeBalls[1]->color = color3;
		activeBalls[2]->color = color1;
	}
	else
	{
		// �������� ������ ������� �������
		activeBalls[0]->color = color3;
		activeBalls[1]->color = color1;
		activeBalls[2]->color = color2;
	}
}
//---------------------------------------------------------------------------
void __fastcall TGameForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (activeBalls.empty()) return;

	// �������� �����
	if (Key == VK_LEFT && CanMoveActiveBalls(-radius) && !isFastFall)
	{
		for (Ball* ball : activeBalls)
		{
			ball->x -= radius;
		}
	}
	// �������� ������
	else if (Key == VK_RIGHT && CanMoveActiveBalls(radius) && !isFastFall)
	{
		for (Ball* ball : activeBalls)
		{
			ball->x += radius;
		}
	}
	// ������� �� ������� �������
	else if (Key == VK_UP)
	{
		RotateActiveBalls(true);
	}
	// ������� ������ ������� �������
	else if (Key == VK_DOWN)
	{
		RotateActiveBalls(false);
	}
	// ��������� �������
	else if (Key == VK_SPACE)
	{
		isFastFall = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TGameForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TGameForm::NextBallsPaint(TObject *Sender)
{
	NextBalls->Canvas->Brush->Color = clBlack;
	NextBalls->Canvas->Brush->Style = bsClear;
	NextBalls->Canvas->Rectangle(NextBalls->ClientRect);

	// ���������, ���������� �� ������ ��� ���������
	if (nextBallsCorols.size() < 3) return;

	TCanvas* Canvas = NextBalls->Canvas;

	Canvas->Brush->Color = nextBallsCorols[0];
	Canvas->Ellipse(30 - radius, 60 - radius,
					30 + radius, 60 + radius);

	Canvas->Brush->Color = nextBallsCorols[1];
	Canvas->Ellipse(70 - radius, 60 - radius,
					70 + radius, 60 + radius);

	Canvas->Brush->Color = nextBallsCorols[2];
	Canvas->Ellipse(50 - radius, 26 - radius,
					50 + radius, 26 + radius);
}
//---------------------------------------------------------------------------

