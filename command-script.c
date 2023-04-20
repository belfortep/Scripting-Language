#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#define MAX_SOURCE_LINE_SIZE 4096
#define MAX_COMMAND_SIZE 4096
#define MAX_PARAM_SIZE 4096
#define COMMAND_PRINTSTRING "PrintString"
#define COMMAND_PRINTSTRINGLOOP "PrintStringLoop"
#define COMMAND_NEWLINE "Newline"
#define COMMAND_WAITFORKEYPRESS "WaitForKeyPress"

int g_iScriptSize = 0;
char **g_ppstrScript;
int g_iCurrScriptLine = 0;
int g_iCurrScriptLineChar;
void UnloadScript()
{
        // Return immediately if the script is already free
        if (!g_ppstrScript)
                return;
        // Free each line of code individually
        for (int iCurrLineIndex = 0;
             iCurrLineIndex < g_iScriptSize;
             ++iCurrLineIndex)
                free(g_ppstrScript[iCurrLineIndex]);
        // Free the script structure itself
        free(g_ppstrScript);
}

void LoadScript(char *pstrFilename)
{
        // Create a file pointer for the script
        FILE *pScriptFile;
        // ---- Find out how many lines of code the script is
        // Open the source file in binary mode
        if (!(pScriptFile = fopen(pstrFilename, "rb")))
        {
                printf("File I/O error.\n");
                exit(0);
        }
        // Count the number of source lines
        while (!feof(pScriptFile))
                if (fgetc(pScriptFile) == '\n')
                        ++g_iScriptSize;
        ++g_iScriptSize;
        // Close the file
        fclose(pScriptFile);
        // ---- Load the script
        // Open the script and print an error if it's not found
        if (!(pScriptFile = fopen(pstrFilename, "r")))
        {
                printf("File I/O error.\n");
                exit(0);
        }                                                                // Allocate a script of the proper size
        g_ppstrScript = (char **)malloc(g_iScriptSize * sizeof(char *)); // claro, me armo un array de los comandos (que son char *)
        // Load each line of code
        for (int iCurrLineIndex = 0;
             iCurrLineIndex < g_iScriptSize;
             ++iCurrLineIndex)
        {
                // Allocate space for the line and a null terminator
                g_ppstrScript[iCurrLineIndex] = (char *)malloc(MAX_SOURCE_LINE_SIZE + 1); // aca guardo espacio para ellos
                // Load the line
                fgets(g_ppstrScript[iCurrLineIndex], MAX_SOURCE_LINE_SIZE, pScriptFile);
        }
        // Close the script
        fclose(pScriptFile);
}

void GetCommand(char *pstrDestString)
{
        // Keep track of the command's length
        int iCommandSize = 0;
        // Create a space for the current character
        char cCurrChar;
        // Read all characters until the first space to isolate the command
        while (g_iCurrScriptLineChar <
               (int)strlen(g_ppstrScript[g_iCurrScriptLine]))
        {
                // Read the next character from the line
                cCurrChar = g_ppstrScript[g_iCurrScriptLine][g_iCurrScriptLineChar];
                // If a space (or newline) has been read, the command is complete
                if (cCurrChar == ' ' || cCurrChar == '\n')
                        break;
                // Otherwise, append it to the current command
                pstrDestString[iCommandSize] = cCurrChar;
                // Increment the length of the command
                ++iCommandSize; // Move to the next character in the current line
                ++g_iCurrScriptLineChar;
        }
        // Skip the trailing space
        ++g_iCurrScriptLineChar;
        // Append a null terminator
        pstrDestString[iCommandSize] = '\0';
        // Convert it all to uppercase
        strupr(pstrDestString);
}

void GetStringParam(char *pstrDestString)
{
        // Keep track of the parameter's length
        int iParamSize = 0;
        // Create a space for the current character
        char cCurrChar; // Move past the opening double quote
        ++g_iCurrScriptLineChar;
        // Read all characters until the closing double quote to isolate
        // the string
        while (g_iCurrScriptLineChar <
               (int)strlen(g_ppstrScript[g_iCurrScriptLine]))
        {
                // Read the next character from the line
                cCurrChar = g_ppstrScript[g_iCurrScriptLine][g_iCurrScriptLineChar];
                // If a double quote (or newline) has been read, the command
                // is complete
                if (cCurrChar == '"' || cCurrChar == '\n')
                        break;
                // Otherwise, append it to the current command
                pstrDestString[iParamSize] = cCurrChar;
                // Increment the length of the command
                ++iParamSize;
                // Move to the next character in the current line
                ++g_iCurrScriptLineChar;
        }
        // Skip the trailing space and double quote
        g_iCurrScriptLineChar += 2;
        // Append a null terminator
        pstrDestString[iParamSize] = '\0';
}

int GetIntParam()
{
        // Create some space for the integer's string representation
        char pstrString[MAX_PARAM_SIZE];
        // Keep track of the parameter's length
        int iParamSize = 0;
        // Create a space for the current character
        char cCurrChar;
        // Read all characters until the next space to isolate the integer
        while (g_iCurrScriptLineChar <
               (int)strlen(g_ppstrScript[g_iCurrScriptLine]))
        {
                // Read the next character from the line
                cCurrChar = g_ppstrScript[g_iCurrScriptLine][g_iCurrScriptLineChar];
                // If a space (or newline) has been read, the command is complete
                if (cCurrChar == ' ' || cCurrChar == '\n')
                        break; // Otherwise, append it to the current command
                pstrString[iParamSize] = cCurrChar;
                // Increment the length of the command
                ++iParamSize;
                // Move to the next character in the current line
                ++g_iCurrScriptLineChar;
        }
        // Move past the trailing space
        ++g_iCurrScriptLineChar;
        // Append a null terminator
        pstrString[iParamSize] = '\0';
        // Convert the string to an integer
        int iIntValue = atoi(pstrString);
        // Return the integer value
        return iIntValue;
}

void RunScript()
{
        // Allocate strings for holding source substrings
        char pstrCommand[MAX_COMMAND_SIZE];
        char pstrStringParam[MAX_PARAM_SIZE];
        // Loop through each line of code and execute it
        for (g_iCurrScriptLine = 0;
             g_iCurrScriptLine < g_iScriptSize;
             ++g_iCurrScriptLine)
        {
                // ---- Process the current line
                // Reset the current character
                g_iCurrScriptLineChar = 0;
                // Read the command
                GetCommand(pstrCommand);
                // ---- Execute the command
                // PrintString
                if (stricmp(pstrCommand, COMMAND_PRINTSTRING) == 0)
                {
                        // Get the string
                        GetStringParam(pstrStringParam);
                        // Print the string
                        printf("\t:%s\n", pstrStringParam);
                }

                // PrintStringLoop
                else if (stricmp(pstrCommand, COMMAND_PRINTSTRINGLOOP) == 0)
                {
                        // Get the string
                        GetStringParam(pstrStringParam);
                        // Get the loop count
                        int iLoopCount = GetIntParam();
                        // Print the string the specified number of times
                        for (int iCurrString = 0; iCurrString < iLoopCount;
                             ++iCurrString)
                                printf("\t%d: %s\n", iCurrString, pstrStringParam);
                }
                // Newline
                else if (stricmp(pstrCommand, COMMAND_NEWLINE) == 0)
                {
                        // Print a newline
                        printf("\n");
                }
                // WaitForKeyPress
                else if (stricmp(pstrCommand, COMMAND_WAITFORKEYPRESS) == 0)
                {
                        // Suspend execution until a key is pressed
                        while (kbhit())
                                getch();
                        while (!kbhit())
                                ;
                }
                // Anything else is invalid
                else
                {
                        printf("\tError: Invalid command.\n");
                        break;
                }
        }
}
//esta bueno, puedo usar todas las funciones menos los if dentro del RunScript.
int main()
{

        LoadScript("tuki.txt");
        RunScript();
        UnloadScript();
}