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

const TColor colors[] = { clRed, clLime, clBlue, clYellow }; // Цвета шариков
const int colorCount = sizeof(colors) / sizeof(colors[0]); // Количество цветов

std::mt19937 rng(std::random_device{}()); // Генератор случайных чисел
std::uniform_int_distribution<int> dist(0, colorCount - 1); // Диапазон для цветов

bool isFastFall = false; // Флаг ускоренного падения
const int fastFallSpeed = 6; // Скорость ускоренного падения шарика

bool canRotateActiveBalls = true; // Флаг возможности вращения активных шариков

const int radius = 20; // Радиус шарика
const int fallSpeed = 1; // Скорость падения шарика
int currentFallSpeed = fallSpeed; // Текущая скорость падения шарика

const int width = 400; // ширина игрового поля
const int height = 550; // высота игрового поля

const int baseScore = 10; // Базовые очки за шарик
int score = 0; // Счёт

struct Ball
{
	int x, y;
	bool isActive;
	TColor color;
};

std::vector<Ball> balls; // Список всех шариков
std::vector<Ball*> activeBalls; // Список указателей на активные шарики
std::vector<TColor> nextBallsCorols; // Список цветов следующих шариков

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

	// Обновляем цвета следующих шариков
	nextBallsCorols.clear();
	for (int i = 0; i < 3; i++)
	{
		nextBallsCorols.push_back(colors[dist(rng)]);
	}

	DrawNextBalls();

	// Добавляем в активную пачку ссылки на новые шары
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
// Функция для проверки, занята ли конкретная позиция шаром
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
		if (!balls[i].isActive) continue; // Обрабатываем только активные шары

		Ball& currentBall = balls[i];

		// Определяем текущую скорость падения
		int fallSpeed = isFastFall ? fastFallSpeed : currentFallSpeed;

		// Проверяем, достиг ли шар нижней границы экрана
		if (currentBall.y + radius + fallSpeed > height)
		{
			currentBall.isActive = false;
			currentBall.y = height - radius;

			// Проверяем, находится ли шарик не на нужной позиции
			if (currentBall.x % (2 * radius) == 0)
			{
				int offset = radius;
				Ball* leftNeighbor = nullptr;
				Ball* rightNeighbor = nullptr;

				// Ищем соседей слева и справа
				for (Ball& other : balls)
				{
					if (other.isActive) // Сосед на той же высоте
					{
						if (other.x == currentBall.x - 2 * radius) // Сосед слева
							leftNeighbor = &other;
						else if (other.x == currentBall.x + 2 * radius) // Сосед справа
							rightNeighbor = &other;
					}
				}

				// Если нет соседей, смещаем шарик влево
				if (!leftNeighbor && !rightNeighbor)
				{
					currentBall.x -= offset;
				}
				// Если есть сосед, смещаем текущий шарик и соседа
				else if (leftNeighbor)
				{
					currentBall.x += offset; // Смещаем текущий шарик вправо
					leftNeighbor->x -= offset; // Смещаем соседа влево
				}
				else if (rightNeighbor)
				{
					currentBall.x -= offset; // Смещаем текущий шарик влево
					rightNeighbor->x += offset; // Смещаем соседа вправо
				}
			}

			continue;
		}

		// Проверяем, достиг ли шар левой границы экрана
		if (currentBall.x - radius < 0)
		{
			currentBall.isActive = false;
			currentBall.x = radius;
			continue;
		}

		// Проверяем, достиг ли шар правой границы экрана
		else if (currentBall.x + radius > width)
		{
			currentBall.isActive = false;
			currentBall.x = width - radius;
			continue;
		}

		// Проверка на столкновение шара с другими шарами
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

		// Если не произошло столкновения с другими шарами — он падает
		if (collidedBall == nullptr)
		{
			currentBall.y += fallSpeed;
			continue;
		}

		canRotateActiveBalls = false;

		// Определяем направление падения: -1 влево, 1 вправо
		int fallSide;

		if (currentBall.x == radius && collidedBall->x == radius) // Близко к левой границе
		{
			fallSide = 1; // Двигаем вправо
		}
		else if (currentBall.x == width - radius && collidedBall->x == width - radius) // Близко к правой границе
		{
			fallSide = -1; // Двигаем влево
		}
		else
		{
			fallSide = (currentBall.x <= collidedBall->x) ? -1 : 1;
		}

		// Если скатывающийся шар сталкивается с соседом нижнего шара справа/слева
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
// Функция для нахождения группы шариков одинакового цвета
void FindGroupIndices(int currentIndex, std::set<int>& groupIndices)
{
	if (groupIndices.count(currentIndex)) return;

    groupIndices.insert(currentIndex);
	Ball& current = balls[currentIndex];

	const int directions[6][2] = {
		{-2 * radius, 0}, // влево
		{-radius, -34}, // вверх-влево
		{radius, -34}, // вверх-вправо
		{2 * radius, 0},  // вправо
		{radius, 34},  // вниз-вправо
		{-radius, 34}  // вниз-влево
	};

	// Проверка 6 направлений
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
		int bonus = (toDeleteIndices.size() - 4) * baseScore; // Бонус за размер группы
		score += baseScore * toDeleteIndices.size() + bonus;
	}

	// Удаляем найденные группы
	std::vector<Ball> newBalls;
	for (size_t i = 0; i < balls.size(); ++i)
	{
		if (!toDeleteIndices.count(i))
		{
			newBalls.push_back(balls[i]);
		}
	}

	balls = std::move(newBalls);

	// Делаем все шары активными только если есть удаленные группы
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
// Проверка окончания игры
bool CheckGameOver()
{
	for (const Ball& ball : balls)
	{
		// Если зафиксированный шарик выше игрового поля
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

	// Проверяем, все ли активные шары зафиксированы
	for (Ball* ball : activeBalls)
	{
		if (ball->isActive)
		{
			allBallsFixed = false;
			break;
		}
	}

	// Проверяем остальные шары
	for (const Ball& ball : balls)
	{
		if (ball.isActive)
		{
			allBallsFixed = false;
			break;
		}
	}

	// Если все шары зафиксированы, очищаем вектор activeBalls, проверяем на наличие групп и сбрасываем новые шарики
	if (allBallsFixed)
	{
		activeBalls.clear();
		ProcessGroups();

		// После обработки групп проверяем, есть ли активные шары
		bool hasActiveBalls = false;
		for (const Ball& ball : balls)
		{
			if (ball.isActive)
			{
				hasActiveBalls = true;
				break;
			}
		}

		// Если активных шаров больше нет, сбрасываем новые
		if (!hasActiveBalls)
		{
			// Проверяем, завершена ли игра
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
	const int scoreThreshold = 100; // Очки, после которых увеличивается скорость
	const int maxSpeed = 10; // Максимальная скорость падения

	// Увеличиваем скорость каждые 'scoreThreshold' очков
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
// Проверка возможности перемещения активной пачки влево/вправо
bool CanMoveActiveBalls(int dx)
{
	for (Ball* ball : activeBalls)
	{
		int newX = ball->x + dx;

		// Проверка на границы экрана
		if (newX - radius < 0 || newX + radius > width)
		{
			return false;
		}

		// Проверка на столкновение с зафиксированными шарами
		for (const Ball& fixedBall : balls)
		{
			if (fixedBall.isActive == false && hypot(newX - fixedBall.x, ball->y - fixedBall.y) < 2 * radius)
			{
				return false; // Коллизия с фиксированным шаром
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void RotateActiveBalls(bool clockwise)
{
	if (activeBalls.size() < 3 || !canRotateActiveBalls) return;

	// Временные переменные для цветов шариков
	TColor color1 = activeBalls[0]->color;
	TColor color2 = activeBalls[1]->color;
	TColor color3 = activeBalls[2]->color;

	if (clockwise)
	{
		// Вращение по часовой стрелке
		activeBalls[0]->color = color2;
		activeBalls[1]->color = color3;
		activeBalls[2]->color = color1;
	}
	else
	{
		// Вращение против часовой стрелки
		activeBalls[0]->color = color3;
		activeBalls[1]->color = color1;
		activeBalls[2]->color = color2;
	}
}
//---------------------------------------------------------------------------
void __fastcall TGameForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (activeBalls.empty()) return;

	// Движение влево
	if (Key == VK_LEFT && CanMoveActiveBalls(-radius) && !isFastFall)
	{
		for (Ball* ball : activeBalls)
		{
			ball->x -= radius;
		}
	}
	// Движение вправо
	else if (Key == VK_RIGHT && CanMoveActiveBalls(radius) && !isFastFall)
	{
		for (Ball* ball : activeBalls)
		{
			ball->x += radius;
		}
	}
	// Поворот по часовой стрелке
	else if (Key == VK_UP)
	{
		RotateActiveBalls(true);
	}
	// Поворот против часовой стрелки
	else if (Key == VK_DOWN)
	{
		RotateActiveBalls(false);
	}
	// Ускорение падения
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

	// Проверяем, достаточно ли цветов для отрисовки
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

