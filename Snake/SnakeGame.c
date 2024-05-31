/*------- File name: SnakeGame.c ---------*
* Snake Game                              *
*                                         *
* Amit Erlich (206362568) 02/01/21        *
*-----------------------------------------*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>

#define UP 72  // Up Arrow 
#define DOWN 80 // Down Arrow 
#define LEFT 75  // Left Arrow 
#define RIGHT 77  // Right Arrow 
#define ESC 27  // Escape Key

#define BORDER_ROW 80
#define BORDER_COL 24
#define MIN_BODY_SIZE 10

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	Point* body;
	int size;
} Snake;

// ---- FUNCTIONS DECLARATION ----

// Menu Functions
int gameMenu();
void printMenu();
int getUserMenuChoice();

// Game Functions
void Game();
void printBorder();
// snake functions
void moveSnake(Snake* snake, int intialMinSize);
bool checkMove(Snake* snake);
Point* snakeRealloc(Snake* oldSnake);
// food functions
Point getRandomFoodPoint(Snake* snake);
void snakeAteFood(Snake* snake, Point* food, Point* tail);

// Print Functions
void printLossMsg();
void printWinMsg();

void gotoxy(int x, int y);
int getKey();

void clearScreen();


/* -------------------- main --------------------
* As long as user does not want to leave,
* prints game's menu and starts the selected game
-----------------------------------------------*/
void main()
{
	while (gameMenu() != 0)
		Game();
	clearScreen();
}

/* ----------- gameMenu ----------- 
* Returns: choice
* 
* Prints the game menu and get user
* choice
---------------------------------*/
int gameMenu()
{
	int choice;

	printMenu();
	choice = getUserMenuChoice();
	
	return choice;
}

/* ---- printMenu ----
* Prints the game menu
--------------------*/
void printMenu()
{
	printf("                    ________________________________________\n");
	printf("                   |                                        |\n");
	printf("                   |          - Welcome to Snake -          |\n");
	printf("                   |                                        |\n");
	printf("                   | Pick Your Level, to exit press 0       |\n");
	printf("                   | (0) Exit                               |\n");
	printf("                   | (1) Basic Level                        |\n");
	printf("                   | (2) Intermidiate Level                 |\n");
	printf("                   | (3) Advanced Level                     |\n");
	printf("                   |________________________________________|\n");
}

/* ----------- getUserMenuChoice -----------
* Retuers: choice
* 
* Gets user's input and checks its integrity
* returns it if it is ok
------------------------------------------*/
int getUserMenuChoice()
{
	int choice;

	printf("\n                   > Enter your choice: ");
	scanf("%d", &choice);

	// if user entered invalid choice
	while (choice < 0 || choice > 1)
	{
		clearScreen();

		if (choice == 2 || choice == 3)
			printf("(*) This level have not yet been developed.\n");
		else
			printf("Invalid choice!\n");

		printMenu();
		printf("\n                   > Enter your choice: ");
		scanf("%d", &choice);
	}
	return choice;
}
/* --------------------------- Game ----------------------------
* Sets the game, prints the border and the food.
* The game starts when an arrow key is pressed,
* then the snake start moving in the selected direction.
* The snake changes direction when the arrow keys are preesed.
* When the snake eat the food it grows and another food appears.
* The game continues until the snake has eaten 10 food, 
* or until it hit the border or itself.
* A win or loss message appears accordingly.
--------------------------------------------------------------*/
void Game()
{
	char c, tempC; // varies to the value of the key pressed
	// countToTen - count until snake body apears fully at the beginning of the game
	// countEatenFood - count how much food the snake ate
	int i, countToTen = 0, countEatenFood = 0;

	Snake snake;
	Point head, tail, food;

	bool hitItself = false;
	bool playerWon = false;

	// sets the snake
	snake.size = MIN_BODY_SIZE;
	snake.body = (Point*)malloc(snake.size * sizeof(Point));
	if (!snake.body) // if memory allocation did not succeeded
	{
		clearScreen();
		printf("ERROR - memory allocation failed!\n");
		return;
	}
	for (i = 0; i < snake.size; i++)
	{
		snake.body[i].x = BORDER_ROW / 2;
		snake.body[i].y = BORDER_COL / 2;
	}

	// for easier access to snake head and tail
	head.x = snake.body[0].x;
	head.y = snake.body[0].y;
	tail.x = snake.body[snake.size - 1].x;
	tail.y = snake.body[snake.size - 1].y;

	// sets a random food point 
	food = getRandomFoodPoint(&snake);

	// clear the screen and print the border
	clearScreen();
	printBorder();

	// print the food
	gotoxy(food.x, food.y);
	printf("$");

	// the snake start from the middle of the screen
	gotoxy(snake.body[0].x, snake.body[0].y);
	// no direction has been selected yet
	c = 0;
	
	// the game runs until user want to exit (ESC key is pressed),
	// until snake ate all food or until snake hit itself or the border
	while (c != ESC && !hitItself && !playerWon)
	{
		// sleeps for third of a second before continuing to the next command
		Sleep(300);

		// only if a key was hit we read what key code it was
		if (_kbhit()) // if any key was hit
		{	
			tempC = getKey();
			// only if the key pressed is allowed (arrow keys or ESC key)
			if (tempC == UP || tempC == DOWN || tempC == RIGHT || tempC == LEFT || tempC == ESC)
				c = tempC;  // change direction
		}

		// if an arrow key was press
		if (c != 0 && c != ESC)
		{
			// As long as the snake's visual size is stil less then 10 links (@)
			if (countToTen < 10)
				countToTen++;

			moveSnake(&snake, countToTen); // move snake in body array

			// make a move in current direction if no change
			switch (c)
			{
			case RIGHT:
				snake.body[0].x++;
				break;
			case DOWN:
				snake.body[0].y++;
				break;
			case LEFT:
				snake.body[0].x--;
				break;
			case UP:
				snake.body[0].y--;
				break;
			}

			// checks if snake hit itself or border
			hitItself = checkMove(&snake);

			// updates the head point to the new snake head
			head.x = snake.body[0].x;
			head.y = snake.body[0].y;

			// if snake hit the food
			if (head.x == food.x && head.y == food.y)
			{
				// increases the size of the snake and finds a new random food point
				snakeAteFood(&snake, &food, &tail);
				// updates the new tail
				tail.x = snake.body[snake.size - 1].x;
				tail.y = snake.body[snake.size - 1].y;
				
				countEatenFood++; // snake ate food
				// if snake ate 10 food - he won
				if (countEatenFood == 10)
					playerWon = true;
				else // the game continues - print food on the screen
				{
					gotoxy(food.x, food.y); 
					printf("$");
				}
			}

			// if snake did not hit itself or the border (game is not over)
			if (!hitItself)
			{
				gotoxy(tail.x, tail.y); // delete tail from the screen
				printf(" ");
				gotoxy(head.x, head.y); // print the new head
				printf("@");
			}

			// updates the tail point to the new snake tail
			tail.x = snake.body[snake.size - 1].x;
			tail.y = snake.body[snake.size - 1].y;
		}

		gotoxy(BORDER_ROW + 1, BORDER_COL + 1);
	}
	free(snake.body);

	// clear screen and print win or loss message
	clearScreen();
	(playerWon) ? (printWinMsg()) : (printLossMsg());
}

 /* ------------ printBorder ------------
 * Prints the game's border in size 80X24
 --------------------------------------*/
void printBorder()
{
	int i;

	for (i = 0; i <= BORDER_ROW; i++)
	{
		gotoxy(i, 0);
		printf("#");
		gotoxy(i, BORDER_COL);
		printf("#");
	}
	for (i = 1; i <= BORDER_COL; i++)
	{
		gotoxy(0, i);
		printf("#");
		gotoxy(BORDER_ROW, i);
		printf("#");
	}
}

/* -------------- moveSnake --------------
* Gets: *snake, intialMinSize
* 
* Move the points array of the snake body
---------------------------------------*/
void moveSnake(Snake* snake, int intialMinSize)
{
	int i, snakeSize;

	// if the snake visual size (on the screen) is less then 10 links
	if (intialMinSize < 10)
		snakeSize = intialMinSize;
	else
		snakeSize = snake->size;

	// move (snakeSize) links of the snake body
	for (i = snakeSize - 1; i > 0; i--)
	{
		snake->body[i].x = snake->body[i - 1].x;
		snake->body[i].y = snake->body[i - 1].y;
	}
}

/* -------------- checkMove ---------------
* Gets: *snake
* Returns: True / False
* 
* Checks if snake hit itself or the border
* and return true if so, else false
----------------------------------------*/
bool checkMove(Snake *snake)
{
	int i;
	
	// checks if hit border
	if (snake->body->x == 0 || snake->body->x == BORDER_ROW - 1 ||
		snake->body->y == 0 || snake->body->y == BORDER_COL)
		return true;

	// checks if hit itself
	for (i = 1; i < snake->size; i++)
		if (snake->body[i].x == snake->body->x && snake->body[i].y == snake->body->y)
			return true;

	return false;
}

/* --------- getRandomFoodPoint ----------
* Gets: *snake
* Returns: p
* 
* Draw a random point for a food point
* Check if it is not located on the border
* or on the snake, and return it.
----------------------------------------*/
Point getRandomFoodPoint(Snake *snake)
{
	Point p;
	bool goodLocation = false;
	int i;

	srand(time(NULL));

	// draw random numbers for the point
	p.x = 1 + rand() % (BORDER_ROW - 2);
	p.y = 1 + rand() % (BORDER_COL - 2);

	// checks if point location is good
	while (!goodLocation)
	{
		goodLocation = true;
		for (i = 1; i < snake->size; i++)
			if (snake->body[i].x == p.x && snake->body[i].y == p.y)
				goodLocation = false;
		// if not - draw another point location
		if (!goodLocation)
		{
			p.x = 1 + rand() % (BORDER_ROW - 1);
			p.y = 1 + rand() % (BORDER_COL - 1);
		}
	}
	return p;
}

/* ------- snakeAteFood -------
* Gets: *snake, *food, *tail
* 
* Increases snake size and find
* a new food point.
-----------------------------*/
void snakeAteFood(Snake *snake, Point *food, Point *tail)
{
	//increases the size of the snake
	snake->body = snakeRealloc(snake);
	// sets the new snake tail
	snake->body[(snake->size) - 1] = *tail;

	// calculate a new food point
	*food = getRandomFoodPoint(snake);
}

/* ------------ snakeRealloc --------------
* Gets: *snake
* Returns: *snakeBody
* 
* Creates a new bigger body for the snake,
* same in its content as the previous body.
* Free the old body array, and returns a 
* pointer to the new body array.
-----------------------------------------*/
Point* snakeRealloc(Snake* snake)
{
	Point* snakeBody;
	int i;

	// updates the new snake size
	snake->size = snake->size + 1;

	snakeBody = (Point*)malloc((snake->size) * sizeof(Point));

	if (!snakeBody)
	{
		clearScreen();
		printf("ERROR - memory allocation failed!\n");
		return;
	}

	// if memory allocation succeeded
	// copy arr into newArr
	for (i = 0; i < snake->size; i++)
		snakeBody[i] = snake->body[i];

	// free allocated memory of snake->body
	free(snake->body);
	
	return snakeBody;
}

/* --- printLossMsg ---
* Prints a loss message
---------------------*/
void printLossMsg()
{
	printf("                    ________________________________________ \n");
	printf("                   |                                        |\n");
	printf("                   |            >> GAME OVER! <<            |\n");
	printf("                   |________________________________________|\n\n");
}

/* --- printWinMsg ---
* Prints a win message
--------------------*/
void printWinMsg()
{
	printf("                    ________________________________________ \n");
	printf("                   |                                        |\n");
	printf("                   |             >> YOU WON! <<             |\n");
	printf("                   |________________________________________|\n\n");
}

// This function moves the cursor to the line y and column x.
// (0,0) is the upper left corner of the screen.
void gotoxy(int x, int y) {
	printf("\x1b[%d;%df", y + 1, x + 1);
}

// This function gets keyCodes for special keys such as arrows , escape and others
// It returns the Key code such as 72 for arrow up, 27 for Escape... 
int getKey()
{
	char KeyStroke = _getch();
	if (KeyStroke == 0 || KeyStroke == -32)
		KeyStroke = _getch();
	return (KeyStroke);
}

/* --- clearScreen ---
* Cleans the screen
--------------------*/
void clearScreen()
{
	system("cls");
}