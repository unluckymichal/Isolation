#include <stdio.h> // print
#include <stdlib.h> //abs
#include <ctime>  // do randoma

// ustawienia planszy
const int rows = 6;
const int columns = 8;
char board[rows][columns];
int player1Row = 2, player1Column = 0;
int player2Row = 3, player2Column = 7;
// przypisanie znakow
const char field = '#';
const char emptyField = ' ';
const char player1 = '1';
const char player2 = '2';
// menu
int userInput = 0;
const int exitNum = 99;
const int AIvsAI = 1;
const int playerVsAI = 2;
const int playerVsPlayer = 3;
const int gameOver = 999;
// gameplay
bool player1Turn = true;
bool movePhase = true;
int gameMode = 0;
int possibleRows[8];
int possibleColumns[8];
int numOfPossibleMoves = 0;
// statystyki
int numOfTurns = 0;
int numOfGames = 0;
const int maxGames = 100;

void initBoard() 
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			board[i][j] = field;
		}
	}

	board[player1Row][player1Column] = player1;
	board[player2Row][player2Column] = player2;
}

void printBoard() 
{
	{
		printf("\n  1 2 3 4 5 6 7 8\n");
		for (int i = 0; i < rows; i++)
		{
			printf("%i ", i + 1);
			for (int j = 0; j < columns; j++)
			{
				printf("%c ", board[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	}
}

bool isInBoard(int row, int column) 
{
	return row >= 0 && row < rows && column >= 0 && column < columns;
}

bool isValidMove(int row, int column, int playerRow, int playerColumn)
{
	bool isValidRow = abs(row - playerRow) <= 1;
	bool isValidColumn = abs(column - playerColumn) <= 1;

	return isValidRow && isValidColumn;
}

bool isValidField(int row, int column)
{
	return board[row][column] == field;
}

bool canMove(int row, int column, int playerRow, int playerColumn)
{
	return isInBoard(row, column) && isValidMove(row, column, playerRow, playerColumn) && isValidField(row, column);
}

void move(int row, int column, int playerRow, int playerColumn)
{
	printf("Valid field\n");
	// zmiana znaku na polu gracza na niezajete pole
	board[playerRow][playerColumn] = field;
	// poruszenie sie na nowe pole
	if (player1Turn) 
	{
		player1Row = row;
		player1Column = column;
		board[row][column] = player1;
	}
	else 
	{
		player2Row = row;
		player2Column = column;
		board[row][column] = player2;
	}
	
	printBoard();
}

bool isMovePossible(int playerRow, int playerColumn)
{
	int row = playerRow;
	int column = playerColumn;
	numOfPossibleMoves = 0;

	for (int i = row - 1; i < row + 2; i++)
	{
		for (int j = column - 1; j < column + 2; j++)
		{
			if (isInBoard(i, j) && isValidField(i, j)) 
			{
				// zapis mozliwych ruchow dla AI
				possibleRows[numOfPossibleMoves] = i;
				possibleColumns[numOfPossibleMoves] = j;
				numOfPossibleMoves++;
			}
		}
	}

	printf("numOfPossibleMoves: %d\n", numOfPossibleMoves);
	return numOfPossibleMoves > 0;
}

bool canDestroy(int row, int column)
{
	return isInBoard(row, column) && isValidField(row, column);
}

void destroy(int row, int column)
{
	printf("Valid field\n");
	board[row][column] = emptyField;
	printBoard();
}

void printMsg() 
{
	const char* phaseName;
	const char* playerName;
	player1Turn ? playerName = "1" : playerName = "2";
	movePhase ? phaseName = "poruszanie" : phaseName = "niszczenie";
	printf("Gracz %s... %s\n", playerName, phaseName);
}

void setRandomField(int& row, int& column)
{
	srand((int)time(0));
	int randomIndex = rand() % numOfPossibleMoves;
	row = possibleRows[randomIndex];
	column = possibleColumns[randomIndex];
	printf("RandomIndex: %d\n", randomIndex);
}

// pobieranie pol w poblizu gracza
void destroyRandomField(int& row, int& column)
{
	if (player1Turn)
	{
		if (isMovePossible(player2Row, player2Column))
		{
			setRandomField(row, column);
		}
		else // moze byc bug
		{
			isMovePossible(player1Row, player1Column);
			setRandomField(row, column);
		}
	}
	else
	{
		if(isMovePossible(player1Row, player1Column))
		{
			setRandomField(row, column);
		}
		else // moze byc bug
		{
			isMovePossible(player2Row, player2Column);
			setRandomField(row, column);
		}
	}
}

void takePlayerField(int& row, int& column)
{
	printf("Wiersz: ");
	userInput = scanf_s("%d", &row);
	printf("Kolumna: ");
	userInput = scanf_s("%d", &column);
	row -= 1;
	column -= 1;
}

void takeBotField(int& row, int& column)
{
	if (movePhase)
	{
		setRandomField(row, column);
	}
	else
	{
		destroyRandomField(row, column);
	}

	printf("Ruch bota...\nWiersz: %d\nKolumna: %d\n", row + 1, column + 1);
}

void takeField(int& row, int& column)
{
	printMsg();
	
	switch (gameMode)
	{
	case AIvsAI:
		takeBotField(row, column);
		break;

	case playerVsAI:
		if (player1Turn) 
		{
			takePlayerField(row, column);
		}
		else
		{
			takeBotField(row, column);
		}
		break;

	case playerVsPlayer:
		takePlayerField(row, column);
		break;

	default:
		break;
	}
}

void changeState(bool& state)
{
	state = !state;
}

void endGame() 
{
	char winner = player1Turn ? player2 : player1;
	printf("WYGRANA: %c\nIlosc tur: %d\n\n", winner, numOfTurns);
	gameMode = gameOver;
}

void tryToMove(int playerRow, int playerColumn)
{
	if (isMovePossible(playerRow, playerColumn))
	{
		// pole na ktore chesz sie poruszyc
		int row, column;
		takeField(row, column);
		// sprawdzenie czy poprawne, poruszenie sie, zmiana fazy
		if (canMove(row, column, playerRow, playerColumn))
		{
			move(row, column, playerRow, playerColumn);
			changeState(movePhase);
		}
		else
		{
			printf("Bad field\n");
		}
	}
	else // koniec gry
	{
		endGame();
	}
}
void tryToDestroy()
{
	// pole na ktore chesz zniszczyc
	int row, column;
	takeField(row, column);
	// sprawdzenie czy poprawne, zniszczenie, zmiana fazy, zmiana gracza
	if (canDestroy(row, column))
	{
		destroy(row, column);
		changeState(movePhase);
		changeState(player1Turn);
	}
	else
	{
		printf("Bad field\n");
	}
}

void takeTurn(bool player1Turn) 
{
	if (movePhase) 
	{
		if (player1Turn) 
		{
			tryToMove(player1Row, player1Column);
			numOfTurns++;
		}
		else 
		{
			tryToMove(player2Row, player2Column);
		}
	}
	else 
	{
		tryToDestroy();
	}
}

void pickGameMode() 
{
	printf("Wybierz tryb gry: \n1 - AI vs AI\n2 - Player vs AI\n3 - Player vs Player\n");
	// granie dla graczy lub gracz vs ai
	//scanf_s("%d", &userInput);
	//gameMode = userInput;
	
	// auto granie dla botow
	gameMode = AIvsAI;
	numOfGames += 1;
}

void resetGame() 
{
	player1Row = 2, player1Column = 0;
	player2Row = 3, player2Column = 7;

	player1Turn = true;
	movePhase = true;
	numOfPossibleMoves = 0;
	numOfTurns = 0;
}

int main()
{
	// stworzenie planszy, wybor trybu gry, wyprintowanie planszy
	while (userInput != exitNum)
	{
		initBoard();
		pickGameMode();
		printBoard();
		// petla do grania
		while (gameMode != gameOver) 
		{
			takeTurn(player1Turn);
		}
		resetGame();
		// dla botow zeby nie bylo nieskonczonej ilosci gier
		if (numOfGames > maxGames)
		{
			userInput = exitNum;
		}
	}

	return 0;
}

