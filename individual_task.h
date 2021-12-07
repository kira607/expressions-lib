#ifndef LAB_3_EXPRESSION_H
#define LAB_3_EXPRESSION_H

#include <iostream>
#include <vector>
#include <stack>
#include <limits>

using namespace std;

template <typename T>
void input(T& ch) //шаблонная проверка на ввод польз.
{
    while(true)
    {
        cin >> ch;
        if(cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            return;
        }
    }
}

//задание про стек из книг и стопок
struct Book
{
    string name;
    string author;
};

void IndividualTask();

#endif //LAB_3_EXPRESSION_H
