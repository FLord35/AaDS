//19. Теннисный  турнир  проходит  по  олимпийской  системе с
//выбываниями.В турнире участвуют 2 ^ n игроков.Известен рейтинг
//каждого игрока.Чем больше рейтинг, тем выше мастерство игрока.
//В первом туре сильнейший игрок встречается со слабейшим, второй
//по рейтингу с предпоследним и т.д.Организаторы собираются и в
//дальнейшем  придерживаться  этого  принципа, предполагая, что в
//очередной  тур  будет  выходить  сильнейшая  половина   игроков
//предыдущего  тура.Известно, что  турнир  прошел   в   полном
//соответствии с рейтингом игроков.Требуется показать в наглядном
//виде дерево проведенного турнира(12).

//Баранов Антон Сергеевич ПС-23

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

struct TreeElement
{
    std::pair<int, std::wstring> player;
    TreeElement* tFather;
    TreeElement* tLeft;
    TreeElement* tRight;
    bool isValueSet;
};

TreeElement* TreeCreate(TreeElement* &tRoot, int maxDepth, int currentDepth)
{
    TreeElement* temp = new TreeElement;
    temp->tFather = tRoot;
    temp->isValueSet = false;

    if (currentDepth != maxDepth)
    {

        temp->tLeft = TreeCreate(temp, maxDepth, currentDepth + 1);
        temp->tRight = TreeCreate(temp, maxDepth, currentDepth + 1);
    }
    else
    {

        temp->tLeft = nullptr;
        temp->tRight = nullptr;
    }

    return temp;
}

void TreeFill(std::multimap <int, std::wstring, std::less<int>> players, TreeElement* &tRoot, int depth, std::pair<int, std::wstring> HigherValue)
{
    if ((tRoot->tLeft != nullptr) && (tRoot->tRight != nullptr))
    {
        std::multimap <int, std::wstring, std::less<int>>::iterator mIteratorLower;
        std::multimap <int, std::wstring, std::less<int>>::iterator mIteratorHigher = players.end();

        --mIteratorHigher;

        tRoot->player = HigherValue;
        TreeFill(players, tRoot->tLeft, depth + 1, HigherValue);

        mIteratorLower = mIteratorHigher;
        for (int i = 1; i < pow(2, depth); i++)
            --mIteratorLower;

        while (mIteratorHigher->first != HigherValue.first)
        {
            ++mIteratorLower;
            --mIteratorHigher;
        }
        TreeFill(players, tRoot->tRight, depth + 1, *mIteratorLower);
    }
    else
        tRoot->player = HigherValue;
}

void OutputTournament(TreeElement* tRoot, int depth, int pCount)
{
    if (tRoot != nullptr)
    {
        OutputTournament(tRoot->tLeft, depth+1, pCount);
        for (int i = depth; i != pCount; i++)
            std::cout << "      ";
        std::wcout << tRoot->player.second << std::endl;
        OutputTournament(tRoot->tRight, depth+1, pCount);
    }
}

bool CreateTournament(std::string inputFileName)
{
    std::wifstream inputFile;
    std::wofstream outputFile;

    size_t playersCount;

    setlocale(LC_ALL, "Russian");

    inputFile.imbue(std::locale("rus_rus.1251"));
    outputFile.imbue(std::locale("rus_rus.1251"));

    inputFile.open(inputFileName);
    if (inputFile.fail())
    {
        std::cout << "File opening error. Check the file name" << std::endl;
        return false;
    }

    TreeElement* tRoot = nullptr;
    std::pair<int, std::wstring> tempPlayer;

    std::multimap <int, std::wstring, std::less<int>> PlayerToRating;

    while (!inputFile.eof())
    {
        inputFile >> tempPlayer.second;
        inputFile >> tempPlayer.first;
        PlayerToRating.insert(std::pair <int, std::wstring> (tempPlayer.first, tempPlayer.second));
    }     
    inputFile.close(); 

    playersCount = PlayerToRating.size();

    tRoot = TreeCreate(tRoot, log2(playersCount), 0);

    std::multimap <int, std::wstring, std::less<int>>::iterator HighestValue = PlayerToRating.end();
    --HighestValue;

    TreeFill(PlayerToRating, tRoot, 1, *HighestValue);

    std::cout << "Here's your tournament chart:" << std::endl << std::endl;
    OutputTournament(tRoot, 0, log2(playersCount));
    std::cout << std::endl;

    return true;
}

int main()
{
    setlocale(0, "Rus");

    std::locale::global(std::locale("Russian_Russia"));

    std::string inputCommand = "null";
    std::cout << "Enter input file name to create a tournament or type 'exit' to leave" << std::endl;

    while (true)
    {
        std::cin >> inputCommand;
        if (inputCommand != "exit")
        {
            if (CreateTournament(inputCommand))
                std::cout << "Task completed succesfully" << std::endl;
        }
        else
            break;
    }

    return 0;
}

