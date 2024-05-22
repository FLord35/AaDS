//17. В космическом центре проектируется  система организации
//связи   между  спутниками, действующая  в  реальном  масштабе
//времени.Каждый  спутник  может  иметь  либо  не  иметь   как
//радиостанцию, так и приемник радиосигналов.Сигналы с одного
//спутника могут передаваться на другие через произвольное число
//промежуточных спутников.Заранее известно, когда два спутника
//появляются в зоне радиовидимости и в  течение  какого  времени
//спутники будут находиться в этой зоне.Поиском в ширину  найти
//способы возможной связи между  двумя  заданными  спутниками  в
//указанный   момент   времени   в   порядке  возрастания  числа
//промежуточных звеньев(9).

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <queue>
#include <time.h>

struct SatToSatTime
{
    std::wstring sSat;
    std::wstring eSat;

    std::time_t startTime;
    std::time_t endTime;
};

struct SatEquipment
{
    std::wstring satName;

    bool hasTransmitter = false;
    bool hasReceiver = false;
};

std::vector<std::vector<int>> FindCommunications(
    std::wstring sSat, 
    std::wstring eSat, 
    std::vector<SatToSatTime> &mainData, 
    std::vector<std::vector<bool>> &adjMatrix, 
    std::map<std::wstring, int> &nameToIndex, 
    std::map<int, std::wstring> &indexToName
    )
{
    std::deque <std::vector<int>> routes;

    std::deque <std::wstring> broadthSearch;
    broadthSearch.push_front(sSat);
    routes.push_front({ nameToIndex[sSat] });

    int lastIndex = nameToIndex[eSat];

    std::vector<std::vector<int>> possibleRoutes;

    while (broadthSearch.size() != 0) //Обрабатываем текущие вершины, пока они не закончатся
    {
        bool skipLoop = false;

        int index = nameToIndex[broadthSearch.front()];

        for (int i = 0; i < routes.front().size() - 1; i++) //Проверяем путь, ищем, есть ли уже в нём текущая вершина
        {
            if (routes.front()[i] == index) //Если есть, уничтожаем путь из очередей
            {
                broadthSearch.pop_front();
                routes.pop_front();

                skipLoop = true;
                break;
            }
        }

        if (skipLoop)
            continue;

        if (index == lastIndex) //Если текущая вершина - это последний спутник, записываем путь в контейнер
        {
            //possibleRoutes.insert(std::pair<int, std::vector<int>>(static_cast<int>(routes.size()), routes.front()));
            possibleRoutes.push_back(routes.front());
            broadthSearch.pop_front();
            routes.pop_front();
            continue;
        }

        for (int i = 0; i < adjMatrix.size(); i++) //Проходим матрицу смежности
        {
            if (adjMatrix[index][i]) //Ищем спутники, с которыми может связаться текущая вершина
            {
                broadthSearch.push_back(indexToName[i]);

                std::vector<int> temp = routes.front();
                temp.push_back(i);
                routes.push_back(temp);
            }
        }

        routes.pop_front();
        broadthSearch.pop_front();
    }

    return possibleRoutes;
}

std::vector<std::vector<bool>> CreateAdjacencyMatrix(
    std::vector<SatToSatTime> &mainData,
    std::time_t &transTime,
    std::vector<SatEquipment> &equipmentList,
    std::map<std::wstring, int> &nameToIndex,
    std::map<int, std::wstring> &indexToName)
{
    std::vector<std::vector<bool>> adjMatrix (nameToIndex.size(), std::vector<bool>(nameToIndex.size()));

    for (int i = 0; i < adjMatrix.size(); i++) //Заполняем всю матрицу значениями false
    {
        for (int j = 0; j < adjMatrix[0].size(); j++)
            adjMatrix[i][j] = false;
    }

    for (int i = 0; i < mainData.size(); i++)  //Обрабатываем каждую пару связей спутников
    {
        if ((transTime <= mainData[i].endTime) && (transTime >= mainData[i].startTime)) //Если время связи спутников не совпадает со временем передачи, связи между ними не будет
        {
            int firstIndex = nameToIndex[mainData[i].sSat];
            int secondIndex = nameToIndex[mainData[i].eSat];

            if (equipmentList[firstIndex].hasTransmitter && equipmentList[secondIndex].hasReceiver) //Проверки на возможность передачи сигнала от спутника 1 к спутнику 2
                adjMatrix[firstIndex][secondIndex] = true;

            if (equipmentList[firstIndex].hasReceiver && equipmentList[secondIndex].hasTransmitter) //Проверки на возможность передачи сигнала от спутника 2 к спутнику 1
                adjMatrix[secondIndex][firstIndex] = true;
        }
    }

    return adjMatrix;
}

std::time_t ParseTime(std::wstring inputTime)
{
    int offset = 0;

    if (inputTime[1] == L':')
        offset = 1;

    std::tm tm{};

    //Для работы функции, необходимо хоть как-то заполнить год, месяц, день и секунды входного времени. На логику и работу программы данные значения не влияют
    tm.tm_year = 2020 - 1900;
    tm.tm_mon = 10;
    tm.tm_mday = 20;
    tm.tm_sec = 50; 

    tm.tm_hour = std::stoi(inputTime.substr(0, 2 - offset));
    tm.tm_min = std::stoi(inputTime.substr(3 - offset, 2));

    return std::mktime(&tm);
}

int main()
{
    setlocale(LC_ALL, "Russian");
    setlocale(0, "Rus");

    std::locale::global(std::locale("Russian_Russia"));

    std::wstring inputFileName = L"null";
    std::cout << "Enter input file name for a program to take satellite data from or type 'exit' to leave" << std::endl;

    std::wifstream inputFile;

    std::wcin >> inputFileName;

    if (inputFileName == L"exit")
        return 0;

    inputFile.imbue(std::locale("rus_rus.1251"));

    inputFile.open(inputFileName);
    if (inputFile.fail())
    {
        std::cout << "File opening error. Check the file name" << std::endl;
        return 0;
    }

    std::vector<SatToSatTime> mainData;

    std::wstring unparsedTime;

    while (!inputFile.eof())
    {
        SatToSatTime temp;
        std::wstring trashDash;
        
        inputFile >> temp.sSat;
        inputFile >> temp.eSat;

        inputFile >> unparsedTime;
        temp.startTime = ParseTime(unparsedTime);

        inputFile >> trashDash;

        inputFile >> unparsedTime;
        temp.endTime = ParseTime(unparsedTime);

        mainData.push_back(temp);
    }
    inputFile.close();

    std::cout << "Next enter a file containing all the satellites' names and their equipment. Or, once again, type 'exit'" << std::endl;

    std::wcin >> inputFileName;

    if (inputFileName == L"exit")
        return 0;

    inputFile.imbue(std::locale("rus_rus.1251"));

    inputFile.open(inputFileName);
    if (inputFile.fail())
    {
        std::cout << "File opening error. Check the file name" << std::endl;
        return 0;
    }

    std::vector<SatEquipment> equipmentList;

    std::map<std::wstring, int> satNameToIndex;
    std::map<int, std::wstring> indexToSatName;
    int i = 0;

    while (!inputFile.eof())
    {
        SatEquipment temp;
        std::wstring boolString;

        inputFile >> temp.satName;
        
        inputFile >> boolString;
        if (boolString == L"Y")
            temp.hasTransmitter = true;
        else
            temp.hasTransmitter = false;

        inputFile >> boolString;
        if (boolString == L"Y")
            temp.hasReceiver = true;
        else
            temp.hasReceiver = false;

        satNameToIndex.insert(std::pair<std::wstring, int>(temp.satName, i));
        indexToSatName.insert(std::pair<int, std::wstring>(i, temp.satName));
        i++;

        equipmentList.push_back(temp);
    }

    inputFile.close(); 

    std::wstring startingSatelliteName = L"null";
    std::wstring finalSatelliteName = L"null";

    std::time_t transTime;

    while (true)
    {
        std::cout << std::endl << "Type the names of the starting satellite and the final satellite and transmission's time" << std::endl;
        std::cout << "Or type 'exit' to leave" << std::endl;

        std::wcin >> startingSatelliteName;

        if (startingSatelliteName != L"exit")
        {
            std::wcin >> finalSatelliteName >> unparsedTime;
            transTime = ParseTime(unparsedTime);

            std::vector<std::vector<bool>> aMatrix = CreateAdjacencyMatrix(mainData, transTime, equipmentList, satNameToIndex, indexToSatName);

            std::vector<std::vector<int>> routes = FindCommunications(startingSatelliteName, finalSatelliteName, mainData, aMatrix, satNameToIndex, indexToSatName);

            if (routes.empty())
                std::cout << "Whoops! Looks like the connection is not possible" << std::endl;
            else
            {
                std::wcout << "Task completed succesfully. Here's all of the possible transmission routes from "
                    << startingSatelliteName << " to " << finalSatelliteName << " on " << unparsedTime << ":" << std::endl;

                for (auto& it : routes)
                {
                    for (auto& vIt : it)
                    {
                        std::wcout <<indexToSatName[vIt];
                        if (indexToSatName[vIt] != finalSatelliteName)
                            std::wcout << " -> ";
                    }

                    std::cout << std::endl;
                }
            }
        }
        else
            break;
    }

    return 0;
}