#include "individual_task.h"

void IndividualTask()
{
    vector<stack<Book>> bunches; //в переводе - стопки
    setlocale(0,"");
    char choice{}; // инициализация по умолчаю
    while(choice != '0')
    {
        //печатаем bunches
        int i = 0;
        for(const auto& s : bunches)
        {
            cout << "Стопка " << i << " (книг: " << s.size() << ") Наверху стопки ";
            if(!s.empty())
            {
                cout << "\"" << s.top().name << "\" -- " << s.top().author;
            }
            else
            {
                cout << "ничего нет.";
            }
            cout << "\n\n";
            ++i;
        }

        cout << "+--------   МЕНЮ    --------+\n";
        cout << "|1     Добавить книгу      1|\n";
        cout << "|2      Убрать книгу       2|\n";
        cout << "|3     Переложить книгу    3|\n";
        cout << "|4     Добавить стопку     4|\n";
        cout << "|5     Удалить стопку      5|\n";
        cout << "|6 Поменять стопки местами 6|\n";
        cout << "|0         Выход           0|\n";
        cout << "+--------   МЕНЮ    --------+\n";
        cout << "Ввод: ";
        input(choice);
        switch (choice)
        {
            case '1': //добавить книгу
            {
                if(bunches.empty())
                {
                    cout << "Некуда класть книги. Добавьте стопку.\n";
                    break;
                }
                Book book{}; //см файл .h - структура
                cout << "Название: "; input(book.name);
                cout << "Автор: "; input(book.author);
                int bunch_number;
                cout << "В какую стопку?: "; input(bunch_number);
                if(bunch_number < bunches.size() && bunch_number >= 0)
                {
                    bunches.at(bunch_number).push(book);
                }
                else
                {
                    cout << "Нет такой стопки\n";
                }
            }
            break;
            case '2': //удалить книгу
            {
                if(bunches.empty())
                {
                    cout << "Неоткуда убирать книги. Добавтьте стопку.\n";
                    break;
                }
                int bunch_number;
                cout << "Откуда убираем?: "; input(bunch_number);
                if(bunch_number < bunches.size() && bunch_number >= 0)
                {
                    if (!bunches.at(bunch_number).empty())
                    {
                        bunches.at(bunch_number).pop();
                    }
                    else
                    {
                        cout << "Эта стопка пустая\n";
                    }
                }
                else
                {
                    cout << "Нет такой стопки\n";
                }
            }
                break;
            case '3': //двигаем книги
            {
                if(bunches.empty() || bunches.size() == 1)
                {
                    cout << "Некуда перекладывать книги\n";
                    break;
                }
                int where_take;
                cout << "Откуда берём?: ";input(where_take);
                if(where_take < bunches.size() && where_take >= 0)
                {
                    if(!bunches.at(where_take).empty())
                    {
                        int where_put;
                        cout << "Куда перекладываем?: ";
                        input(where_put);
                        if (where_put < bunches.size() && where_put >= 0)
                        {
                            if (where_take != where_put)
                            {
                                    bunches.at(where_put).push(bunches.at(where_take).top());
                                    bunches.at(where_take).pop();
                            }
                            else
                            {
                                cout << "Это та же самая стопка\n";
                            }
                        }
                        else
                        {
                            cout << "Нет такой стопки\n";
                        }
                    }
                    else
                    {
                        cout << "Эта стопка пустая\n";
                    }
                }
                else
                {
                    cout << "Нет такой стопки\n";
                }
            }
                break;
            case '4': //добавить стопку
            {
                stack<Book> s;
                bunches.push_back(s);
            }
                break;
            case '5': //удалить стопку
            {
                int number_to_delete;
                cout<<"Какую стопку удаляем?: ";input(number_to_delete);
                if(number_to_delete < bunches.size() && number_to_delete >= 0)
                {
                    bunches.erase(bunches.cbegin()+number_to_delete);
                }
                else
                {
                    cout << "Нет такой стопки\n";
                }
            }
                break;
            case '6':// двигаем стопки свапаем
            {
                if(bunches.size() < 2)
                {
                    cout << "Нет стопок, которые можно поменять. Добавьте стопку.\n";
                    break;
                }
                int first, second;
                cout << "Первая стопка: "; input(first);
                if(first < bunches.size() && first >= 0)
                {
                    cout << "Вторая стопка: "; input(second);
                    if(second < bunches.size() && second >= 0)
                    {
                        if(first != second)
                        {
                            bunches.at(first).swap(bunches.at(second));
                        }
                        else
                        {
                            cout << "Это та же самая стопка\n";
                        }
                    }
                    else
                    {
                        cout << "Нет такой стопки\n";
                    }
                }
                else
                {
                    cout << "Нет такой стопки\n";
                }
            }
                break;
            case '0':
            default : break;
        }
    }
}
