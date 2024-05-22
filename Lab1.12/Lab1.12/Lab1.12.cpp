//12. Имеется  текстовый   файл.Разработать    программу,
//обеспечивающую   вставку   запятой  перед  словами  "который",
//"которая", "которое", "которые", если они не являются первым
//словом предложения(7).

//VS 2022 Баранов Антон Сергеевич ПС-23
#include <iostream>
#include <fstream>
#include <string>

const int kFileNameLength = 63;
const int kWordLength = 63;

const int kSearchedWordRootLength = 6; //Длина корня искомоых слов + 1



bool isWordRootValid(wchar_t word[kWordLength], wchar_t searchedWordRoot[kSearchedWordRootLength]) //Проверка корня искомых слов
{
    for (int i = 2; i < kSearchedWordRootLength; i++)
    {
        if (word[i] != searchedWordRoot[i])
            return false;
    }
    return true;
}

bool isSuffixValid(wchar_t word[kWordLength]) //проверка окончаний искомых слов
{
    switch (word[kSearchedWordRootLength])
    {
        case L'ы':
            switch (word[kSearchedWordRootLength + 1])
            {
                case L'й': return true;

                case L'е': return true;

                default: return false;
            }

        case L'о':
            if (word[kSearchedWordRootLength + 1] = L'е') return true;

        case L'а':
            if (word[kSearchedWordRootLength + 1] = L'я') return true;

        default: return false;
    }
}

int main()
{
    setlocale(0, "Rus");

    wchar_t searchedWordRoot[kSearchedWordRootLength]; //Корень слов, после которых программа будет ставить запятую
    
    //Инициализация корня слова
    searchedWordRoot[1] = L'к';
    searchedWordRoot[2] = L'о';
    searchedWordRoot[3] = L'т';
    searchedWordRoot[4] = L'о';
    searchedWordRoot[5] = L'р';

    char inputFileName[kFileNameLength]; //v
    char outputFileName[kFileNameLength]; //Массивы символов с именами входного и выходного файлов
    wchar_t word[kWordLength]{}; // Массив символов для хранения слов
    wchar_t symbol; //Переменная для посимвольного чтения входного файла
    bool skipFirstSpace = true; //Костыль для пропуска первого пробела
    bool isSymbolComma = false; //Запоминание присутствия запятой, чтобы не ставить ещё одну
    bool isNewLine = false; //Замечаем переход на новую строку
    int i; 

    std::locale::global(std::locale("Russian_Russia"));

    std::wifstream inputFile;
    std::wofstream outputFile;

    inputFile.imbue(std::locale("rus_rus.1251"));
    outputFile.imbue(std::locale("rus_rus.1251"));

    std::cout << "Введите имена входного и выходного файлов" << std::endl;

    std::cin >> inputFileName >> outputFileName;

    inputFile.open(inputFileName);
    if (inputFile.fail())
    {
        std::cout << "Ошибка открытия входного файла, проверьте указанные имена на верность" << std::endl;
        return 0;
    }
    outputFile.open(outputFileName);
    if (outputFile.fail())
    {
        std::cout << "Ошибка открытия выходного файла, проверьте указанные имена на верность" << std::endl;
        return 0;
    }
    else
    {
        while (!inputFile.eof()) //Цикл до конца файла
        {
            inputFile.get(symbol);

            if (symbol == searchedWordRoot[1]) //Обнаружена первая буква искомого слова
            {
                i = 1;
                //До нового пробела, каждая буква слова будет записываться в массив
                while ((symbol != L' ') && !inputFile.eof())
                {
                    word[i] = symbol;
                    i++;
                    inputFile.get(symbol);
                }
                //При совпадении слова с одним из искомых слов и отсутствии перед ним запятой, пишется запятая
                if ((isWordRootValid(word, searchedWordRoot)) && (isSuffixValid(word)) && (!isSymbolComma))
                    outputFile << L',';
                if (isNewLine)
                {
                    outputFile << L'\n';
                    isNewLine = false;
                }
                else
                    outputFile << L' ';
                //Вывод слова из массива
                for (int j = 1; j < i; j++)
                {
                    outputFile << word[j];
                }
            }
            else 
            {
                if (!skipFirstSpace) //Костыль для отсутствия пробела перед первым словом в выводе
                {
                    if (isNewLine)
                    {
                        outputFile << L'\n';
                        isNewLine = false;
                    }
                    else
                        outputFile << L' ';
                }
                else
                    skipFirstSpace = false;
            }

            while ((symbol != L' ') && !inputFile.eof())
            {
                isSymbolComma = false;
                outputFile << symbol;
                inputFile.get(symbol);
                if (symbol == L',') 
                {
                    isSymbolComma = true;
                    outputFile << symbol;
                    inputFile.get(symbol);
                }
                if (symbol == L'\n')
                {
                    isNewLine = true;
                    break;
                }
            }
        }
    }
    return 0;
}