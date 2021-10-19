#include <stdio.h> // print
#include <stdlib.h> //abs
#include <ctime>  // do randoma
#include <algorithm> // std::max

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
// gameplay
bool player1Turn = true;
bool movePhase = true;
bool isGameOver = false;
int possibleRows[8];
int possibleColumns[8];
int numOfPossibleMoves = 0;
// statystyki
int numOfGames = 0;
int maxGames = 0;
// algorytmy
int maxDepth = 3;

struct Node 
{
	int row;
	int column;
};

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

	return numOfPossibleMoves > 0;
}

int minimax(Node& node, int depth, bool maximizingPlayer)
{
	if (depth == 0 )// TODO)
	{
		// TODO
	}

	if (maximizingPlayer)
	{
		// TODO
	}
	else
	{
		// TODO
	}

	return 0;
}

bool canDestroy(int row, int column)
{
	return isInBoard(row, column) && isValidField(row, column);
}

void destroy(int row, int column)
{
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
}

// pobieranie pol w poblizu gracza
void destroyRandomField(int& row, int& column)
{
	if (player1Turn)
	{
		if (!isMovePossible(player2Row, player2Column))
		{
			isMovePossible(player1Row, player1Column);
		}
	}
	else
	{
		if(!isMovePossible(player1Row, player1Column))
		{
			isMovePossible(player2Row, player2Column);
		}
	}
	setRandomField(row, column);
}

void takeField(int& row, int& column)
{
	printMsg();
	movePhase ? setRandomField(row, column) : destroyRandomField(row, column);
	printf("Wiersz: %d\nKolumna: %d\n", row + 1, column + 1);
}

void changeState(bool& state)
{
	state = !state;
}

void endGame() 
{
	char winner = player1Turn ? player2 : player1;
	printf("WYGRANA: %c\n", winner);
	isGameOver = true;
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
	}
	else
	{
		endGame();
	}
}
void tryToDestroy()
{
	// pole ktore chesz zniszczyc
	int row, column;
	takeField(row, column);
	// sprawdzenie czy poprawne, zniszczenie, zmiana fazy, zmiana gracza
	if (canDestroy(row, column))
	{
		destroy(row, column);
		changeState(movePhase);
		changeState(player1Turn);
	}
}

void takeTurn() 
{
	if (movePhase) 
	{
		player1Turn ? tryToMove(player1Row, player1Column) : tryToMove(player2Row, player2Column);
	}
	else 
	{
		tryToDestroy();
	}
}

void setNumOfGames() 
{
	printf("Podaj ilosc gier: ");
	scanf_s("%d", &maxGames);
}

void resetGame() 
{
	player1Row = 2, player1Column = 0;
	player2Row = 3, player2Column = 7;

	player1Turn = true;
	movePhase = true;
	isGameOver = false;
	numOfPossibleMoves = 0;

	if (numOfGames < maxGames) 
	{
		initBoard();
		printBoard();
	}
}

int main()
{
	// wybor ilosci gier, stworzenie planszy, wyprintowanie planszy
	setNumOfGames();
	initBoard();
	printBoard();
	// petla do grania
	while (numOfGames < maxGames)
	{
		while (!isGameOver)
		{
			takeTurn();
		}

		numOfGames += 1;
		resetGame();
	}

	return 0;
}

