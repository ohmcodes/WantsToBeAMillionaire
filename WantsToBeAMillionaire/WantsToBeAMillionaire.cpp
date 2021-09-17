// WantsToBeAMillionaire.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h> 
#include <Windows.h>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>

struct NodeQuestion 
{
	char nQuestion[256];
	char nChoices[4][256];
	char nAnswer;
	char nDialog[256];
	struct NodeQuestion* next;
	struct NodeQuestion* prev;
};

struct NodeQuestion* head;

FILE* QuestionFile;
FILE* AnswerFile;

char Letters[4] = { 'A', 'B', 'C', 'D' };
//declare size of array coz it will disappear when you pass it to a function
char ChoicesSize = sizeof(Letters) / sizeof(Letters[0]);
char YesNo = 'Y';
char Question[256];
char ChoicesArray[4][256];
char Answer;
char Dialog[256];
int ll50 = 1; //lifeline 50/50
int llCaF = 1; //lifeline Call a Friend
int llAtA = 1; //lifeline Ask the Audience
int currentQuestion = 1;
char header[256];

//pre init functions
void viewMainMenu();
void viewQEditor();
void viewAdd();
void viewEdit();
void viewDelete();
void viewInsert();
void viewPrint();
void PrintLogo();
void PrintHeader(int viewIndex);
void PrintQHeader(int QuestionIndex);
void PrintNextLevel();
void PrintGameOver(int MoneyIndex, int Gtype);
void PrintGameInfo(int ll50, int llCaF, int llATA, int current);
void PrintList(bool head, char header[256]);
void Print();
void CreateFileTxt();
void PlayGame(int QNum);
void DeleteNodes();
void InitFile();

const char* getfield(char* line, int num)
{
	char* token = NULL;
	char* next_token = NULL;

	//initiate line
	for (token = strtok_s(line, ";", &next_token);
		//check if not null
		token && *token;
		//remove newline when detects
		token = strtok_s(NULL, ";\n", &next_token))
	{
		if (!--num)
			return token;
	}
}

struct NodeQuestion* GetNewNodeQuestion(char Question[256], char Answer, char ChoicesArray[4][256], int ChoicesSize, char Dialog[256])
{
	struct NodeQuestion* newNode = (struct NodeQuestion*)malloc(sizeof(struct NodeQuestion));

	//Insert Question
	strcpy_s(newNode->nQuestion, Question);

	//Insert Answer
	newNode->nAnswer = toupper(Answer);

	//Insert Choices
	for (int i = 0; i < ChoicesSize; i++)
	{
		//debug
		//printf("Node Ins %s\n", ChoicesArray[i]);
		strcpy_s(newNode->nChoices[i], ChoicesArray[i]);
	}

	//Insert Dialog
	strcpy_s(newNode->nDialog, Dialog);

	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

char* GetCurrentMoney(int MoneyIndex)
{
	FILE* fp;
	fopen_s(&fp, "Money.txt", "r");

	char line[1024];
	int i = 1;
	char Money[20];

	if (fp)
	{

		while (fgets(line, 1024, fp))
		{
			char* tmp = _strdup(line);

			if (i == MoneyIndex)
			{
				strcpy_s(Money, getfield(tmp, 2));
			}

			free(tmp);
			i++;
		}
		fclose(fp);

		return Money;
	}
}

//Convert Answer to array index of Answer arrays
int AnswerToIndex(char Answer)
{
	int iAnswer;

	switch (toupper(Answer))
	{
	case 'A':
		iAnswer = 0;
		break;
	case 'B':
		iAnswer = 1;
		break;
	case 'C':
		iAnswer = 2;
		break;
	case 'D':
		iAnswer = 3;
		break;
	default:
		iAnswer = 0;
	}

	return iAnswer;
}

int CountIndex()
{
	struct NodeQuestion* temp = head;
	int i = 1;

	while (temp != NULL) {
		i++;
		temp = temp->next;
	}

	return i - 1;
}

void setConsoleSettings()
{
	TCHAR szOldTitle[MAX_PATH];
	TCHAR szNewTitle[MAX_PATH];

	if (GetConsoleTitle(szOldTitle, MAX_PATH))
	{
		StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Who Wants to Be a Millionaire?"));
		SetConsoleTitle(szNewTitle);
	}

	SMALL_RECT windowSize = { 0 , 0 , 114 , 49 };
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
}

//clearbuffer for scanf (if has input error it will only display 1 error not redundant)
void CleanBuffer()
{
	int n;
	while ((n = getchar()) != EOF && n != '\n');
}

void InsertNodeQuestion(char Question[256], char Answer, char ChoicesArray[4][256], int ChoicesSize, char Dialog[256]) 
{
	struct NodeQuestion* temp = head;
	struct NodeQuestion* newNode = GetNewNodeQuestion(Question, Answer, ChoicesArray, ChoicesSize, Dialog);

	if (head == NULL)
	{
		head = newNode;
		return;
	}
	while (temp->next != NULL)
	{
		temp = temp->next; // Go To last Node
	}
	temp->next = newNode;
	newNode->prev = temp;
}

void InsertAtIndexQuestion(int NodeIndex, int viewIndex)
{
	struct NodeQuestion* temp = head;
	int Counter = 1;

	while (temp != NULL)
	{
		if (Counter == NodeIndex + 1)
		{
			printf("NOTE: type 'cancel' to go back to menu\n");
			printf("Enter new Question: ");
			scanf_s(" %[^\n]s", Question, 256);

			//Allow user to skipp adding data
			if (strcmp(Question, "cancel") == 0)
			{
				viewQEditor();
			}

			printf("\n\nEnter desired answer for A,B,C and D \n");
			printf("\n");

			//loop for inserting choices
			for (int j = 0; j < ChoicesSize; j++)
			{
				printf("Choice %c: ", Letters[j]);
				scanf_s(" %[^\n]s", &ChoicesArray[j], 256);

				//Allow user to skipp adding data
				if (strcmp(ChoicesArray[j], "cancel") == 0)
				{
					viewQEditor();
				}
			}

			printf("\n");

			printf("NOTE: Choose from (A,B,C,D) upper or lower doesn't matter. \n");
			printf("NOTE: cancel feature is not available in this phase. \n");
			do
			{
				printf("What is the answer? ");
				scanf_s(" %c", &Answer, 1);

				if (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'))
				{
					printf("Invalid input please try again.\n");
					CleanBuffer();
				}
			} while (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'));


			printf("\nEnter Call a friend dialog for this question: ");
			scanf_s(" %[^\n]s", Dialog, 256);

			//Allow user to skipp adding data
			if (strcmp(Dialog, "cancel") == 0)
			{
				viewQEditor();
			}


			struct NodeQuestion* newNode = GetNewNodeQuestion(Question, Answer, ChoicesArray, ChoicesSize, Dialog);

			//selected node (prev) point to newnode
			(temp->prev)->next = newNode;
			//point newnode prev to prev node
			newNode->prev = (temp->prev);
			//point newnode next to selected node
			newNode->next = temp;
			//point selected node prev to newnode
			temp->prev = newNode;

			do
			{
				printf("\nInsert more question? (y|n): ");
				scanf_s(" %c", &YesNo, 1);

				//validate if the input has error
				if (toupper(YesNo) == 'Y')
				{
					viewInsert();
				}
				else if (toupper(YesNo) == 'N')
				{
					viewQEditor();
				}
				else
				{
					printf("Invalid input please try again.\n");
					CleanBuffer();
				}
			} while (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N');
			
		}
		Counter++;
		temp = temp->next;
	}
}

void InsertQuestion()
{
	printf("NOTE: type 'cancel' to go back to menu\n");
	printf("Enter new Question: ");
	scanf_s(" %[^\n]s", Question, 256);

	//Allow user to skipp adding data
	if (strcmp(Question, "cancel") == 0)
	{
		viewQEditor();
	}

	printf("\n\nEnter desired answer for A,B,C and D \n");
	printf("\n");

	//loop for inserting choices
	for (int i = 0; i < ChoicesSize; i++)
	{
		printf("Choice %c: ", Letters[i]);
		scanf_s(" %[^\n]s", &ChoicesArray[i], 256);

		//Allow user to skipp adding data
		if (strcmp(ChoicesArray[i], "cancel") == 0)
		{
			viewQEditor();
		}
	}

	printf("\n");

	printf("NOTE: Choose from (A,B,C,D) upper or lower doesn't matter. \n");
	printf("NOTE: cancel feature is not available in this phase. \n");
	do
	{
		printf("What is the answer?: ");
		scanf_s(" %c", &Answer, 1);

		if (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'))
		{
			printf("Invalid input please try again.\n");
			CleanBuffer();
		}
		CleanBuffer();
	} while (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'));

	printf("Enter Call a friend dialog for this question: ");
	scanf_s(" %[^\n]s", Dialog, 256);

	//Allow user to skipp adding data
	if (strcmp(Dialog, "cancel") == 0)
	{
		viewQEditor();
	}

	//Insert data to node
	InsertNodeQuestion(Question, Answer, ChoicesArray, ChoicesSize, Dialog);

	do
	{
		printf("\nDo you want to view updated question list? (y|n): ");
		scanf_s(" %c", &YesNo, 1);

		if (toupper(YesNo) == 'Y')
		{
			printf("\n\n");
			Print();
			printf("\n");
			CleanBuffer();
		}

		if (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N')
		{
			printf("Invalid input please try again.\n");
			CleanBuffer();
		}
	} while (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N');

	do
	{
		printf("\nAdd more question? (y|n): ");
		scanf_s(" %c", &YesNo, 1);

		//validate if the input has error
		if (toupper(YesNo) == 'Y')
		{
			CleanBuffer();
			InsertQuestion();
		}
		else if (toupper(YesNo) == 'N')
		{
			//save file
			CreateFileTxt();
			Sleep(500);
			viewQEditor();
		}
		else
		{
			printf("Invalid input please try again.\n");
			CleanBuffer();
		}

	} while (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N');
}

//need to optimize
void EditQuestion(int NodeIndex)
{
	struct NodeQuestion* temp = head;
	int i = 1;
	char nwQuestion[256];
	char cYesNo, aYesNo;

	while (temp != NULL)
	{
		if (i == NodeIndex)
		{
			printf("\nCurrent Question: %s\n", temp->nQuestion);
			printf("\nNOTE: type 'cancel' to go back to menu\n");
			printf("NOTE: type 'skip' to skip modifying question\n");
			printf("Enter your modified question: ");
			scanf_s(" %[^\n]s", nwQuestion, 256);

			//Allow user to skip adding data
			if (strcmp(nwQuestion, "cancel") == 0)
			{
				viewQEditor();
			}
			else if (strcmp(nwQuestion, "skip") == 0)
			{
				strcpy_s(nwQuestion, temp->nQuestion);
			}

			//modify choices
			do
			{
				printf("\nDo you want to modify choices? (type in: y|n): ");
				scanf_s(" %c", &cYesNo, 1);
				printf("\n\n");

				if (toupper(cYesNo) == 'Y')
				{
					printf("Enter desired answer for A,B,C and D \n");

					//loop for inserting choices
					for (int i = 0; i < ChoicesSize; i++)
					{
						printf("Current Choice %c: %s\n", Letters[i], temp->nChoices[i]);
						printf("Choice %c: ", Letters[i]);
						scanf_s(" %[^\n]s", &ChoicesArray[i], 256);

						//Allow user to skipp adding data
						if (strcmp(ChoicesArray[i], "cancel") == 0)
						{
							CleanBuffer();
							break;
						}
						else if (strcmp(ChoicesArray[i], "skip") == 0)
						{
							strcpy_s(ChoicesArray[i], temp->nChoices[i]);
						}
					}
				}
				else if (toupper(cYesNo) == 'N')
				{
					CleanBuffer();
					break;
				}
				else
				{
					printf("\nInvalid input please try again.\n");
					CleanBuffer();
				}
			} while (toupper(cYesNo) != 'Y' && toupper(cYesNo) != 'N');

			printf("\n");

			//modify answer
			do
			{
				printf("Do you want to modify answer? (type in: y|n): ");
				scanf_s(" %c", &aYesNo, 1);

				if (toupper(aYesNo) == 'Y')
				{
					printf("NOTE: Choose from (A,B,C,D) upper or lower doesn't matter. \n");
					do
					{
						printf("What is the answer? ");
						scanf_s(" %c", &Answer, 1);

						if (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'))
						{
							printf("Invalid input please try again.\n");
							CleanBuffer();
						}
						else
						{
							temp->nAnswer = toupper(Answer);
						}
					} while (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'));
				}
				else if (toupper(aYesNo) == 'N')
				{
					Answer = temp->nAnswer;
				}
				else
				{
					printf("\nInvalid input please try again.\n");
					CleanBuffer();
				}
			} while (toupper(aYesNo) != 'Y' && toupper(aYesNo) != 'N');

			//save record
			if (strcmp(nwQuestion, "skip") < 0 || cYesNo == 'Y' || aYesNo == 'Y')
			{
				do
				{
					printf("Are you sure you want to save this record? (y|n): ");
					scanf_s(" %c", &YesNo, 1);

					if (toupper(YesNo) == 'Y')
					{
						CreateFileTxt();
						strcpy_s(header, "Do you want to show the updated list of questions? (y|n) :");
						PrintList(false, header);
					}
					else if (toupper(YesNo) == 'N')
					{
						DeleteNodes();
						InitFile();
						viewQEditor();
					}
					else
					{
						printf("\nInvalid input please try again.\n");
						CleanBuffer();
					}
				} while (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N');
			}
			else
			{
				break;
			}
		}
		i++;
		temp = temp->next;
	}
}

void DeleteQuestion(int NodeIndex)
{
	struct NodeQuestion* temp = head;
	int i = 1;

	//iterate through records/linkedlist
	while (temp != NULL)
	{
		//validate if selected node
		if (i == NodeIndex)
		{
			printf("Current Question: %s\n", temp->nQuestion);

			do
			{
				printf("\nAre you sure you want to delete this record? (y|n): ");
				scanf_s(" %c", &YesNo, 1);

				if (toupper(YesNo) == 'Y')
				{
					CleanBuffer();
					if (temp != head)
					{
						//validate if ahead node is null or if its head
						if ((temp->prev) != NULL)
						{
							//point to prev of selected next node
							(temp->prev)->next = temp->next;
						}

						//validate if next is null
						if ((temp->next) != NULL)
						{
							//point to next of selected prev node
							(temp->next)->prev = temp->prev;
						}

						//free or delete mem
						free(temp);
					}
					else
					{
						//set new head to next node since the original head has been deleted
						head = head->next;
						//free or delete mem
						free(temp);
					}
					break;
				}
				else
				{
					viewQEditor();
				}

				if (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N')
				{
					printf("Invalid input please try again.\n");
					CleanBuffer();
				}
			} while (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N');

			strcpy_s(header, "Do you want to show the updated list of questions? (y|n) :");
			PrintList(true, header);

			do
			{
				printf("\nDo you want to delete more record? (y|n): ");
				scanf_s(" %c", &YesNo, 1);

				if (toupper(YesNo) == 'Y')
				{
					viewDelete();
				}
				else if (toupper(YesNo) == 'N')
				{
					break;
				}

				if (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N')
				{
					printf("Invalid input please try again.\n");
					CleanBuffer();
				}

			} while (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N');

			break;
		}
		i++;
		temp = temp->next;
	}
}

void Print() 
{
	struct NodeQuestion* temp = head;
	int i = 1;

	while (temp != NULL) {
		printf("Q%i: %s :Answer: %c \n", i, temp->nQuestion, temp->nAnswer);

		for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
		{
			printf("%c:) %s\t", Letters[i], temp->nChoices[i]);
		}

		printf("\n\n#########################################################");
		printf("##########################################################\n\n");

		i++;
		temp = temp->next;
	}
	
	printf("\nTotal of %i questions.\n\n", i-1);
}

//debug purposes
void PrintReversed(char header[256])
{
	struct NodeQuestion* temp = head;

	if (header != NULL)
	{
		printf("%s \n", header);
	}

	while (temp->next != NULL)
	{
		temp = temp->next; //Go to last node
	}


	while (temp != NULL) {
		printf("Question: %s \n", temp->nQuestion);

		for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
		{
			printf("%c:) %s\t", Letters[i], temp->nChoices[i]);
		}

		printf("\nAnswer: %c\n", temp->nAnswer);

		temp = temp->prev;
	}

	printf("\n");
}

void DeleteNodes()
{
	struct NodeQuestion* temp;

	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp);
	}
}

void InitFile()
{
	FILE* fp;
	fopen_s(&fp, "Questions.txt", "r");
	char line[1024];
	int i = 0;
	char Question[256];
	char ChoicesArray[4][256];
	char Answer;
	char Dialog[256];

	if (fp)
	{
		while (fgets(line, 1024, fp))
		{
			int k = 0;
			if(i > 0)
			{
				/*
				for (int j = 1; j < 7; j++)
				{
					char* tmp = _strdup(line);
					if (j == 6)
					{
						printf("%s", getfield(tmp, j));
					}
					else
					{
						printf("%s,", getfield(tmp, j));
					}
					
					// NOTE strtok clobbers tmp
					free(tmp);
				}
				*/

				//construct choices array
				for (int j = 1; j < 8; j++)
				{
					char* tmp = _strdup(line);
					//printf("%i\n", j);
					if (j == 1)
					{
						strcpy_s(Question, getfield(tmp, j));
						//printf("%s\n", Question);
						//printf("%s", getfield(tmp, j));
					}
					else if (j == 2)
					{
						Answer = getfield(tmp, j)[0];
						//printf(",%c\n", Answer);
					}
					else if (j >= 3 && j <= 6)
					{
						//printf(",%s\n", getfield(tmp, j));
						//printf("INDEX %i\n", k);
						strcpy_s(ChoicesArray[k], getfield(tmp, j));
						//printf(",%s\n", ChoicesArray[k]);
						k++;
					}
					else
					{
						//printf(",%s\n", getfield(tmp, j));
						strcpy_s(Dialog, getfield(tmp, j));
					}

					//printf("%s", getfield(tmp, i));
					free(tmp);
				}

				//debug
				/*
				printf("%s\n", Question);
				printf("%c\n", Answer);
				printf("%s\n", ChoicesArray[0]);
				printf("%s\n", ChoicesArray[1]);
				printf("%s\n", ChoicesArray[2]);
				printf("%s\n", ChoicesArray[3]);
				*/
				

				//Insert data to node
				InsertNodeQuestion(Question, Answer, ChoicesArray, 4, Dialog);

				//printf("\n");
				//printf("------------------------------\n");
			}
			i++;
		}
		fclose(fp);
	}
}

void CreateFileTxt() 
{
	FILE* fp;
	struct NodeQuestion* temp = head;

	printf("\nPlease wait... saving file...\n");

	fopen_s(&fp, "Questions.txt", "w");

	fprintf(fp, "Question;Answer;cA;cB;cC;cD");

	while (temp != NULL) {
		fprintf(fp, "\n%s", temp->nQuestion);
		fprintf(fp, ";%c", temp->nAnswer);

		for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
		{
			fprintf(fp, ";%s", temp->nChoices[i]);
		}

		fprintf(fp, ";%s", temp->nDialog);

		temp = temp->next;
	}

	fclose(fp);
}

void AnswerTheQuestion(int QNum, char Ans)
{
	printf("\nNOTE: Choose from (A,B,C,D) upper or lower doesn't matter. \n");

	do
	{
		printf("What is your answer? ");
		scanf_s(" %c", &Answer, 1);
		
		if (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'))
		{
			printf("\nInvalid input please try again.\n");
			CleanBuffer();
		}
		else
		{
			if (Ans == toupper(Answer))
			{
				currentQuestion++;
				PrintNextLevel();
				Sleep(1000);
				if (currentQuestion <= 15)
				{
					PlayGame(currentQuestion);
				}
				else
				{
					PrintGameOver(QNum + 1, 2);
				}
			}
			else
			{
				CleanBuffer();
				PrintGameOver(QNum - 1, 2);
			}
		}
	} while (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'));
}

void PlayGame(int QNum)
{
	struct NodeQuestion* temp = head;
	int k = 1;
	
	int GameChoice;
	int llChoice = 0; //lifelineChoice 1- ll50 2-llCaF 3-llAta

	while (temp != NULL) {
		if (k == QNum)
		{
			CleanBuffer();
			system("cls");

			PrintLogo();

			printf("\n\n");

			printf("#########################################################");
			printf("##########################################################\n\n");

			PrintGameInfo(ll50, llCaF, llAtA, k);

			printf("#########################################################");
			printf("##########################################################\n\n");

			PrintQHeader(QNum);

			printf("\n\n");
			printf("\t\t%s\n\n", temp->nQuestion);
			printf("#########################################################");
			printf("##########################################################\n\n");

			printf("\t\t\t");
			for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
			{
				printf("%c:) %s\t", Letters[i], temp->nChoices[i]);
			}

			printf("\n\n");
			printf("#########################################################");
			printf("##########################################################\n\n");

			do
			{
				printf("Choose option: 1 - Answer | 2 - Use Lifelines | 3 - Go home: ");
				scanf_s(" %i", &GameChoice);

				if (GameChoice == 1)
				{
					AnswerTheQuestion(QNum,temp->nAnswer);
				}
				else if (GameChoice == 2)
				{
					do
					{
						printf("\nChoose Lifeline: 1 - 50/50 | 2 - Call a Friend | 3 - Ask the Audience | 4 - Cancel: ");
						scanf_s(" %i", &llChoice);

						if (llChoice == 1)
						{
							if (ll50 > 0)
							{
								printf("\nYou have choose lifeline: 50/50\n");
								printf("Here is the remaining choices below:\n\n");

								ll50--;
								int TwoWrong = 1;
								for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
								{
									if (TwoWrong >= i || temp->nAnswer == Letters[i])
									{
										printf("%c:) %s\t", Letters[i], temp->nChoices[i]);
									}
								}

								printf("\n\n");
								AnswerTheQuestion(QNum, temp->nAnswer);
							}
							else
							{
								printf("\nYou can't use this anymore.\n");
								Sleep(500);
								PlayGame(QNum);
							}
						}
						else if (llChoice == 2)
						{
							if (llCaF > 0)
							{
								printf("\nYou have choose lifeline: Call a Friend\n");
								printf("\nRing! ring! ring!...\n");
								printf("\nWe've call your friend and this is the response...\n\n");

								printf("Friend: \"%s\"\n\n", temp->nDialog);

								llCaF--;

								printf("\n\n");
								AnswerTheQuestion(QNum, temp->nAnswer);
							}
							else
							{
								printf("\nYou can't use this anymore.\n");
								Sleep(500);
								PlayGame(QNum);
							}
						}
						else if (llChoice == 3)
						{
							if (llAtA > 0)
							{
								printf("\nYou have choose lifeline: Ask the Audience\n");
								printf("Here's the percentage per choices:\n\n");

								int n = 0, m = 100;

								for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
								{
									srand(time(0));
									n = rand() % m + 1;
									if (i == 3)
									{
										printf("%d%% - %c:) %s\t", m, Letters[i], temp->nChoices[i]);
									}
									else
									{
										printf("%d%% - %c:) %s\t", n, Letters[i], temp->nChoices[i]);
									}
									m = m - n;
								}

								llAtA--;

								printf("\n\n");
								AnswerTheQuestion(QNum, temp->nAnswer);
							}
							else
							{
								printf("\nYou can't use this anymore.");
								Sleep(500);
								PlayGame(QNum);
							}
						}
						else if (llChoice == 4)
						{
							PlayGame(QNum);
						}
						else
						{
							printf("Invalid input. please try again.\n\n");
							Sleep(500);
							//CleanBuffer();
							PlayGame(QNum);
						}
					} while (ll50 < 0 || llCaF < 0 || llAtA < 0 && (llChoice < 4 && llChoice > 0));
				}
				else if (GameChoice == 3)
				{
					printf("\n\n");
					PrintGameOver(QNum - 1, 1);
				}
				else
				{
					printf("Invalid input. please try again.\n\n");
					Sleep(500);
					CleanBuffer();
				}
			} while (GameChoice <= 0 || GameChoice > 3);
		}

		k++;
		temp = temp->next;
	}
}

//prints
void PrintLogo()
{
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("\t\t\t\t$$\t\t\t\t\t\t$$\n");
	printf("\t\t\t\t$$\t   Who Wants to Be a Millionaire  \t$$\n");
	printf("\t\t\t\t$$\t\t\t\t\t\t$$\n");
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
}

void PrintHeader(int viewIndex)
{
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");


	if (viewIndex == 1)
	{
		printf("\t\t\t\t$$\t\t QUESTION EDITOR      \t\t$$\n");
		printf("\t\t\t\t$$\t\t (ADD QUESTION)  \t\t$$\n");
	}
	else if (viewIndex == 2)
	{
		printf("\t\t\t\t$$\t\t QUESTION EDITOR      \t\t$$\n");
		printf("\t\t\t\t$$\t\t (EDIT QUESTION)  \t\t$$\n");
	}
	else if (viewIndex == 3)
	{
		printf("\t\t\t\t$$\t\t QUESTION EDITOR      \t\t$$\n");
		printf("\t\t\t\t$$\t\t(DELETE QUESTION)   \t\t$$\n");
	}
	else if (viewIndex == 4)
	{
		printf("\t\t\t\t$$\t\t QUESTION EDITOR      \t\t$$\n");
		printf("\t\t\t\t$$\t\t(INSERT QUESTION)   \t\t$$\n");
	}
	else if (viewIndex == 5)
	{
		printf("\t\t\t\t$$\t\t QUESTION EDITOR      \t\t$$\n");
		printf("\t\t\t\t$$\t\t     (LIST)     \t\t$$\n");
	}
	else if (viewIndex == 6)
	{
		printf("\t\t\t\t$$\t\t QUESTION EDITOR      \t\t$$\n");
	}
	else
	{
		printf("\t\t\t\t$$\t\t   MAIN MENU  \t\t\t$$\n");
	}
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
}

void PrintQHeader(int QuestionIndex)
{
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("\t\t\t\t$$\t\t   QUESTION #%i  \t\t$$\n", QuestionIndex);
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
}

void PrintNextLevel()
{
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("\t\t\t\t$$\t\t    CORRECT!     \t\t$$\n");
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
}

void PrintGameOver(int MoneyIndex, int Gtype)
{
	printf("\n\n");
	printf("#########################################################");
	printf("##########################################################\n\n");
	printf("\t\t\t\t\t\t  GAMEOVER!\n\n");

	if (MoneyIndex > 0)
	{
		if (Gtype == 1)
		{
			printf("\t\t\t\t\t\t You've won $%s!!!\n", GetCurrentMoney(MoneyIndex));
		}
		else
		{
			if (MoneyIndex >= 5 && MoneyIndex < 10)
			{
				printf("\t\t\t\t\t\t You've won $%s!!!\n", GetCurrentMoney(5));
			}
			else if (MoneyIndex >= 10 && MoneyIndex < 15)
			{
				printf("\t\t\t\t\t\t You've won $%s!!!\n", GetCurrentMoney(10));
			}
			else if (MoneyIndex == 16)
			{
				printf("\t\t\t\t\t\t You've won $%s!!!\n", GetCurrentMoney(15));
			}
		}
	}

	printf("#########################################################");
	printf("##########################################################\n\n");

	do
	{
		printf("\nDo you want to play again? (y|n): ");
		scanf_s(" %c", &YesNo, 1);

		if (toupper(YesNo) == 'Y')
		{
			//re initialize values
			ll50 = 1; //lifeline 50/50
			llCaF = 1; //lifeline Call a Friend
			llAtA = 1; //lifeline Ask the Audience
			currentQuestion = 1;

			Sleep(500);
			currentQuestion = 1;
			CleanBuffer();
			system("cls");
			viewMainMenu();
		}
		else if (toupper(YesNo) == 'N')
		{
			exit(0);
		}
		else
		{
			CleanBuffer();
			printf("\nInvalid input please try again.\n");
		}

	} while (toupper(YesNo) != 'Y' && toupper(YesNo) != 'N');

}

void PrintGameInfo(int ll50, int llCaF, int llATA, int current)
{
	FILE* fp;
	fopen_s(&fp, "Money.txt", "r");

	char line[1024];
	int i = 1;


	//printf("\t\t\t\t\t$$$$$ Available Lifelines $$$$$\n\n");

	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("\t\t\t\t$$\t\tAvailable Lifelines   \t\t$$\n");
	printf("\t\t\t\t$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");

	if (ll50 > 0)
	{
		printf("\033[0;32m");
		printf("\t\t\t\t(%i) 50/50 ", ll50);
		printf("\033[0m");
	}
	else
	{
		printf("\t\t\t\t(%i) 50/50 ", ll50);
	}

	if (llCaF > 0)
	{
		printf("\033[0;32m");
		printf("| (%i) Call a Friend ", ll50);
		printf("\033[0m");
	}
	else
	{
		printf("| (%i) Call a Friend ", ll50);
	}

	if (llATA > 0)
	{
		printf("\033[0;32m");
		printf("| (%i) Ask the Audience", llATA);
		printf("\033[0m");
	}
	else
	{
		printf("| (%i) Ask the Audience", llATA);
	}



	//printf("\t\t\t\t(%i) 50/50 | (%i) Call a Friend | (%i) Ask the Audience", ll50, llCaF, llATA);

	printf("\n\n\t\t\t");
	if (fp)
	{

		while (fgets(line, 1024, fp))
		{
			char* tmp = _strdup(line);

			if (i == 8)
			{
				printf("\n\t\t");
			}

			if (i == current)
			{
				printf("\033[0;32m");
				printf("$%-10s", getfield(tmp, 2));
				printf("\033[0m");
			}
			else
			{
				if (i == 5 || i == 10 || i == 15)
				{
					printf("\033[0;33m");
					printf("$%-10s", getfield(tmp, 2));
					printf("\033[0m");
				}
				else
				{
					printf("$%-10s", getfield(tmp, 2));
				}
			}

			free(tmp);
			i++;
		}
		fclose(fp);
	}

	printf("\n\n");
}

void PrintList(bool head, char header[256])
{
	int Choice;
	do
	{
		printf("%s", header);
		scanf_s(" %c", &YesNo, 1);

		if (toupper(YesNo) == 'Y')
		{
			printf("\n\n");
			Print();
			printf("\n");
			CleanBuffer();
			break;
		}
		else if (toupper(YesNo) == 'N')
		{
			CleanBuffer();
			break;
		}
		else
		{
			printf("Invalid input. please try again.\n\n");
			Sleep(500);
			CleanBuffer();
		}
	} while (toupper(YesNo) != 'Y' || toupper(YesNo) != 'N');

	if (!head)
	{
		printf("Choose option: 1 - Go Back ");
		scanf_s("%i", &Choice);

		if (Choice == 1)
		{
			viewQEditor();
		}
	}
}

//views

//viewIndex = 1
void viewAdd()
{
	int addChoice = 0;

	CleanBuffer();
	system("cls");

	PrintLogo();

	printf("\n\n");

	PrintHeader(1);

	printf("\n\n");

	InsertQuestion();
}

//viewIndex = 2
void viewEdit()
{
	int EditIndex;

	CleanBuffer();
	system("cls");

	PrintLogo();

	printf("\n\n");

	PrintHeader(2);

	printf("\n\n");

	strcpy_s(header,"Do you want to show the list of questions? (y|n) :");
	PrintList(true, header);

	printf("\nNOTE: Enter zero(0) to go back to main menu\n");
	printf("Enter question index from 1 to %i :", CountIndex());
	scanf_s("%i", &EditIndex);
	printf("\n");

	if (EditIndex > 0 && EditIndex <= CountIndex())
	{
		EditQuestion(EditIndex);
		Sleep(500);
		viewQEditor();
	}
	else if (EditIndex == 0)
	{
		viewQEditor();
	}
	else
	{
		printf("Invalid input. please try again.\n\n");
		Sleep(500);
		CleanBuffer();
		system("cls");
		Print();
		viewEdit();
	}
}

//viewIndex = 3
void viewDelete()
{
	int DelIndex;

	CleanBuffer();
	system("cls");

	PrintLogo();

	printf("\n\n");

	PrintHeader(3);

	printf("\n\n");

	strcpy_s(header,"Do you want to show the list of questions? (y|n) :");
	PrintList(true, header);

	printf("\nNOTE: Enter zero(0) to go back to main menu\n");
	do
	{
		printf("Enter question index from 1 to %i :", CountIndex());
		scanf_s("%i", &DelIndex);

		if (DelIndex > 0 && DelIndex <= CountIndex())
		{
			DeleteQuestion(DelIndex);
			CreateFileTxt();
			Sleep(500);
			viewQEditor();
		}
		else if (DelIndex == 0)
		{
			viewQEditor();
		}
		else
		{
			printf("Invalid input. please try again.\n\n");
			Sleep(500);
			CleanBuffer();
		}
	} while (DelIndex < 0 || DelIndex > CountIndex());
	
}

//viewIndex = 4
void viewInsert()
{
	int InsertIndex;

	CleanBuffer();
	system("cls");

	PrintLogo();

	printf("\n\n");

	PrintHeader(4);

	printf("\n\n");

	strcpy_s(header,"Do you want to show the list of questions? (y|n) :");
	PrintList(true, header);

	do
	{
		printf("\nNOTE: Enter zero(0) to go back to main menu\n");
		printf("Enter index number from 1 to %i:", CountIndex());
		scanf_s("%i", &InsertIndex);

		printf("\n");

		if (InsertIndex > 0 && InsertIndex <= CountIndex())
		{
			InsertAtIndexQuestion(InsertIndex, 4);
			CreateFileTxt();
			strcpy_s(header, "Do you want to show the updated list of questions? (y|n) :");
			PrintList(false, header);
			Sleep(500);
			viewQEditor();
		}
		else if (InsertIndex == 0)
		{
			viewQEditor();
		}
		else
		{
			printf("Invalid input. please try again.\n\n");
			Sleep(500);
			CleanBuffer();
		}
	} while (InsertIndex > CountIndex() || InsertIndex < 0);
}

//viewIndex = 5
void viewPrint()
{
	int printChoice = 0;
	int Start = 1;
	int End = 5;

	CleanBuffer();
	system("cls");

	PrintLogo();

	printf("\n\n");

	PrintHeader(5);

	printf("\n\n");

	Print();

	printf("Choose option: 1 - Go back :");
	scanf_s("%i", &printChoice);

	if (printChoice <= 0 || printChoice > 5)
	{
		printf("Invalid input. please try again.\n\n");
		Sleep(500);
		CleanBuffer();
		system("cls");
		viewPrint();
	}
	else
	{
		viewQEditor();
	}
}

void viewQEditor()
{
	int settingChoice = 0;

	CleanBuffer();
	system("cls");

	PrintLogo();

	printf("\n\n");

	PrintHeader(6);

	printf("\n\n");

	printf("Choose option: 1 - Add New | 2 - Insert At | 3 - Edit | 4 - Delete | 5 - List down | 6 - Main Menu :");
	scanf_s("%i", &settingChoice);

	if (settingChoice <= 0 || settingChoice > 6)
	{
		printf("Invalid input. please try again.\n\n");
		Sleep(500);
		CleanBuffer();
		system("cls");
	}
	else
	{
		if (settingChoice == 1)
		{
			viewAdd();
		}
		else if (settingChoice == 2)
		{
			viewInsert();
		}
		else if (settingChoice == 3)
		{
			viewEdit();
		}
		else if (settingChoice == 4)
		{
			viewDelete();
		}
		else if (settingChoice == 5)
		{
			viewPrint();
		}
		else if (settingChoice == 6)
		{
			//go back to main menu
			CleanBuffer();
			system("cls");
			viewMainMenu();
		}
	}
}

void viewMainMenu()
{
	int gameChoice = 0;

	do
	{
		PrintLogo();

		printf("\n\n");

		PrintHeader(0); 
		printf("\n\n");

		printf("Choose option: 1 - Play Game | 2 - Question Editor | 3 - Quit Game :");
		scanf_s("%i", &gameChoice);

		if (gameChoice <= 0 || gameChoice > 3)
		{
			printf("Invalid input. please try again.\n\n");
			Sleep(500);
			CleanBuffer();
			system("cls");
		}
		else
		{
			if (gameChoice == 1)
			{
				PlayGame(currentQuestion);
			}
			else if (gameChoice == 2)
			{
				viewQEditor();
			}
			else if (gameChoice == 3)
			{
				exit(0);
			}
		}
	} while (gameChoice >= 0);
}

int main()
{
	/*
	int n = 0,m = 100;
	for (int i = 0; i <= 3; i++)
	{
		srand(time(0));
		n = rand() % m + 1;
		//printf("N = %d\n", n);
		//printf("M = %d\n", m);
		if (i == 3)
		{
			printf("%d%%\t", m);
		}
		else
		{
			printf("%d%%\t", n);
			
		}
		m = m - n;
	}
	*/

	//char test[4][6][256] = { { "test123", "B", "123", "qwe", "asd", "zxc" }, { "test321", "B", "123", "qwe", "asd", "zxc" }, { "test456", "B", "123", "qwe", "asd", "zxc" } };
	//system("cls");

	setConsoleSettings();

	InitFile();

	viewMainMenu();

}
