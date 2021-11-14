#include <stdio.h> // print
#include <stdlib.h> //abs
#include <ctime>  // do randoma
#include <vector>  // do przechowywania ruchow
#include <chrono> // do czasu funkcji

using namespace std::chrono;

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
int player1Wins = 0;
// algorytmy
int maxDepth = 6;
const int MINIMAX = 1;
const int NEGAMAX = 2;
const int ALPHABETA = 3;
int algorithm = -99;

struct Move
{
	int row;
	int column;
};

struct GameState
{
	int player1Row;
	int player1Column;
	int player2Row;
	int player2Column;
	Move bestMove;
};

GameState gameState{ player1Row, player1Column, player2Row, player2Column };

/********************* GRA NIE ZMIANIAC***********************************/

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
		printf("\n  0 1 2 3 4 5 6 7\n");
		for (int i = 0; i < rows; i++)
		{
			printf("%i ", i);
			for (int j = 0; j < columns; j++)
			{
				printf("%c ", board[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	}
}

void printMsg()
{
	printf("Gracz %s... %s\n", player1Turn ? "1" : "2", movePhase ? "poruszanie" : "niszczenie");
}

bool isInBoard(int row, int column)
{
	return row >= 0 && row < rows&& column >= 0 && column < columns;
}

bool isValidField(int row, int column)
{
	return board[row][column] == field;
}

void changeState(bool& state)
{
	state = !state;
}

// true gdy znaleziono mozliwe pole
bool findFieldsAround(int playerRow, int playerColumn)
{
	numOfPossibleMoves = 0;

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			if (i == playerRow && j == playerColumn)
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
			{
				possibleRows[numOfPossibleMoves] = i;
				possibleColumns[numOfPossibleMoves] = j;
				numOfPossibleMoves++;
			}
		}
	}

	return numOfPossibleMoves > 0;
}

void move(int newRow, int newColumn)
{
	if (player1Turn)
	{
		board[player1Row][player1Column] = field;
		player1Row = newRow;
		player1Column = newColumn;
		board[player1Row][player1Column] = player1;
	}
	else
	{
		board[player2Row][player2Column] = field;
		player2Row = newRow;
		player2Column = newColumn;
		board[player2Row][player2Column] = player2;
	}

	printBoard();
}
// playerRow, playerColumn OUT parameters
void GetPlayerPos(int& playerRow, int& playerColumn, bool player)
{
	if (player)
	{
		playerRow = player1Row;
		playerColumn = player1Column;
	}
	else
	{
		playerRow = player2Row;
		playerColumn = player2Column;
	}
}

bool isMovePossible()
{
	int playerRow, playerColumn;
	GetPlayerPos(playerRow, playerColumn, player1Turn);

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			if (i == playerRow && j == playerColumn)
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
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
	board[row][column] = emptyField;
	printBoard();
}

void setRandomField(int& row, int& column)
{
	srand((int)time(0));
	int randomIndex = rand() % numOfPossibleMoves;
	row = possibleRows[randomIndex];
	column = possibleColumns[randomIndex];
}
/********************* GRA NIE ZMIANIAC***********************************/

/********************* ALGORYTMY ***********************************/

Move getOldMove(bool player)
{
	return player ? Move{ player1Row, player1Column } : Move{ player2Row, player2Column };
}

void movePlayer(Move move, bool player)
{
	if (player)
	{
		board[player1Row][player1Column] = field;
		player1Row = move.row;
		player1Column = move.column;
		board[player1Row][player1Column] = player1;

	}
	else
	{
		board[player2Row][player2Column] = field;
		player2Row = move.row;
		player2Column = move.column;
		board[player2Row][player2Column] = player2;
	}

	//printBoard();
}

std::vector<Move> getAllMoves(bool maximizingPlayer)
{
	std::vector<Move> Moves;
	int playerRow, playerColumn;
	GetPlayerPos(playerRow, playerColumn, maximizingPlayer);

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			// nie bierzemy pod uwage ruchow w tym samym miejscu lub gdy jest jeden ruch od poczatkowego miejsca i wybierzemy miejsce na ktorym stoimy
			if ((i == playerRow && j == playerColumn))
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
			{
				Move possibleMove{ i, j };
				Moves.push_back(possibleMove);
			}
		}
	}

	return Moves;
}

bool IsInCorner(int row, int column)
{			//lewo gora           // prawo gora				// lewo dol					// prawo dol
	return (row < 2 && column < 2) || (row < 2 && column > 5) || (row > 3 && column < 2) || (row > 3 && column > 5);
}

bool IsInCenter(int row, int column)
{
	return ((row >= 2 && row <= 3) && (column >= 2 && column <= 5));
}

int getValue(bool maximizingPlayer)
{
	int playerRow, playerColumn, value = 0;
	GetPlayerPos(playerRow, playerColumn, maximizingPlayer);

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			if (i == playerRow && j == playerColumn)
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
			{
				value++;
			}
		}
	}

	if (IsInCenter(playerRow, playerColumn))
	{
		return value + 100;
	}
	else if (IsInCorner(playerRow, playerColumn))
	{
		return value;
	}
	else
	{
		return value + 10;
	}
}

// wynik to iloœæ mo¿liwych ruchow w danym ruchu
int evaluate(bool maximizingPlayer)
{
	return getValue(maximizingPlayer);
}

int alphabeta(GameState& gameState, int depth, bool maximizingPlayer, int alpha, int beta)
{
	int a = alpha;
	int b = beta;

	std::vector<Move> Moves = getAllMoves(maximizingPlayer);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1) // > 1 bo jak zostaje 1 ruch to jest on niszczony i w efekcie jest 0 ruchow
	{
		if (depth > 0)
		{
			if (maximizingPlayer)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = alphabeta(gameState, depth - 1, false, a, b);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						if (maxDepth - depth <= 1)
						{
							gameState.bestMove = move;
						}
						bestScore = newScore;
					}

					if (newScore >= b) 
					{
						break;
					}

					if (newScore > a)
					{
						a = newScore;
					}

				}
				return bestScore;
			}
			else
			{
				int bestScore = INT_MAX;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = alphabeta(gameState, depth - 1, true, a, b);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore < bestScore)
					{
						bestScore = newScore;
					}

					if (newScore <= a)
					{
						break;
					}

					if (newScore < b)
					{
						b = newScore;
					}
				}
				return bestScore;
			}
		}
		else // depth == 0
		{
			if (maxDepth <= 1)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = evaluate(maximizingPlayer);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						gameState.bestMove = move;
						bestScore = newScore;
					}

					if (newScore >= b)
					{
						break;
					}

					if (newScore > a)
					{
						a = newScore;
					}
				}
				if (maxDepth == 0)
				{
					return bestScore;
				}
			}

			return evaluate(maximizingPlayer);
		}
	}
	else // end game
	{
		return maximizingPlayer ? INT_MIN : INT_MAX;
	}
}

int negamax(GameState& gameState, int depth, int sign)
{
	std::vector<Move> Moves = getAllMoves(sign == 1);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1) // > 1 bo jak zostaje 1 ruch to jest on niszczony i w efekcie jest 0 ruchow
	{
		if (depth > 0)
		{
			int bestScore = INT_MIN;
			for (auto move : Moves)
			{
				Move oldMove{ getOldMove(sign == 1) };
				movePlayer(move, sign == 1);
				int newScore;
				sign == 1? newScore = -negamax(gameState, depth - 1, -1) : newScore = -negamax(gameState, depth - 1, 1);
				movePlayer(oldMove, sign == 1);
				
				if (newScore > bestScore)
				{
					if (maxDepth - depth <= 1 && sign == 1)
					{
						gameState.bestMove = move;
					}
					bestScore = newScore;
				}
			}
			return bestScore;
		}
		else // depth == 0
		{
			if (maxDepth <= 1)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(sign == 1) };
					movePlayer(move, sign == 1);
					int newScore = evaluate(sign == 1);
					movePlayer(oldMove, sign == 1);

					if (newScore > bestScore)
					{
						gameState.bestMove = move;
						bestScore = newScore;
					}
				}
				if (maxDepth == 0)
				{
					return bestScore;
				}
			}

			return sign * evaluate(sign == 1);
		}
	}
	else // end game
	{
		return sign == 1 ? sign * INT_MIN : sign * INT_MAX;
	}
}

int minimax(GameState& gameState, int depth, bool maximizingPlayer)
{
	std::vector<Move> Moves = getAllMoves(maximizingPlayer);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1) // > 1 bo jak zostaje 1 ruch to jest on niszczony i w efekcie jest 0 ruchow
	{
		if (depth > 0)
		{
			if (maximizingPlayer)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = minimax(gameState, depth - 1, false);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						if (maxDepth - depth <= 1)
						{
							gameState.bestMove = move;
						}
						bestScore = newScore;
					}
				}
				return bestScore;
			}
			else
			{
				int bestScore = INT_MAX;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = minimax(gameState, depth - 1, true);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore < bestScore)
					{
						bestScore = newScore;
					}
				}
				return bestScore;
			}
		}
		else // depth == 0
		{
			if (maxDepth <= 1)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = evaluate(maximizingPlayer);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						gameState.bestMove = move;
						bestScore = newScore;
					}
				}
				if (maxDepth == 0)
				{
					return bestScore;
				}
			}

			return evaluate(maximizingPlayer);
		}
	}
	else // end game
	{
		return maximizingPlayer ? INT_MIN : INT_MAX;
	}
}

/********************* ALGORYTMY ***********************************/

/********************* GRA NIE ZMIANIAC***********************************/
void destroyRandomField(int& row, int& column)
{
	if (player1Turn)
	{
		if (!findFieldsAround(player2Row, player2Column))
		{
			findFieldsAround(player1Row, player1Column);
		}
	}
	else
	{
		if (!findFieldsAround(player1Row, player1Column))
		{
			findFieldsAround(player2Row, player2Column);
		}
	}
	setRandomField(row, column);
}

void takeField(int& row, int& column)
{
	printMsg();
	if (movePhase)
	{
		if (player1Turn)
		{
			int value;
			gameState = { player1Row, player1Column, player2Row, player2Column, Move{player1Row, player1Column} };
			
			if (algorithm == MINIMAX) 
			{
				maxDepth % 2 == 0 ? value = minimax(gameState, maxDepth, true) : value = minimax(gameState, maxDepth, false);
			}
			else if (algorithm == NEGAMAX) 
			{
				maxDepth % 2 == 0 ? value = negamax(gameState, maxDepth, 1) : value = negamax(gameState, maxDepth, -1);
			}
			else if (algorithm == ALPHABETA)
			{
				maxDepth % 2 == 0 ? value = alphabeta(gameState, maxDepth, true, INT_MIN, INT_MAX) : value = alphabeta(gameState, maxDepth, false, INT_MIN, INT_MAX);
			}

			printf("Best value: %d best move row: %d column: %d: \n", value, gameState.bestMove.row, gameState.bestMove.column);
			row = gameState.bestMove.row;
			column = gameState.bestMove.column;

			if (gameState.player1Row == row && gameState.player1Column == column)
			{
				findFieldsAround(player1Row, player1Column);
				setRandomField(row, column);
			}

			movePlayer(Move{ gameState.player1Row, gameState.player1Column }, true);
			movePlayer(Move{ gameState.player2Row, gameState.player2Column }, false);
			player1Turn = true;
		}
		else
		{
			findFieldsAround(player2Row, player2Column);
			setRandomField(row, column);
		}
	}
	else
	{
		destroyRandomField(row, column);
	}

	printf("Ruch do Wiersz: %d Kolumna: %d\n", row, column);
}

void endGame()
{
	if (player1Turn)
	{
		printf("WYGRANA: %c\n", player2);
	}
	else
	{
		printf("WYGRANA: %c\n", player1);
		player1Wins++;
	}
	isGameOver = true;
}

void tryToMove()
{
	if (isMovePossible())
	{
		int row, column;
		takeField(row, column);
		move(row, column);
		changeState(movePhase);
	}
	else
	{
		endGame();
	}
}
void tryToDestroy()
{
	int row, column;
	takeField(row, column);
	if (canDestroy(row, column))
	{
		destroy(row, column);
		changeState(movePhase);
		changeState(player1Turn);
	}
}

void takeTurn()
{
	movePhase ? tryToMove() : tryToDestroy();
}

void setGameConfig()
{
	printf("Podaj liczbe gier: ");
	scanf_s("%d", &maxGames);

	printf("Wybierz algorytm:\n1 - MiniMax\n2 - NegaMax\n3 - AlphaBeta\n");
	scanf_s("%d", &algorithm);

	printf("Podaj glebokosc dla algorytmu: ");
	scanf_s("%d", &maxDepth);
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
/********************* GRA NIE ZMIANIAC***********************************/

int main()
{
	setGameConfig();
	initBoard();
	printBoard();

	auto start = high_resolution_clock::now();

	while (numOfGames < maxGames) // petla do grania
	{
		while (!isGameOver)
		{
			takeTurn();
		}

		numOfGames += 1;
		resetGame();
	}

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);

	printf("ALGORYTM WYGRAL %d NA %d GIER\n", player1Wins, numOfGames);
	printf("CZAS ROZEGRANIA GIER %d SEKUND\n", duration.count());

	return 0;
}


