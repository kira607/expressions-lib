#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <chrono>

#include "expression.h"
#include "individual_task.h"
extern void IndividualTask();

using namespace std;

random_device rd; // для рандомного числа и 15 строчки
mt19937 Rand(rd()); // интовое рандом с равновероятностным событием

enum ExpressionNumber
{
    simple = 0,
    direct = 1, // прямая нотация
    reverse = 2, // обратная нотация
    none = -1,
};

enum Message // сообщения об ошибке или успехе
{
    kFileError, // код стайл по гуглу
    kFileSuccess,
    kGenError,
    kGenSuccess,
};

enum class ActiveInputBox // для позиции курсора
{
    expression,
    x,y,
    file,
    generator,
    none,
};

bool mouseCursorIn(const sf::RectangleShape& rect)  // проверка позиции курсора
{
    return sf::Mouse::getPosition().x > rect.getPosition().x &&
           sf::Mouse::getPosition().x < rect.getPosition().x + rect.getSize().x
           && sf::Mouse::getPosition().y > rect.getPosition().y &&
           sf::Mouse::getPosition().y < rect.getPosition().y + rect.getSize().y;
}

void updateCursorWithActiveInputBox(sf::RectangleShape& cursor, sf::Text& active_input_box)
{
    cursor.setPosition(active_input_box.getPosition().x+active_input_box.getGlobalBounds().width+8,active_input_box.getPosition().y+7);
} // обновление позиции курсора

void resetOutputTexts(array<sf::Text,3>& output_texts,
                      array<sf::Text,3>& result_texts)
{
    output_texts.at(ExpressionNumber::simple).setString("");
    output_texts.at(ExpressionNumber::direct).setString("");
    output_texts.at(ExpressionNumber::reverse).setString("");
    result_texts.at(ExpressionNumber::simple).setString("");
    result_texts.at(ExpressionNumber::direct).setString("");
    result_texts.at(ExpressionNumber::reverse).setString("");
} // сборс выходных текстов

void updateOutputTexts(array<sf::Text,3>& output_texts, array<sf::Text,3>& result_texts,
                       Expression& simpleE, Expression& directE, Expression& reverseE, Expression& mainE)
{
    resetOutputTexts(output_texts,result_texts);
    stringstream ss1;
    stringstream ss2;
    stringstream ss3;
    ss1.str(string());
    ss2.str(string());
    ss3.str(string());
    output_texts.at(ExpressionNumber::simple).setString(simpleE.getString());
    output_texts.at(ExpressionNumber::direct).setString(directE.getString());
    output_texts.at(ExpressionNumber::reverse).setString(reverseE.getString());

    if((!mainE.has_x || mainE.set_x)&&(!mainE.has_y || mainE.set_y))
    {
        if(mainE.type != ExpressionType::none)
        {
            ss1 << fixed << setprecision(2) << simpleE.expression_value;
            ss2 << fixed << setprecision(2) << directE.expression_value;
            ss3 << fixed << setprecision(2) << reverseE.expression_value;
            result_texts.at(ExpressionNumber::simple).setString(ss1.str());
            result_texts.at(ExpressionNumber::direct).setString(ss2.str());
            result_texts.at(ExpressionNumber::reverse).setString(ss3.str());
        }
    }
    else
    {
        result_texts.at(ExpressionNumber::simple). setString("");
        result_texts.at(ExpressionNumber::direct). setString("");
        result_texts.at(ExpressionNumber::reverse).setString("");
    }
} // обновление выходных текстов

void updateExpressions(Expression& simpleE, Expression& directE, Expression& reverseE, Expression& mainE)
{
    simpleE.reset();
    directE.reset();
    reverseE.reset();

    simpleE.set(mainE.getString());
    directE.set(mainE.getString());
    reverseE.set(mainE.getString());

    simpleE. type = mainE.type;
    directE. type = mainE.type;
    reverseE.type = mainE.type;

    simpleE.convert(ExpressionType::simple);
    directE.convert(ExpressionType::direct);
    reverseE.convert(ExpressionType::reverse);

    simpleE. set_x = mainE.set_x;
    directE. set_x = mainE.set_x;
    reverseE.set_x = mainE.set_x;

    simpleE. set_y = mainE.set_y;
    directE. set_y = mainE.set_y;
    reverseE.set_y = mainE.set_y;

    simpleE. x_value = mainE.x_value;
    directE. x_value = mainE.x_value;
    reverseE.x_value = mainE.x_value;

    simpleE. y_value = mainE.y_value;
    directE. y_value = mainE.y_value;
    reverseE.y_value = mainE.y_value;

    simpleE.calculate();
    directE.calculate();
    reverseE.calculate();
} // обновление выражений

void updateMainE_X(Expression& mainE, const sf::Text& x, bool& invalid_x)
{
    try
    {
        mainE.x_value = stoi(string(x.getString()));
        mainE.set_x = true;
    }
    catch(exception&)
    {
        invalid_x = true;
        mainE.set_x = false;
    }
} // обновление переменной х

void updateMainE_Y(Expression& mainE, const sf::Text& y, bool& invalid_y)
{
    try
    {
        mainE.y_value = stoi(string(y.getString()));
        mainE.set_y = true;
    }
    catch (exception &)
    {
        invalid_y = true;
        mainE.set_y = false;
    }
} // обновление переменной у

bool setExpressionFromFile(const sf::String& path, Expression& mainE) // загрузить файл
{
    ifstream fin;
    fin.open(path);
    if(!fin.is_open())
    {
        return false;
    }
    string file_data;
    getline(fin,file_data);
    mainE.reset();
    if(mainE.set(file_data))
    {
        return mainE.setType() != ExpressionType::none;
    }
    return false;
}

string generateExpression(int number_of_operands) // генерация выражения
{
    char cached_operator = '0';
    string result;
    int operand;
    char operator_;
    for(int i = 0; i < number_of_operands-1; ++i)
    {

        if(cached_operator == '/') //предыдущий оператор
        {
            operand = 1 + static_cast<int>(Rand()%6); // 1 до 5
        }
        else if (cached_operator == '^')
        {
            operand = 1 + static_cast<int>(Rand()%4); // 1 до 3
        }
        else
        {
            operand = -5 + static_cast<int>(Rand()%11); // -5 до 5
        }
        result.append(to_string(operand)+" "); // метод для добавления стрингов
        switch(Rand()%5)
        {
            case 0: operator_ = '+'; break;
            case 1: operator_ = '-'; break;
            case 2: operator_ = '*'; break;
            case 3: operator_ = '/'; break;
            case 4: operator_ = '^'; break;
            default: throw runtime_error("Error: generateExpression(): Unexpected Rand() value");
        }
        cached_operator = operator_;
        result += operator_;
        result += " ";
    }
    operand = -5 + static_cast<int>(Rand()%11); // -100 до 100
    result.append(to_string(operand));
    return result;
}
string generateAnswer(const Expression& expression, int task_number, ExpressionType type_to_gen) // генерация ответов к заданиям
{
    stringstream ss; // возможность класть в строчку все чтг угодно
    string s;
    stringstream answer;
    if(task_number == 1)
    {
        if(type_to_gen == ExpressionType::direct) // условие для прямой нотации - её вычисление
        {
            answer << "Решение:\n"
                      "Читаем выражение справа налево\n"
                      "Если встречаем операнд (число),\n"
                      "помещаем его в стек, если оператор - выполняем операцию.\n\n";
            vector<PartOfExpression> direct_stack;
            for(unsigned long i = expression.vec.size()-1; i != -1; --i)
            {
                if(expression.vec.at(i).type == PartType::operand) // встретили число
                {
                    ss << fixed << setprecision(2) << expression.vec.at(i).data.operand;
                    answer << "Встретили: (" + ss.str() + ")\n";
                    ss.str(string());
                    answer << "Помещаем в стек\n";
                    direct_stack.push_back(expression.vec.at(i));
                    answer << "Cтек: ";
                    for(const auto& part : direct_stack)
                    {
                        ss << fixed << setprecision(2) << part.data.operand;
                        answer << ss.str() + " ";
                        ss.str(string());
                    }
                    answer << "\n\n";
                }
                else if(expression.vec.at(i).type == PartType::operatorPT) // встретили знак
                {
                    answer << "Встретили: (" << expression.vec.at(i).data.operatorP << ")\n";
                    answer << "Выполняем операцию (";

                    char operator_ = expression.vec.at(i).data.operatorP;
                    double result = direct_stack.at(direct_stack.size()-1).data.operand;
                    ss << fixed << setprecision(2) << result;
                    answer << ss.str() + " ";
                    ss.str(string());
                    answer << operator_;
                    direct_stack.erase(direct_stack.end()-1);
                    switch (operator_)
                    {
                        case '+': result += direct_stack.at(direct_stack.size()-1).data.operand; break;
                        case '-': result -= direct_stack.at(direct_stack.size()-1).data.operand; break;
                        case '*': result *= direct_stack.at(direct_stack.size()-1).data.operand; break;
                        case '/': result /= direct_stack.at(direct_stack.size()-1).data.operand; break;
                        case '^': result = pow(result,direct_stack.at(direct_stack.size()-1).data.operand); break;
                        default: throw runtime_error("Error: generateAnswer(): Unexpected operator value"); ///
                    }
                    ss << fixed << setprecision(2) << direct_stack.at(direct_stack.size()-1).data.operand;
                    answer << " " + ss.str() + ") и помещаем результат в стек\n";
                    ss.str(string());
                    direct_stack.erase(direct_stack.end()-1);
                    PartOfExpression tmp;
                    tmp.type = PartType::operand;
                    tmp.data.operand = result;
                    direct_stack.push_back(tmp);

                    answer << "Cтек: ";
                    for(const auto& part : direct_stack)
                    {
                        ss << fixed << setprecision(2) << part.data.operand;
                        answer << ss.str() + " ";
                        ss.str(string());
                    }
                    answer << "\n\n";
                }
            }
            ss << fixed << setprecision(2) << direct_stack.at(0).data.operand;
            answer << "Ответ: " + ss.str() + " ";
            ss.str(string());
            return answer.str();
        }
        else // обратная нотация - её вычисление
        {
            answer << "Решение:\n"
                      "Читаем выражение слева направо\n"
                      "Если встречаем операнд (число),\n"
                      "помещаем его в стек, если оператор - выполняем операцию.\n\n";
            vector<PartOfExpression> reverse_stack;
            for (const auto& part : expression.vec)
            {
                if (part.type == PartType::operand) //встретили число
                {
                    ss << fixed << setprecision(2) << part.data.operand;
                    answer << "Встретили: (" + ss.str() + ")\n";
                    ss.str(string());
                    answer << "Помещаем в стек\n";
                    reverse_stack.push_back(part);
                    answer << "Cтек: ";
                    for(const auto& part_from_stack : reverse_stack) // вывод операндов из стека
                    {
                        ss << fixed << setprecision(2) << part_from_stack.data.operand;
                        answer << ss.str() + " ";
                        ss.str(string());
                    }
                    answer << "\n\n";
                }
                else if (part.type == PartType::operatorPT) // встретили знак
                {
                    double result = reverse_stack.at(reverse_stack.size()-1).data.operand;
                    reverse_stack.erase(reverse_stack.end()-1);
                    answer << "Встретили: (" << part.data.operatorP << ")\n";

                    answer << "Выполняем операцию (";
                    ss << fixed << setprecision(2) << reverse_stack.at(reverse_stack.size()-1).data.operand;
                    answer << ss.str() + " ";
                    ss.str(string());
                    answer << part.data.operatorP;
                    switch (part.data.operatorP)
                    {
                        case '+': reverse_stack.at(reverse_stack.size()-1).data.operand += result; break;
                        case '-': reverse_stack.at(reverse_stack.size()-1).data.operand -= result; break;
                        case '*': reverse_stack.at(reverse_stack.size()-1).data.operand *= result; break;
                        case '/': reverse_stack.at(reverse_stack.size()-1).data.operand /= result; break;
                        case '^': reverse_stack.at(reverse_stack.size()-1).data.operand  = pow(reverse_stack.at(reverse_stack.size()-1).data.operand,result);
                        break;
                        default: throw runtime_error("Error: Unexpected operator value");
                    }
                    ss << fixed << setprecision(2) << result;
                    answer << " " + ss.str();
                    ss.str(string());
                    answer << ") и помещаем результат в стек\n";

                    answer << "Cтек: ";
                    for(const auto& part_from_stack : reverse_stack)
                    {
                        ss << fixed << setprecision(2) << part_from_stack.data.operand;
                        answer << ss.str() + " ";
                        ss.str(string());
                    }
                    answer << "\n\n";
                }
            }
            ss << fixed << setprecision(2) << reverse_stack.at(0).data.operand;
            answer << "Ответ: " + ss.str() + " ";
            ss.str(string());
            return answer.str();
        }
    }
    else // преобразование в прямую нотацию
    {
        if(type_to_gen == ExpressionType::direct)
        {
            answer << "Решение:\n"
                      "Читаем выражение справа налево.\n"
                      "Операнды (числа) кладём в стек результата.\n"
                      "Операторы, в зависимости от их приоритета, либо кладём в стек операторов,\n"
                      "либо кладём в стек результата\n"
                      "В конце переворачиваем стек результата\n\n";
            vector<PartOfExpression> result;
            vector<PartOfExpression> stack_operators;
            for(unsigned long i = expression.vec.size()-1; i != -1; --i)
            {
                switch(expression.vec.at(i).type)
                {
                    case PartType::operatorPT:
                    {
                        answer << "Встретили (" << expression.vec.at(i).data.operatorP << ")\n";
                        if(stack_operators.empty())
                        {
                            answer << "Стек операторов пуст, поэтому кладём оператор в стек операторов\n";
                            stack_operators.push_back(expression.vec.at(i));
                            answer << "Стек операторов: ";
                            for(const auto& part_operators : stack_operators)
                            {
                                answer << part_operators.data.operatorP << " ";
                            }
                            answer << "\nСтек результата: ";
                            for(const auto& part_result : result)
                            {
                                switch(part_result.type)
                                {
                                    case PartType::operatorPT:
                                        answer << part_result.data.operatorP << " "; break;
                                    case PartType::operand:
                                        answer << part_result.data.operand << " ";   break;
                                    case PartType::variable:
                                    case PartType::bracket:
                                    case PartType::none:
                                        break;
                                }
                            }
                            answer << "\n\n";
                        }
                        else if(priority(expression.vec.at(i)) < priority(stack_operators.at(stack_operators.size()-1)))
                        {
                            answer << "Приоритетет \"" <<
                            expression.vec.at(i).data.operatorP << "\" меньше чем у \"" <<
                            stack_operators.at(stack_operators.size()-1).data.operatorP <<
                            "\" поэтому выталкиваем в стек результата все операторы из стека результата,\n"
                            "чей приоритет выше чем у \"" <<
                            expression.vec.at(i).data.operatorP << "\"\n";

                            while(priority(stack_operators.at(stack_operators.size()-1)) > priority(expression.vec.at(i)))
                            {
                                result.push_back(stack_operators.at(stack_operators.size()-1));
                                stack_operators.erase(stack_operators.end()-1);
                                if(stack_operators.empty())
                                {
                                    break;
                                }
                            }
                            stack_operators.push_back(expression.vec.at(i));
                            answer << "Стек операторов: ";
                            for(const auto& part_operators : stack_operators)
                            {
                                answer << part_operators.data.operatorP << " ";
                            }
                            answer << "\nСтек результата: ";
                            for(const auto& part_result : result)
                            {
                                switch(part_result.type)
                                {
                                    case PartType::operatorPT:
                                        answer << part_result.data.operatorP << " "; break;
                                    case PartType::operand:
                                        answer << part_result.data.operand << " ";   break;
                                    case PartType::variable:
                                    case PartType::bracket:
                                    case PartType::none:
                                        break;
                                }
                            }
                            answer << "\n\n";
                        }
                        else
                        {
                            answer << "Приоритетет \"" <<
                                   expression.vec.at(i).data.operatorP << "\" больше или равен приоритету \"" <<
                                   stack_operators.at(stack_operators.size()-1).data.operatorP <<
                                   "\" поэтому кладём оператор в стек операторов\n";
                            stack_operators.push_back(expression.vec.at(i));
                            answer << "Стек операторов: ";
                            for(const auto& part_operators : stack_operators)
                            {
                                answer << part_operators.data.operatorP << " ";
                            }
                            answer << "\nСтек результата: ";
                            for(const auto& part_result : result)
                            {
                                switch(part_result.type)
                                {
                                    case PartType::operatorPT:
                                        answer << part_result.data.operatorP << " "; break;
                                    case PartType::operand:
                                        answer << part_result.data.operand << " ";   break;
                                    case PartType::variable:
                                    case PartType::bracket:
                                    case PartType::none:
                                        break;
                                }
                            }
                            answer << "\n\n";
                        }
                    }
                        break;
                    case PartType::operand:
                        answer << "Встретили (" << expression.vec.at(i).data.operand << ")\n";
                        answer << "Записываем в стек результата\n";
                        result.push_back(expression.vec.at(i));
                        answer << "Стек операторов: ";
                        for(const auto& part_operators : stack_operators)
                        {
                            answer << part_operators.data.operatorP << " ";
                        }
                        answer << "\nСтек результата: ";
                        for(const auto& part_result : result)
                        {
                            switch(part_result.type)
                            {
                                case PartType::operatorPT:
                                    answer << part_result.data.operatorP << " "; break;
                                case PartType::operand:
                                    answer << part_result.data.operand << " ";   break;
                                case PartType::variable:
                                case PartType::bracket:
                                case PartType::none:
                                    break;
                            }
                        }
                        answer << "\n\n";
                        break;
                    case PartType::bracket:
                    case PartType::none:
                    case PartType::variable:
                        break;
                }
            }
            answer << "Если стек операторов не пуст, выталкиваем всё,\n"
                      "что осталось, в стек результата, после чего переворачиваем его.\n\n";
            while(!stack_operators.empty())
            {
                result.push_back(stack_operators.at(stack_operators.size()-1));
                stack_operators.erase(stack_operators.end()-1);
            }
            answer << "Ответ: ";
            for(unsigned long i = result.size()-1; i != -1; --i)
            {
                switch(result.at(i).type)
                {
                    case PartType::operatorPT:
                        answer << result.at(i).data.operatorP << " "; break;
                    case PartType::operand:
                        answer << result.at(i).data.operand << " ";   break;
                    case PartType::variable:
                    case PartType::bracket:
                    case PartType::none:
                        break;
                }
            }
            return answer.str();

        }
        else // преобразование в обратную нотацию
        {
            answer << "Решение:\n"
                      "Читаем выражение слева направо.\n"
                      "Операнды (числа) выписываем в результат.\n"
                      "Операторы, в зависимости от их приоритета, либо кладём в стек операторов,\n"
                      "либо выписываем в результат.\n\n";
            vector<PartOfExpression> result;
            vector<PartOfExpression> stack_operators;
            for(const auto& part : expression.vec)
            {
                switch (part.type)
                {
                    case PartType::operatorPT:
                    {
                        answer << "Встретили (" << part.data.operatorP << ")\n";
                        if(!stack_operators.empty())
                        {
                            answer << "Если приоритет \"" << part.data.operatorP << "\" ,больше или равен приоритету \"" <<
                            stack_operators.at(stack_operators.size()-1).data.operatorP << "\", "
                            "выписываем в результат операторы из стека операторов, пока не встретим оператор, \n"
                            "чей приоритет меньше приоритетета \"" << part.data.operatorP << "\"\n";
                            while (priority(part) <= priority(stack_operators.at(stack_operators.size()-1)))
                            {
                                result.push_back(stack_operators.at(stack_operators.size()-1));
                                stack_operators.erase(stack_operators.end()-1);
                                if(stack_operators.empty())
                                {
                                    break;
                                }
                            }
                        }
                        answer << "Помещаем в стек операторов\n";
                        stack_operators.push_back(part);
                        answer << "Cтек операторов: ";
                        for(const auto o : stack_operators)
                        {
                            answer << o.data.operatorP << " ";
                        }
                        answer << "\nРезультат: ";
                        for(const auto p : result)
                        {
                            switch (p.type)
                            {
                                case PartType::operatorPT:
                                    answer << p.data.operatorP << " ";
                                    break;
                                case PartType::operand:
                                    answer << p.data.operand << " ";
                                    break;
                                case PartType::variable:
                                case PartType::bracket:
                                case PartType::none:
                                    break;
                            }
                        }
                        answer << "\n\n";
                    }
                        break;
                    case PartType::operand:
                        answer << "Встретили (" << part.data.operand << ")\n";
                        answer << "Выписываем в результат\n";
                        result.push_back(part);
                        answer << "Cтек операторов: ";
                        for(const auto o : stack_operators)
                        {
                            answer << o.data.operatorP << " ";
                        }
                        answer << "\nРезультат: ";
                        for(const auto p : result)
                        {
                            switch (p.type)
                            {
                                case PartType::operatorPT:
                                    answer << p.data.operatorP << " ";
                                    break;
                                case PartType::operand:
                                    answer << p.data.operand << " ";
                                    break;
                                case PartType::variable:
                                case PartType::bracket:
                                case PartType::none:
                                    break;
                            }
                        }
                        answer << "\n\n";
                        break;
                    case PartType::variable:
                    case PartType::bracket:
                    case PartType::none:
                        break;
                }
            }
            answer << "Если в стеке остались операторы, выписываем их в результат.\n\n";
            while(!stack_operators.empty())
            {
                result.push_back(stack_operators.at(stack_operators.size()-1));
                stack_operators.erase(stack_operators.end()-1);
            }
            answer << "Ответ: ";
            for(const auto p : result)
            {
                switch (p.type)
                {
                    case PartType::operatorPT:
                        answer << p.data.operatorP << " ";
                        break;
                    case PartType::operand:
                        answer << p.data.operand << " ";
                        break;
                    case PartType::variable:
                    case PartType::bracket:
                    case PartType::none:
                        break;
                }
            }
            return answer.str();
        }
    }
}
void TasksGenerator(int number) // генерация выражения
{
    string var_file_name = "variant_";
    string answer_file_name = "answers_variant_";
    ofstream variant;
    ofstream answer;
    Expression tmp_expression;
    ExpressionType type_to_gen;
    for(int i = 0; i < number; ++i)
    {
        variant.open(var_file_name + to_string(i+1));
        answer.open(answer_file_name + to_string(i+1));
        variant << "Вариант " << i+1 << "\n\n";
        answer << "Вариант " << i+1 << " ответы\n\n";

        ///Задание #1/////////////////////////////////////////////////////////////////////////////
        type_to_gen = static_cast<ExpressionType>(Rand()%2 + 1); // 1 или 2 значит прямая нотац или обратная нотац

        variant << "Задание 1.\n";
        variant << "Вычислите выражение, записанное в ";
        if(type_to_gen == ExpressionType::direct)
            variant << "прямой";
        else
            variant << "обратной";
        variant << " польской нотации.\nЕсли нужно, ответ округлите до двух знаков после запятой.\n\n";

        tmp_expression.reset();
        tmp_expression.set(generateExpression(5+Rand()%5));
        tmp_expression.setType();
        tmp_expression.convert(type_to_gen);
        variant << tmp_expression.getString();

        answer << "Задание 1.\n\n";
        answer << generateAnswer(tmp_expression,1,type_to_gen);

        ///Задание #2/////////////////////////////////////////////////////////////////////////////
        type_to_gen = static_cast<ExpressionType>(Rand()%2 + 1); // 1 or 2 -> direct or reverse ///
        variant << "\n\nЗадание 2.\n";
        variant << "Перепишите выражение в ";
        if(type_to_gen == ExpressionType::direct)
            variant << "прямой";
        else
            variant << "обратной";
        variant << " польской нотации.\n\n";

        tmp_expression.reset();
        tmp_expression.set(generateExpression(5+Rand()%5)); // 5 до 9 чисел
        tmp_expression.setType();
        variant << tmp_expression.getString();

        answer << "\n\nЗадание 2.\n\n";
        answer << generateAnswer(tmp_expression,2,type_to_gen);

        variant.close();
        answer.close();
    }
}

void compareSpeed() // сравнение скорости вычисления и создания с проверкой прямой и обр нотации + вывод данных в файл
{
    ofstream file("compare_speed.txt");
    if(!file.is_open())
    {
        return;
    }

    Expression expression;
    chrono::time_point<chrono::system_clock> start, end;
    int number_of_operands = 30;
    file << "Выражение в прямой польской нотации\n";
    file << "Создание и проверка/Вычисление (микросекунд):\n\n";
    int sum1{0}, sum2{0};
    for(int i = 0; i < 20; ++i)
    {
        expression.set(generateExpression(number_of_operands));
        expression.setType();
        expression.convert(ExpressionType::direct);

        file << expression.getString() << "\n\n";
        file << "[" << i+1 << "]\t\t";
        // создание и проверка
        start = chrono::system_clock::now();
        Expression direct;
        direct.set(expression.getString());
        direct.setType();
        end = chrono::system_clock::now();
        file << chrono::duration_cast<chrono::microseconds>(end-start).count() << "\t";
        sum1+=chrono::duration_cast<chrono::microseconds>(end-start).count();

        //вычисление
        start = chrono::system_clock::now();
        direct.calculate();
        end = chrono::system_clock::now();
        file << chrono::duration_cast<chrono::microseconds>(end-start).count() << "\n\n";
        sum2+=chrono::duration_cast<chrono::microseconds>(end-start).count();

        direct.reset();
        expression.reset();
    }

    file << "Итого: " << sum1/20 << "/" << sum2/20;
    sum1 = 0; sum2 = 0;
    file << "\n";
    file << "Выражение в обратной польской нотации\n";
    file << "Создание и проверка/Вычисление (микросекунд):\n\n";
    for(int i = 0; i < 20; ++i)
    {
        expression.set(generateExpression(number_of_operands)); // метод проверки символов в строке для выражения
        expression.setType();
        expression.convert(ExpressionType::reverse);

        file << expression.getString() << "\n\n";
        file << "[" << i+1 << "]\t";
        // создание и проверка
        start = chrono::system_clock::now();
        Expression reverse;
        reverse.set(expression.getString());
        reverse.setType();
        end = chrono::system_clock::now();
        file << chrono::duration_cast<chrono::microseconds>(end-start).count() << "\t";
        sum1+=chrono::duration_cast<chrono::microseconds>(end-start).count();

        //вычисление
        start = chrono::system_clock::now();
        reverse.calculate();
        end = chrono::system_clock::now();
        file << chrono::duration_cast<chrono::microseconds>(end-start).count() << "\n\n";
        sum2+=chrono::duration_cast<chrono::microseconds>(end-start).count();

        reverse.reset();
        expression.reset();
    }
    file << "Итого: " << sum1/20 << "/" << sum2/20;
}

int main()
{
    compareSpeed();
    Expression mainE;
    Expression simpleE;
    Expression directE;
    Expression reverseE;

    // установка окна
    sf::RenderWindow window(sf::VideoMode(600, 400), "Laboratory work #3",sf::Style::Fullscreen);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(true);
    const auto screen_width  = (float)window.getSize().x;
    const auto screen_height = (float)window.getSize().y;

    //установка языков вывода
    sf::Font dosis;
    dosis.loadFromFile("../fonts/Dosis/Dosis/static/Dosis-Light.ttf");
    sf::Font afronic;
    afronic.loadFromFile("../fonts/mr_AfronicG.ttf");
    constexpr int kCharacterSize = 46;

    //установка цветов
    sf::Color in_box_color{sf::Uint8(255), sf::Uint8(255), sf::Uint8(255), sf::Uint8(20)};
    sf::Color in_box_button_color{sf::Uint8(255), sf::Uint8(255), sf::Uint8(255), sf::Uint8(40)};
    sf::Color text_color{sf::Uint8(170), sf::Uint8(170), sf::Uint8(170), sf::Uint8(220)};
    sf::Color background_text_color{sf::Uint8(170),sf::Uint8(170),sf::Uint8(170),sf::Uint8(30)};

    //установка поля для ввода выражения
    sf::RectangleShape expression_input_box(sf::Vector2f{screen_width * 0.7f, screen_height * 0.06f});
    expression_input_box.setPosition((screen_width - expression_input_box.getSize().x) / 2, screen_height / 6);
    expression_input_box.setFillColor(in_box_color);
    expression_input_box.setOutlineThickness(1);
    expression_input_box.setOutlineColor(sf::Color::White);

    // установка вводимого выражения
    sf::Text expression(sf::String(""),dosis);
    expression.setFillColor(text_color);
    expression.setPosition(expression_input_box.getPosition().x, expression_input_box.getPosition().y);
    expression.setCharacterSize(kCharacterSize);

    //установка при вводе выражения бэкграундного текста
    sf::Text expression_background_text(sf::String("Input expression"), dosis);
    expression_background_text.setFillColor(background_text_color);
    expression_background_text.setPosition(expression_input_box.getPosition().x, expression_input_box.getPosition().y);
    expression_background_text.setCharacterSize(static_cast<int>(expression_input_box.getSize().y) - 10);

    //установка окна для переменной x
    sf::RectangleShape x_input_box(sf::Vector2f{screen_width * 0.08f, screen_height * 0.06f});
    x_input_box.setPosition(expression_input_box.getPosition().x, expression_input_box.getPosition().y+expression_input_box.getSize().y+15);
    x_input_box.setFillColor(in_box_color);
    x_input_box.setOutlineThickness(1);
    x_input_box.setOutlineColor(sf::Color::White);

    //установка окна переменной x
    sf::Text x(sf::String(""),dosis);
    x.setFillColor(text_color);
    x.setPosition(x_input_box.getPosition().x, x_input_box.getPosition().y);
    x.setCharacterSize(kCharacterSize);

    //установка x background text
    sf::Text x_background_text(sf::String("Input X"), dosis);
    x_background_text.setFillColor(background_text_color);
    x_background_text.setPosition(x_input_box.getPosition().x, x_input_box.getPosition().y);
    x_background_text.setCharacterSize(kCharacterSize);

    //установка y input box
    sf::RectangleShape y_input_box(sf::Vector2f{screen_width * 0.08f, screen_height * 0.06f});
    y_input_box.setPosition(x_input_box.getPosition().x+x_input_box.getSize().x+15, x_input_box.getPosition().y);
    y_input_box.setFillColor(in_box_color);
    y_input_box.setOutlineThickness(1);
    y_input_box.setOutlineColor(sf::Color::White);

    //установка текста в окне для y
    sf::Text y(sf::String(""),dosis);
    y.setFillColor(text_color);
    y.setPosition(y_input_box.getPosition().x, y_input_box.getPosition().y);
    y.setCharacterSize(kCharacterSize);

    //установка в окне переменной y текста на фоне
    sf::Text y_background_text(sf::String("Input Y"), dosis);
    y_background_text.setFillColor(background_text_color);
    y_background_text.setPosition(y_input_box.getPosition().x, y_input_box.getPosition().y);
    y_background_text.setCharacterSize(kCharacterSize);

    float width_of_output_line; //оцентровка
    {
        sf::Text equal_tmp("=", dosis, 50);
        width_of_output_line = (expression_input_box.getSize().x + equal_tmp.getGlobalBounds().width +
                                      screen_width * 0.2f + 42);
    }

    // установка выходящих окон
    sf::RectangleShape output_box(sf::Vector2f{screen_width * 0.7f, screen_height * 0.06f});
    output_box.setFillColor(in_box_color);
    output_box.setOutlineThickness(1);
    output_box.setOutlineColor(sf::Color::White);
    array<sf::RectangleShape,3> output_boxes;
    for(int i = 0; i < 3; ++i)
    {
        output_box.setPosition((screen_width-width_of_output_line)/2,
                               expression_input_box.getPosition().y+expression_input_box.getSize().y*(3.2f+1.5f*static_cast<float>(i)));
        output_boxes.at(i) = output_box;
    }

    // установка выходящих окон
    sf::Text output_text(sf::String(""),dosis);
    output_text.setFillColor(text_color);
    output_text.setCharacterSize(kCharacterSize);
    array<sf::Text,3> output_texts;
    for(int i = 0; i < 3; ++i)
    {
        output_text.setPosition(output_boxes.at(i).getPosition().x, output_boxes.at(i).getPosition().y);
        output_texts.at(i) = output_text;
    }

    //фоновый текст для окон с другими выражениями
    vector<string> background_strings{"Simple expression","Direct expression","Reverse expression"};
    sf::Text background_text(sf::String(""), dosis);
    background_text.setFillColor(background_text_color);
    background_text.setCharacterSize(kCharacterSize);
    array<sf::Text,3> background_texts;
    for(int i = 0; i < 3; ++i)
    {
        background_text.setString(background_strings.at(i));
        background_text.setPosition(output_boxes.at(i).getPosition().x, output_boxes.at(i).getPosition().y);
        background_texts.at(i) = background_text;
    }

    //установка знака равно
    sf::Text equal("=",dosis,50);
    equal.setFillColor(text_color);
    array<sf::Text,3> equal_signs;
    for(int i = 0; i < 3; ++i)
    {
        equal.setPosition(output_boxes.at(ExpressionNumber::simple).getPosition().x + output_boxes.at(ExpressionNumber::simple).getSize().x + 20,
                          output_boxes.at(ExpressionNumber::simple).getPosition().y + i * 1.5 * output_boxes.at(ExpressionNumber::simple).getSize().y);
        equal_signs.at(i) = equal;
    }

    //боксы для значения вычислений
    sf::RectangleShape result_box(sf::Vector2f{screen_width * 0.2f, screen_height * 0.06f});
    result_box.setFillColor(in_box_color);
    result_box.setOutlineThickness(1);
    result_box.setOutlineColor(sf::Color::White);
    array<sf::RectangleShape,3> result_boxes;
    for(int i = 0; i < 3; ++i)
    {
        result_box.setPosition(output_boxes.at(ExpressionNumber::simple).getPosition().x +
        output_boxes.at(ExpressionNumber::simple).getSize().x + 42 + equal.getGlobalBounds().width,
                               output_boxes.at(ExpressionNumber::simple).getPosition().y + i * 1.5 * output_boxes.at(ExpressionNumber::simple).getSize().y);
        result_boxes.at(i) = result_box;
    }

    // устнаковка текстов для результатов выражений
    sf::Text result_text(sf::String(""),dosis);
    result_text.setFillColor(text_color);
    result_text.setCharacterSize(kCharacterSize);
    array<sf::Text,3> result_texts;
    for(int i = 0; i < 3; ++i)
    {
        result_text.setPosition(result_boxes.at(i).getPosition().x, result_boxes.at(i).getPosition().y);
        result_texts.at(i) = result_text;
    }

    // устнаковка фоновых текстов для результатов выражений
    sf::Text background_result_text(sf::String("0"),dosis);
    background_result_text.setFillColor(background_text_color);
    background_result_text.setCharacterSize(kCharacterSize);
    array<sf::Text,3> background_result_texts;
    for(int i = 0; i < 3; ++i)
    {
        background_result_text.setPosition(result_boxes.at(i).getPosition().x, result_boxes.at(i).getPosition().y);
        background_result_texts.at(i) = background_result_text;
    }

    // курсор
    sf::RectangleShape cursor(sf::Vector2f{2, expression_background_text.getGlobalBounds().height});
    cursor.setFillColor(sf::Color::White);
    cursor.setPosition(expression.getPosition().x+expression.getGlobalBounds().width+8,expression.getPosition().y+7);

    //параметры для кнопки файла
    sf::RectangleShape file_button(sf::Vector2f{150, 65});
    file_button.setPosition(x_input_box.getPosition().x + expression_input_box.getSize().x - file_button.getSize().x, x_input_box.getPosition().y);
    file_button.setFillColor(in_box_button_color);
    file_button.setOutlineThickness(1);
    file_button.setOutlineColor(sf::Color::White);

    //кнопка файла
    sf::Text file_button_text(sf::String("LOAD"),dosis);
    file_button_text.setFillColor(text_color);
    file_button_text.setCharacterSize(kCharacterSize);
    file_button_text.setPosition(file_button.getPosition().x - 5 + (file_button.getSize().x-file_button_text.getGlobalBounds().width)/2,
                                    file_button.getPosition().y);

    //бокс для ввода названия файла
    sf::RectangleShape file_input_box(sf::Vector2f{screen_width * 0.4f, screen_height * 0.06f});
    file_input_box.setPosition(file_button.getPosition().x - file_input_box.getSize().x -15, x_input_box.getPosition().y);
    file_input_box.setFillColor(in_box_color);
    file_input_box.setOutlineThickness(1);
    file_input_box.setOutlineColor(sf::Color::White);

    //ввод текста в окно для файла
    sf::Text file(sf::String(""),dosis);
    file.setFillColor(text_color);
    file.setPosition(file_input_box.getPosition().x, file_input_box.getPosition().y);
    file.setCharacterSize(kCharacterSize);

    //фоновый текст на окне для ввода файла
    sf::Text file_background_text(sf::String("File name (path)"), dosis);
    file_background_text.setFillColor(background_text_color);
    file_background_text.setPosition(file_input_box.getPosition().x, file_input_box.getPosition().y);
    file_background_text.setCharacterSize(kCharacterSize);

    //сообщение об ошибке открытия файла
    sf::Text file_error_message(sf::String("Could not load file: file does not exist or its content is not an expression"),dosis);
    file_error_message.setFillColor(sf::Color::Red);
    file_error_message.setCharacterSize(20);
    file_error_message.setPosition(file_input_box.getSize().x + file_input_box.getPosition().x - file_error_message.getGlobalBounds().width,
                                   file_input_box.getSize().y + file_input_box.getPosition().y + 3);

    //сообщение об успешности открытия файла
    sf::Text file_success_message(sf::String("File loaded successfully."),dosis);
    file_success_message.setFillColor(sf::Color::Green);
    file_success_message.setCharacterSize(20);
    file_success_message.setPosition(file_input_box.getSize().x + file_input_box.getPosition().x - file_success_message.getGlobalBounds().width,
                                     file_input_box.getSize().y + file_input_box.getPosition().y + 3);

    //кнопка для генерирования
    sf::RectangleShape generator_button(sf::Vector2f{200, 130});
    generator_button.setPosition(output_boxes.at(2).getPosition().x, output_boxes.at(2).getPosition().y + 200);
    generator_button.setFillColor(in_box_button_color);
    generator_button.setOutlineThickness(1);
    generator_button.setOutlineColor(sf::Color::White);

    //текст в кнопке для генирирования
    sf::Text generator_button_text(sf::String("Generate\n variants"),dosis);
    generator_button_text.setFillColor(text_color);
    generator_button_text.setCharacterSize(kCharacterSize);
    generator_button_text.setPosition(generator_button.getPosition().x +
                                      (generator_button.getSize().x - generator_button_text.getGlobalBounds().width)/2,
                                      generator_button.getPosition().y - 15 +
                                      (generator_button.getSize().y - generator_button_text.getGlobalBounds().height)/2);

    //бокс для ввода количества вариантов
    sf::RectangleShape generator_input_box(sf::Vector2f{generator_button.getSize().x, screen_height*0.06f});
    generator_input_box.setPosition(generator_button.getPosition().x, generator_button.getPosition().y + generator_button.getSize().y + 15);
    generator_input_box.setFillColor(in_box_color);
    generator_input_box.setOutlineThickness(1);
    generator_input_box.setOutlineColor(sf::Color::White);

    //текст для ввода количества вариантов
    sf::Text generator(sf::String(""),dosis);
    generator.setFillColor(text_color);
    generator.setPosition(generator_input_box.getPosition().x, generator_input_box.getPosition().y);
    generator.setCharacterSize(kCharacterSize);

    //Фон в окне для ввода количества вариантов
    sf::Text generator_background_text(sf::String("Number"), dosis);
    generator_background_text.setFillColor(background_text_color);
    generator_background_text.setPosition(generator_input_box.getPosition().x, generator_input_box.getPosition().y);
    generator_background_text.setCharacterSize(kCharacterSize);

    //сообщение об ушибке ввода количества вар
    sf::Text generator_error_message(sf::String("Bad number of variants! (must be less than 100)"),dosis);
    generator_error_message.setFillColor(sf::Color::Red);
    generator_error_message.setCharacterSize(20);
    generator_error_message.setPosition(generator_input_box.getPosition().x,
            generator_input_box.getPosition().y + generator_input_box.getSize().y + 15);

    //сообщение об успешности ввода количества вар
    sf::Text generator_success_message(sf::String("Generated successfully"),dosis);
    generator_success_message.setFillColor(sf::Color::Green);
    generator_success_message.setCharacterSize(20);
    generator_success_message.setPosition(generator_input_box.getPosition().x ,
                                          generator_input_box.getPosition().y+ generator_input_box.getSize().y + 15);

    //кнопка выход
    sf::RectangleShape exit_button(sf::Vector2f{result_box.getSize().x, screen_height*0.06f});
    exit_button.setPosition(result_boxes.at(2).getPosition().x, generator_button.getPosition().y);
    exit_button.setFillColor(in_box_button_color);
    exit_button.setOutlineThickness(1);
    exit_button.setOutlineColor(sf::Color::White);

    //текст в кнопке выхода
    sf::Text exit_button_text(sf::String("Exit"),dosis);
    exit_button_text.setFillColor(text_color);
    exit_button_text.setCharacterSize(kCharacterSize);
    exit_button_text.setPosition(exit_button.getPosition().x+(exit_button.getSize().x-exit_button_text.getGlobalBounds().width)/2,exit_button.getPosition().y);

    //кнопка задания
    sf::RectangleShape individual_task_button(sf::Vector2f{result_box.getSize().x, screen_height*0.06f});
    individual_task_button.setPosition(exit_button.getPosition().x, exit_button.getPosition().y + exit_button.getSize().y + 15);
    individual_task_button.setFillColor(in_box_button_color);
    individual_task_button.setOutlineThickness(1);
    individual_task_button.setOutlineColor(sf::Color::White);

    //текст в кнопке задания
    sf::Text individual_task_button_text(sf::String("Individual task"),dosis);
    individual_task_button_text.setFillColor(text_color);
    individual_task_button_text.setCharacterSize(kCharacterSize);
    individual_task_button_text.setPosition(individual_task_button.getPosition().x + (individual_task_button.getSize().x -
                                individual_task_button_text.getGlobalBounds().width)/2,individual_task_button.getPosition().y);

    //инфа о программе
    sf::Text rules(sf::String(L"О программе\n"
                              "Input Expression - ввод любого выражения\n"
                              "Simple Expression - простое выражение и его результат\n"
                              "Direct Expression - выражение прямой нотации и его результат\n"
                              "Reverse Expression - выражение обратной нотации и его результат\n"
                              "LOAD - загрузка выражения из файла (введите 1,2 или 3)\n"
                              "GENERATE- сгенирировать до 100 (введите число) проверочных вариантов и ответов к ним \n"
                              "INDIVIDUAL TASK- индивидуальное задание\n"
                              "EXIT - выйти из программы"),afronic);
    rules.setFillColor(text_color);
    rules.setPosition(output_boxes.at(ExpressionNumber::reverse).getPosition().x+output_boxes.at(ExpressionNumber::reverse).getSize().x/3.7,
                      output_boxes.at(ExpressionNumber::reverse).getPosition().y+output_boxes.at(ExpressionNumber::reverse).getSize().y+120);
    rules.setCharacterSize(25);

    sf::Clock cursor_clock; // для мигания курсора
    sf::String expression_input, x_input, y_input, file_input, generator_input;
    bool invalid_x = false, invalid_y = false;
    int i;
    ActiveInputBox active_input_box = ActiveInputBox::expression;
    array<bool,4> draw_message{false,false,false,false};
    bool launch_individual_task = false;

    while (window.isOpen())
    {
        sf::Time cursor_time = cursor_clock.getElapsedTime();
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if(event.type == sf::Event::MouseButtonPressed)
            {
                for(auto& m : draw_message)
                {
                    m = false;
                }
                // боксы для ввода:
                if(mouseCursorIn(expression_input_box))
                {
                    updateCursorWithActiveInputBox(cursor,expression);
                    active_input_box = ActiveInputBox::expression;
                }
                if(mouseCursorIn(x_input_box))
                {
                    updateCursorWithActiveInputBox(cursor,x);
                    active_input_box = ActiveInputBox::x;
                }
                if(mouseCursorIn(y_input_box))
                {
                    updateCursorWithActiveInputBox(cursor,y);
                    active_input_box = ActiveInputBox::y;
                }
                if(mouseCursorIn(file_input_box))
                {
                    updateCursorWithActiveInputBox(cursor,file);
                    active_input_box = ActiveInputBox::file;
                }
                if(mouseCursorIn(generator_input_box))
                {
                    updateCursorWithActiveInputBox(cursor,generator);
                    active_input_box = ActiveInputBox::generator;
                }
                //кнопки :
                if(mouseCursorIn(file_button))
                {
                    mainE.reset();
                    if(!setExpressionFromFile(file.getString(),mainE))
                    {
                        draw_message.at(kFileError) = true;
                    }
                    else
                    {
                        expression.setString(mainE.getString());
                        updateMainE_X(mainE,x,invalid_x);
                        updateMainE_Y(mainE,y,invalid_y);
                        if(!invalid_x)
                        {
                            updateExpressions(simpleE,directE,reverseE,mainE);
                            updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE,mainE);
                        }
                        else
                        {
                            invalid_x = false;
                            mainE.x_value = 0;
                        }
                        if(!invalid_y)
                        {
                            updateExpressions(simpleE,directE,reverseE,mainE);
                            updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE,mainE);
                        }
                        else
                        {
                            invalid_y = false;
                            mainE.x_value = 0;
                        }
                        updateExpressions(simpleE,directE,reverseE,mainE);
                        updateOutputTexts(output_texts,result_texts,simpleE,directE,reverseE,mainE);
                        draw_message.at(kFileSuccess) = true;
                    }
                }
                if(mouseCursorIn(generator_button))
                {
                    int number_of_variants;
                    try
                    {
                        number_of_variants = stoi(string(generator.getString()));
                        if(number_of_variants >=100 || number_of_variants <= 0)
                        {
                            draw_message.at(Message::kGenError) = true;
                        }
                        else
                        {
                            TasksGenerator(number_of_variants);
                            draw_message.at(Message::kGenSuccess) = true;
                        }
                    }
                    catch(exception&)
                    {
                        draw_message.at(Message::kGenError) = true;
                    }
                }
                if(mouseCursorIn(exit_button))
                {
                    window.close();
                }
                if(mouseCursorIn(individual_task_button))
                {
                    launch_individual_task = true;
                    window.setVisible(false);
                    //window.close();
                }
            }
        }


        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }
        if(event.type == sf::Event::TextEntered)
        {
            for (auto &m : draw_message)
            {
                m = false;
            }
            switch (active_input_box)
            {
                case ActiveInputBox::expression:
                {
                    resetOutputTexts(output_texts, result_texts);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                    {
                        if (expression.getString().getSize() != 0)
                        {
                            expression_input = expression.getString();
                            expression_input.erase(expression_input.getSize() - 1, 1);
                            expression.setString(expression_input);
                            cursor.setPosition(expression.getPosition().x + expression.getGlobalBounds().width + 8,
                                               expression.getPosition().y + 7);

                            mainE.reset();
                            if (mainE.set(string(expression.getString())))
                            {
                                if (mainE.setType() != ExpressionType::none)
                                {
                                    updateExpressions(simpleE, directE, reverseE, mainE);
                                    updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE, mainE);
                                }
                            }
                        }
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ||
                               sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                               event.text.unicode > 127 ||
                               event.text.unicode < 0)
                    {/* do nothing */} // для ввода только англ, цифр
                    else
                    {
                        expression_input = expression.getString() + static_cast<char>(event.text.unicode);
                        expression.setString(expression_input);
                        updateCursorWithActiveInputBox(cursor, expression);

                        mainE.reset();
                        if (mainE.set(string(expression.getString())))
                        {
                            if (mainE.setType() != ExpressionType::none)
                            {
                                updateExpressions(simpleE, directE, reverseE, mainE);
                                updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE, mainE);
                            }
                        }
                    }
                }
                    break;
                case ActiveInputBox::x:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                    {
                        if (x.getString().getSize() != 0)
                        {
                            x_input = x.getString();
                            x_input.erase(x_input.getSize() - 1, 1);
                            x.setString(x_input);
                            updateCursorWithActiveInputBox(cursor, x);
                            updateMainE_X(mainE, x, invalid_x);
                            if (!invalid_x)
                            {
                                updateExpressions(simpleE, directE, reverseE, mainE);
                                updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE, mainE);
                            }
                            else
                            {
                                invalid_x = false;
                                mainE.x_value = 0;
                            }
                        }
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ||
                               sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || event.text.unicode > 127 ||
                               event.text.unicode < 0)
                    {/* do nothing */}
                    else
                    {
                        x_input = x.getString() + static_cast<char>(event.text.unicode);
                        x.setString(x_input);
                        updateCursorWithActiveInputBox(cursor, x);
                        updateMainE_X(mainE, x, invalid_x);
                        if (!invalid_x)
                        {
                            updateExpressions(simpleE, directE, reverseE, mainE);
                            updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE, mainE);
                        }
                        else
                        {
                            invalid_x = false;
                            mainE.x_value = 0;
                        }
                    }
                }
                    break;
                case ActiveInputBox::y:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                    {
                        if (y.getString().getSize() != 0)
                        {
                            y_input = y.getString();
                            y_input.erase(y_input.getSize() - 1, 1);
                            y.setString(y_input);
                            updateCursorWithActiveInputBox(cursor, y);
                            updateMainE_Y(mainE, y, invalid_y);
                            if (!invalid_y)
                            {
                                updateExpressions(simpleE, directE, reverseE, mainE);
                                updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE, mainE);
                            }
                            else
                            {
                                invalid_y = false;
                                mainE.x_value = 0;
                            }
                        }
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ||
                             sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || event.text.unicode > 127 ||
                             event.text.unicode < 0)
                    {/* do nothing */}
                    else
                    {
                        y_input = y.getString() + static_cast<char>(event.text.unicode);
                        y.setString(y_input);
                        updateCursorWithActiveInputBox(cursor, y);
                        updateMainE_Y(mainE, y, invalid_y);
                        if (!invalid_y)
                        {
                            updateExpressions(simpleE, directE, reverseE, mainE);
                            updateOutputTexts(output_texts, result_texts, simpleE, directE, reverseE, mainE);
                        }
                        else
                        {
                            invalid_y = false;
                            mainE.x_value = 0;
                        }
                    }
                }
                    break;
                case ActiveInputBox::file:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                    {
                        if (file.getString().getSize() != 0)
                        {
                            file_input = file.getString();
                            file_input.erase(file_input.getSize() - 1, 1);
                            file.setString(file_input);
                            updateCursorWithActiveInputBox(cursor, file);
                        }
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ||
                             sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || event.text.unicode > 127 ||
                             event.text.unicode < 0){ }
                    else
                    {
                        file_input = file.getString() + static_cast<char>(event.text.unicode);
                        file.setString(file_input);
                        updateCursorWithActiveInputBox(cursor, file);
                    }
                }
                    break;
                case ActiveInputBox::generator:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                    {
                        if (generator.getString().getSize() != 0)
                        {
                            generator_input = generator.getString();
                            generator_input.erase(generator_input.getSize() - 1, 1);
                            generator.setString(generator_input);
                            updateCursorWithActiveInputBox(cursor, generator);
                        }
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ||
                                   sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || event.text.unicode > 127 ||
                                   event.text.unicode < 0){ }
                    else
                    {
                        generator_input = generator.getString() + static_cast<char>(event.text.unicode);
                        generator.setString(generator_input);
                        updateCursorWithActiveInputBox(cursor, generator);
                    }
                }
                    break;
                case ActiveInputBox::none:
                    break;
            }
        }
    //}


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        window.clear();

        // отрисовка кнопок:
        window.draw(file_button_text);
        window.draw(file_button);
        window.draw(generator_button_text);
        window.draw(generator_button);
        window.draw(exit_button_text);
        window.draw(exit_button);
        window.draw(individual_task_button_text);
        window.draw(individual_task_button);

        window.draw(rules);

        if(expression.getString() == "")
        {
            window.draw(expression_background_text);
        }
        else
        {
            window.draw(expression);
        }

        if(x.getString() == "")
        {
            window.draw(x_background_text);
        }
        else
        {
            window.draw(x);
        }

        if(y.getString() == "")
        {
            window.draw(y_background_text);
        }
        else
        {
            window.draw(y);
        }

        if(file.getString() == "")
        {
            window.draw(file_background_text);
        }
        else
        {
            window.draw(file);
        }

        if(generator.getString() == "")
        {
            window.draw(generator_background_text);
        }
        else
        {
            window.draw(generator);
        }

        if(sf::seconds(0.0f) <= cursor_time && cursor_time <= sf::seconds(0.5f))
        {
            window.draw(cursor);
        }
        else if(cursor_time >= sf::seconds(1.f))
        {
            cursor_clock.restart();
        }

        for(const auto& ob: output_boxes)
        {
            window.draw(ob);
        }
        for(const auto& rb : result_boxes)
        {
            window.draw(rb);
        }

        for(const auto& eq : equal_signs)
        {
            window.draw(eq);
        }

        i = 0;
        for(const auto& ot : output_texts)
        {
            if(ot.getString() == "")
            {
                window.draw(background_texts.at(i));
            }
            else
            {
                window.draw(ot);
            }
            ++i;
        }
        i = 0;
        for(const auto& rt : result_texts)
        {
            if(rt.getString() == "")
            {
                window.draw(background_result_texts.at(i));
            }
            else
            {
                window.draw(rt);
            }
            ++i;
        }

        // отрисовка боксов
        window.draw(expression_input_box);
        window.draw(x_input_box);
        window.draw(y_input_box);
        window.draw(generator_input_box);
        window.draw(file_input_box);
        i=0;
        for(const auto& m : draw_message)
        {
            if(m)
            {
                switch (i)
                {
                    case Message::kFileError:   window.draw(file_error_message);        break;
                    case Message::kFileSuccess: window.draw(file_success_message);      break;
                    case Message::kGenError:    window.draw(generator_error_message);   break;
                    case Message::kGenSuccess:  window.draw(generator_success_message); break;
                    default: break;
                }
            }
            ++i;
        }
        window.display();

        if(launch_individual_task)
        {
            IndividualTask();
            window.setVisible(true);
            launch_individual_task = false;
        }
    }

    return 0;
}