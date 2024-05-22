//23. В   некотором   компиляторе   ПАСКАЛя  текст  программы
//включает примечания, выделенные  фигурными  скобками  '{', '}'
//либо  парами  символов  '(*'  и  '*)'.Примечания  могут быть
//вложенными друг в друга.Если примечание открыто знаком  '{',
//то оно должно быть закрыто знаком '}'.Аналогично примечание,
//начинающееся с символов '(*'  должно  заканчиваться  символами
//'*)'.Требуется:
//1) проверить правильность вложенности примечаний;
//2) переписать   файл   с   исходным   текстом   так, чтобы
//отсутствовала  вложенность  комментариев  при  сохранении   их
//содержания  и  в  качестве  ограничивающих  символов  остались
//только  фигурные  скобки.Учесть   случай, когда   символы
//примечаний находятся в апострофах.При некорректности  указать
//номера строки и позиции первой ошибки(10).

//Баранов Антон Сергеевич ПС-23

#include <iostream>
#include <fstream>
#include <string>

struct StackElt
{
    char sBracketType;
    StackElt* sNext;
};

void ErrorHandling(StackElt* stack, int line, int pos)
{
    std::cout << "An error has occured in line " << line << ", position " << pos << ". ";

    if (!stack)
    {
        std::cout << "No opening bracket found" << std::endl;
    }
    else
    {
        switch (stack->sBracketType)
        {
        case '{':
            std::cout << "Expected '}'" << std::endl;
            break;
        case '(':
            std::cout << "Expected '*)'" << std::endl;
            break;
        }
    }     
}

wchar_t ReadSymbol(wchar_t inputChar, int &line, int &pos)
{
    if (inputChar != L'\n')
        pos++;
    else
    {
        line++;
        pos = 0;
    }

    return inputChar;
}

void SPush(StackElt* &stack, char bracketType)
{
    StackElt* temp = new StackElt;
    temp->sBracketType = bracketType;
    temp->sNext = stack;
    stack = temp;
}

bool SPop(StackElt* &stack, char bracketType)
{
    if (!stack)
    {
        return false;
    }
    else
    {
        switch (bracketType)
        {
            case '}':
                if (stack->sBracketType != '{')
                    return false;
                break;

            case ')':
                if (stack->sBracketType != '(')
                    return false;
                break;
        }
    }
    StackElt* temp = stack;
    stack = stack->sNext;
    delete temp;
    return true;
}

void SPrint(StackElt* stack)
{
    StackElt* temp = stack;
    while (temp)
    {
        std::cout << temp->sBracketType << " ";
        temp = temp->sNext;
    }
    std::cout;
}

void SClear(StackElt* stack)
{
    while (stack)
    {
        StackElt* temp = stack;
        stack = stack->sNext;
        delete temp;
    }
}

void DeleteReduntantComments(std::wstring& inputString, std::wofstream& outputFile)
{
    StackElt* sTop = 0;
    bool apostropheSkip = false;

    for (size_t i = 0; i < inputString.length() - 1; i++)
    {
        if (inputString[i] == L'\'')
            apostropheSkip = !apostropheSkip;

        if (apostropheSkip != true)
        {
            switch (inputString[i])
            {
            case L'{':
                if (!sTop)
                    outputFile << L'{';
                SPush(sTop, '{');
                break;

            case L'(':
                if (inputString[i + 1] == L'*')
                {
                    i++;
                    if (!sTop)
                        outputFile << L'{';
                    SPush(sTop, '(');
                }
                else
                    outputFile << L'(';
                break;

            case L'}':
                SPop(sTop, '}');
                if (!sTop)
                    outputFile << L'}';
                break;

            case L'*':
                if (inputString[i + 1] == L')')
                {
                    i++;
                    SPop(sTop, ')');
                    if (!sTop)
                        outputFile << L'}';
                }
                else
                    outputFile << L'*';
                break;

            default: outputFile << inputString[i];
            }
        }
        else
            outputFile << inputString[i];
    }
}

bool CheckBrackets(std::string inputFileName)
{
    std::wifstream inputFile;
    std::wofstream outputFile;
    wchar_t symbol;
    int curLine;
    int curPos;
    size_t i;
    bool apostropheSkip = false;

    inputFile.imbue(std::locale("rus_rus.1251"));
    outputFile.imbue(std::locale("rus_rus.1251"));

    inputFile.open(inputFileName);
    if (inputFile.fail())
    {
        std::cout << "Input file opening error. Check the file name" << std::endl;
        return false;
    }
    StackElt *sTop = 0;
    curLine = 1;
    curPos = 0;

    std::wstring inputString;
    std::wstring tempString;

    while (std::getline(inputFile, tempString, L'\n'))
        inputString += tempString + L'\n';
    inputFile.close();

    for (i = 0; i < inputString.length(); i++)
    {
        symbol = ReadSymbol(inputString[i], curLine, curPos);

        if (symbol == L'\'')
            apostropheSkip = !apostropheSkip;

        if (apostropheSkip != true)
        {
            switch (symbol)
            {
            case L'{':
                SPush(sTop, '{');
                break;

            case L'(':
                symbol = ReadSymbol(inputString[i + 1], curLine, curPos);
                if (symbol == L'*')
                {
                    i++;
                    SPush(sTop, '(');
                }
                break;

            case L'}':
                if (!SPop(sTop, '}'))
                {
                    ErrorHandling(sTop, curLine, curPos);
                    return false;
                }
                break;

            case L'*':
                symbol = ReadSymbol(inputString[i + 1], curLine, curPos);
                if (symbol == L')')
                {
                    i++;
                    if (!SPop(sTop, ')'))
                    {
                        ErrorHandling(sTop, curLine, curPos);
                        return false;
                    }
                }
                break;

            default: continue;
            }
        }
    }

    if (sTop)
    {
        ErrorHandling(sTop, curLine, curPos);
        return false;
    }

    outputFile.open(inputFileName);
    if (outputFile.fail())
    {
        std::cout << "Output file opening error. Check the file name" << std::endl;
        return false;
    }

    DeleteReduntantComments(inputString, outputFile);
    return true;
}

int main()
{
    setlocale(0, "Rus");

    std::locale::global(std::locale("Russian_Russia"));

    std::string inputCommand = "null";
    std::cout << "Enter input file name to check and correct comments in a code or type 'exit' to leave" << std::endl;

    while (true)
    {
        std::cin >> inputCommand;
        if (inputCommand != "exit")
        {
            if (CheckBrackets(inputCommand))
                std::cout << "Task completed succesfully" << std::endl;
        }
        else
            break;
    }

    return 0;
}