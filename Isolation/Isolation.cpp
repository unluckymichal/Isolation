#include <stdio.h> // print
#include <stdlib.h> //abs
//
// ustawienia planszy
const int rows = 6;
const int columns = 8;
char board[rows][columns];
int player1Row = 2, player1Column = 0;
int player2Row = 3, player2Column = 7;
// przypisanie znakow
const char field = '#';
const char emptyField = ' ';
const char player1 = 'A';
const char player2 = 'B';
// menu
int userInput = 0;
const int exitNum = 99;
bool player1Turn = true;
bool movePhase = true;

void initBoard() 
{
	// inicjalizacja pol
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			board[i][j] = field;
		}
	}

	// ustawienie graczy
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
	return row > 0 && row <= rows && column > 0 && column <= columns;
}

bool isValidMove(int row, int column, int playerRow, int playerColumn)
{
	row -= 1;
	column -= 1;
	bool isValidRow = abs(row - playerRow) <= 1;
	bool isValidColumn = abs(column - playerColumn) <= 1;

	return isValidRow && isValidColumn;
}

bool isValidField(int row, int column)
{
	row -= 1;
	column -= 1;
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
	row -= 1;
	column -= 1;
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
	int row = playerRow + 1;
	int column = playerColumn + 1;

	for (int i = row - 1; i < row + 2; i++)
	{
		for (int j = column - 1; j < column + 2; j++)
		{
			if (isInBoard(i, j) && isValidField(i, j)) 
			{
				return true;
			}
		}
	}

	return false;
}

bool canDestroy(int row, int column)
{
	return isInBoard(row, column) && isValidField(row, column);
}

void destroy(int row, int column)
{
	printf("Valid field\n");
	row -= 1;
	column -= 1;
	board[row][column] = emptyField;
	printBoard();
}

void takeFieldCords(int& row, int& column, const char* text)
{
	if (player1Turn) // mozna zrobic funkcje
	{
		printf("Gracz 1... %s\n", text);
	}
	else 
	{
		printf("Gracz 2... %s\n", text);
	}
	
	printf("Wiersz: ");
	userInput = scanf_s("%d", &row);
	printf("Kolumna: ");
	userInput = scanf_s("%d", &column);
}

void changeState(bool& state)
{
	state = !state;
}

void tryToMove(int playerRow, int playerColumn)
{
	if (isMovePossible(playerRow, playerColumn))
	{
		// pole na ktore chesz sie poruszyc
		int row, column;
		takeFieldCords(row, column, "Faza poruszania");
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
	else
	{
		// TODO GAME OVER
		printf("GAME OVER!\n");
		userInput = exitNum;
	}
}
void tryToDestroy()
{
	// pole na ktore chesz zniszczyc
	int row, column;
	takeFieldCords(row, column, "Faza niszczenia");
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
	//movePhase = false; - do testow niszczenia
	if (movePhase) 
	{
		if (player1Turn) 
		{
			tryToMove(player1Row, player1Column);
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

int main()
{
	// stworzenie planszy 
	initBoard();
	// wyprintowanie planszy
	printBoard();
	// petla do testowania
	while (userInput != exitNum)
	{
		takeTurn(player1Turn);
	}

	return 0;
}

