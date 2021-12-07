//
// Created by Lada on 21.04.2020.
//

#ifndef LAB_3_EXPRESSION_H
#define LAB_3_EXPRESSION_H

#include <iostream>
#include <vector>
#include <sstream>
#include <array>
#include <stack>
#include <cmath>

using namespace std;

enum class ExpressionType //вид выражения
{
    simple,
    direct, //прямая нотация
    reverse, //обратная нотация
    none,
};
enum class PartType //детали выражний
{
    operatorPT,
    operand,
    variable,
    bracket, //скобоки в простых выражениях
    none,
};
union PartData //одна единица памяти для всех
{
    double operand;
    char operatorP;
    char variable;
};
struct PartOfExpression //для PartData с его типом
{
    PartData data{};
    PartType type = PartType::none;
};

struct Expression //данные о выражении и функции для них
{
    vector<PartOfExpression> vec{};
    ExpressionType type = ExpressionType::none;
    double expression_value = 0;

    bool has_x = false;
    bool has_y = false;
    bool set_x = false;
    bool set_y = false;
    int x_value = 0;
    int y_value = 0;


    void reset();
    bool set(const string& expression_str);
    ExpressionType setType();
    double calculate();
    string getString();
    void convert(ExpressionType type_to_convert);

    void convertSimpleInDirect();
    void convertSimpleInReverse();
    void convertDirectInSimple();
    void convertReverseInSimple();
    void convertReverseInDirect();
    void convertDirectInReverse();

    void print();
};

void doSimpleCalculation(stack<PartOfExpression>& operators, stack<PartOfExpression>& operands); //операция после каждого знака имея 2 числа и знак

int priority(PartOfExpression p); //приоритет оператора

ostream& operator<< (ostream &out, const ExpressionType &type); //для типа, чтобы выводить тип в строчку

#endif //LAB_3_EXPRESSION_H
