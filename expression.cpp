//
// Created by Lada on 21.04.2020.
//

#include "expression.h"


// сбросить выр-ние
void Expression::reset()
{
    vec.erase(vec.begin(), vec.end());
    type = ExpressionType::none;
    has_x = false;
    has_y = false;
    x_value = 0;
    y_value = 0;
}

// установка выражний из строки
bool Expression::set(const string& expression_str)
{
    string part_of_expression_str;
    stringstream converter;
    PartOfExpression part_of_expression;
    const array<string, 9> good_symbols{"+", "-", "/", "*", "^", "x", "y", "(", ")"};

    converter << expression_str;

    bool is_digit;
    while(converter >> part_of_expression_str) //обрабатывает каждую часть выражния
    {
        is_digit = true;

        //проверить является ли чать выр-я цифрой
        int i = 0;
        for(const auto& symbol : part_of_expression_str)
        {
            if(part_of_expression_str.size() == 1)
            {
                if(!isdigit(symbol))
                {
                    is_digit = false;
                    break;
                }
            }
            if(i == 0)
            {
                if(!isdigit(symbol))
                {
                    if(symbol != '-')
                    {
                        is_digit = false;
                        break;
                    }
                }
            }
            else if(!isdigit(symbol))
            {
                is_digit = false;
                break;
            }
            ++i;
        }

        //проверить является ли чать выр-я подходящим символом
        if(!is_digit)
        {
            int iteration{0};
            for (const auto& good_symbol : good_symbols)
            {
                if (part_of_expression_str != good_symbol)
                {
                    if (iteration == good_symbols.size() - 1)
                    {
                        //если не и не подходящ символ и не цифры - загадка дыры!
                        reset();
                        return false;
                    }
                    ++iteration;
                }
                else
                {
                    break;
                }
            }
            if(part_of_expression_str == "x")
            {
                has_x = true;
                part_of_expression.type = PartType::variable;
                part_of_expression.data.variable = part_of_expression_str.at(0);
                vec.push_back(part_of_expression);
            }
            else if(part_of_expression_str == "y")
            {
                has_y = true;
                part_of_expression.type = PartType::variable;
                part_of_expression.data.variable = part_of_expression_str.at(0);
                vec.push_back(part_of_expression);
            }
            else if(part_of_expression_str == ")" || part_of_expression_str == "(")
            {
                part_of_expression.type = PartType::bracket;
                part_of_expression.data.operatorP = part_of_expression_str.at(0);
                vec.push_back(part_of_expression);
            }
            else
            {
                part_of_expression.type = PartType::operatorPT;
                part_of_expression.data.operatorP = part_of_expression_str.at(0);
                vec.push_back(part_of_expression);
            }
        }
        else // если является цифрой
        {
            part_of_expression.type = PartType::operand;
            try
            {
                part_of_expression.data.operand = stoi(part_of_expression_str);
            }
            catch(invalid_argument&)
            {
                reset();
                return false;
            }
            catch(out_of_range&)
            {
                reset();
                return false;
            }
            vec.push_back(part_of_expression);
        }
    }
    return true;
}

//проверка корректности и устанавливаем тип
ExpressionType Expression::setType()
{
    if(vec.size() < 3)
        return ExpressionType::none;

    ExpressionType expression_type_to_set{ExpressionType::simple};
    stack<char> brackets_stack;
    stack<PartOfExpression> expression_stack;

    //первая часть - операнд, оператор или скобка
    if(vec.at(0).type == PartType::operatorPT)
    {
        expression_type_to_set = ExpressionType::direct;
    }
    else if(vec.at(0).type == PartType::bracket)
    {
        if(vec.at(0).data.operatorP == ')')
        {
            expression_type_to_set = ExpressionType::none;
        }
        else
        {
            brackets_stack.push(vec.at(0).data.operatorP);
            expression_type_to_set = ExpressionType::simple;
        }
    }

    //последняя часть - операнд, оператор или скобка
    if(vec.at(vec.size()-1).type == PartType::operatorPT)
    {
        expression_type_to_set = ExpressionType::reverse;
    }
    else if(vec.at(vec.size()-1).type == PartType::bracket)
    {
        if(vec.at(vec.size()-1).data.operatorP == '(')
        {
            expression_type_to_set = ExpressionType::none;
        }
        else
        {
            if(vec.at(vec.size()-1).data.operatorP == '(')
            {
                expression_type_to_set = ExpressionType::none;
            }
            else
            {
                expression_type_to_set = ExpressionType::simple;
            }
        }
    }

    //сравнить части
    if(vec.at(0).type == PartType::operatorPT && vec.at(vec.size()-1).type == PartType::operatorPT)
    {
        return ExpressionType::none;
    }

    //заполняем стэк expression_stack и brackets_stack
    expression_stack.push(vec.at(0));
    for(int i{1}; i < vec.size(); ++i)
    {
        switch(vec.at(i).type)
        {
            case PartType::operatorPT:
            case PartType::operand:
            case PartType::variable:
            {
                expression_stack.push(vec.at(i));
            }
                break;
            case PartType::bracket:
            {
                if(vec.at(i).data.operatorP == '(')
                {
                    brackets_stack.push('(');
                }
                else if(vec.at(i).data.operatorP == ')')
                {
                    if(!brackets_stack.empty() && vec.at(i-1).data.operatorP != '(')
                        brackets_stack.pop();
                    else
                    {
                        return ExpressionType::none;
                    }
                }
            }
                break;
            case PartType::none:
            {
                type = ExpressionType::none;
                return type;
            }
        }
    }

    if(!brackets_stack.empty())
    {
        type = ExpressionType::none;
        return type;
    }

    //для установки типа
    switch (expression_type_to_set)
    {
        case ExpressionType::simple:
        {
            PartOfExpression cache;
            while (expression_stack.size() > 1)
            {
                cache = expression_stack.top();
                expression_stack.pop();
                if (cache.type == expression_stack.top().type)
                {
                    type = ExpressionType::none;
                    return type;
                }
            }
            type = ExpressionType::simple;
            return type;
        }
        case ExpressionType::direct:
        {
            stack<PartOfExpression> direct_stack;
            while (!expression_stack.empty())
            {
                if(expression_stack.top().type == PartType::operand)
                {
                    direct_stack.push(expression_stack.top());
                    expression_stack.pop();
                }
                else if(expression_stack.top().type == PartType::variable)
                {
                    PartOfExpression tmp;
                    tmp.type = PartType::operand;
                    tmp.data.operand = 1; //проверка на корректность прямой
                    direct_stack.push(tmp);
                    expression_stack.pop();
                }
                else if(expression_stack.top().type == PartType::operatorPT)
                {
                    char operator_ = expression_stack.top().data.operatorP;
                    expression_stack.pop();
                    double result = direct_stack.top().data.operand;
                    direct_stack.pop();
                    switch (operator_)
                    {
                        case '+':
                        {
                            if(!direct_stack.empty())
                            {
                                result += direct_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '-':
                        {
                            if(!direct_stack.empty())
                            {
                                result -= direct_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '*':
                        {
                            if(!direct_stack.empty())
                            {
                                result *= direct_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '/':
                        {
                            if(!direct_stack.empty())
                            {
                                result /= direct_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '^':
                        {
                            if(!direct_stack.empty())
                            {
                                result = pow(result,direct_stack.top().data.operand);
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        default:
                        {
                            type = ExpressionType::none;
                            return type;
                        }
                    }
                    direct_stack.pop();
                    PartOfExpression tmp;
                    tmp.type = PartType::operand;
                    tmp.data.operand = result;
                    direct_stack.push(tmp);
                }
                else //если встретили скобки
                {
                    type = ExpressionType::none;
                    return type;
                }
            }
            if(direct_stack.size() != 1 )
            {
                type = ExpressionType::none;
                return type;
            }
            else
            {
                type = ExpressionType::direct;
                return type;
            }
        }
        case ExpressionType::reverse:
        {
            stack<PartOfExpression> reverse_stack;
            stack<PartOfExpression> expression_stack2;
            while(!expression_stack.empty())
            {
                expression_stack2.push(expression_stack.top());
                expression_stack.pop();
            }
            while(!expression_stack2.empty())
            {
                if(expression_stack2.top().type == PartType::operand)
                {
                    reverse_stack.push(expression_stack2.top());
                    expression_stack2.pop();
                }
                else if(expression_stack2.top().type == PartType::variable)
                {
                    PartOfExpression tmp;
                    tmp.type = PartType::operand;
                    tmp.data.operand = 1;
                    reverse_stack.push(tmp);
                    expression_stack2.pop();
                }
                else if(expression_stack2.top().type == PartType::operatorPT)
                {
                    char operator_ = expression_stack2.top().data.operatorP;
                    expression_stack2.pop();
                    double result = reverse_stack.top().data.operand;
                    reverse_stack.pop();
                    switch (operator_)
                    {
                        case '+':
                        {
                            if(!reverse_stack.empty())
                            {
                                result += reverse_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '-':
                        {
                            if(!reverse_stack.empty())
                            {
                                result -= reverse_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '*':
                        {
                            if(!reverse_stack.empty())
                            {
                                result *= reverse_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '/':
                        {
                            if(!reverse_stack.empty())
                            {
                                result /= reverse_stack.top().data.operand;
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        case '^':
                        {
                            if(!reverse_stack.empty())
                            {
                                result = pow(result,reverse_stack.top().data.operand);
                            }
                            else
                            {
                                type = ExpressionType::none;
                                return type;
                            }
                        }
                            break;
                        default:
                        {
                            type = ExpressionType::none;
                            return type;
                        }
                    }
                    reverse_stack.pop();
                    PartOfExpression tmp;
                    tmp.type = PartType::operand;
                    tmp.data.operand = result;
                    reverse_stack.push(tmp);
                }
                else //если скобки в обратной
                {
                    type = ExpressionType::none;
                    return type;
                }
            }
            if(reverse_stack.size() != 1 )
            {
                type = ExpressionType::none;
                return type;
            }
            else
            {
                type = ExpressionType::reverse;
                return type;
            }
        }
        case ExpressionType::none:
        {
            type = ExpressionType::none;
            return type;
        }
    }
}

//вычисления
double Expression::calculate()
{
    if(vec.empty())
    {
        return 0;
    }

    if(has_x && !set_x || has_y && !set_y)
    {
        return 0;
    }

    PartOfExpression variable_replacer;
    variable_replacer.type = PartType::operand;

    switch (type)
    {
        case ExpressionType::simple:
        {
            stack<PartOfExpression> stack_operands;
            stack<PartOfExpression> stack_operators;
            bool broke;
            for(const auto& part : vec)
            {
                broke = false;
                switch(part.type)
                {
                    case PartType::operatorPT:
                    {
                        if(stack_operators.empty())
                        {
                            stack_operators.push(part);
                        }
                        else if(priority(part) > priority(stack_operators.top()))
                        {
                            stack_operators.push(part);
                        }
                        else
                        {
                            while(priority(part) <= priority(stack_operators.top()))
                            {
                                doSimpleCalculation(stack_operators, stack_operands);
                                if(stack_operators.empty())
                                {
                                    stack_operators.push(part);
                                    broke = true;
                                    break;
                                }

                            }
                            if(!broke)
                                stack_operators.push(part);
                        }
                    }
                        break;
                    case PartType::operand:
                    {
                        stack_operands.push(part);
                    }
                        break;
                    case PartType::variable:
                    {
                        if(part.data.variable == 'x')
                        {
                            variable_replacer.data.operand = x_value;
                        }
                        else
                        {
                            variable_replacer.data.operand = y_value;
                        }
                        stack_operands.push(variable_replacer);
                    }
                        break;
                    case PartType::bracket:
                    {
                        if(part.data.operatorP == '(')
                        {
                            stack_operators.push(part);
                        }
                        else if(part.data.operatorP == ')')
                        {
                            while(stack_operators.top().data.operatorP != '(')
                            {
                                doSimpleCalculation(stack_operators, stack_operands);
                            }
                            stack_operators.pop();
                        }
                    }
                        break;
                    case PartType::none:
                    {
                        throw runtime_error("Error: calculate(): Unexpected type of part of expression");
                    }
                }
            }
            while(!stack_operators.empty())
            {
                doSimpleCalculation(stack_operators, stack_operands);
            }
            expression_value = stack_operands.top().data.operand;
            return expression_value;
        }
        case ExpressionType::direct:
        {
            stack<PartOfExpression> direct_stack;
            for(unsigned long i = vec.size()-1; i != -1; --i)
            {
                if(vec.at(i).type == PartType::operand)
                {
                    direct_stack.push(vec.at(i));
                }
                else if(vec.at(i).type == PartType::variable)
                {
                    if(vec.at(i).data.variable == 'x')
                    {
                        variable_replacer.data.operand = x_value;
                    }
                    else
                    {
                        variable_replacer.data.operand = y_value;
                    }
                    direct_stack.push(variable_replacer);
                }
                else if(vec.at(i).type == PartType::operatorPT)
                {
                    char operator_ = vec.at(i).data.operatorP;
                    double result = direct_stack.top().data.operand;
                    direct_stack.pop();
                    switch (operator_)
                    {
                        case '+': result += direct_stack.top().data.operand; break;
                        case '-': result -= direct_stack.top().data.operand; break;
                        case '*': result *= direct_stack.top().data.operand; break;
                        case '/': result /= direct_stack.top().data.operand; break;
                        case '^': result = pow(result,direct_stack.top().data.operand); break;
                        default: throw runtime_error("Error: calculate(): Unexpected operator value");
                    }
                    direct_stack.pop();
                    PartOfExpression tmp;
                    tmp.type = PartType::operand;
                    tmp.data.operand = result;
                    direct_stack.push(tmp);
                }
            }
            expression_value = direct_stack.top().data.operand;
            return expression_value;
        }
        case ExpressionType::reverse:
        {
            stack<PartOfExpression> reverse_stack;
            for (const auto& part : vec)
            {
                if (part.type == PartType::operand)
                {
                    reverse_stack.push(part);
                }
                else if (part.type == PartType::variable)
                {
                    if(part.data.variable == 'x')
                    {
                        variable_replacer.data.operand = x_value;
                    }
                    else
                    {
                        variable_replacer.data.operand = y_value;
                    }
                    reverse_stack.push(variable_replacer);
                }
                else if (part.type == PartType::operatorPT)
                {
                    double result = reverse_stack.top().data.operand;
                    reverse_stack.pop();
                    switch (part.data.operatorP)
                    {
                        case '+': reverse_stack.top().data.operand += result; break;
                        case '-': reverse_stack.top().data.operand -= result; break;
                        case '*': reverse_stack.top().data.operand *= result; break;
                        case '/': reverse_stack.top().data.operand /= result; break;
                        case '^': reverse_stack.top().data.operand =  pow(reverse_stack.top().data.operand,result); break;
                        default: throw runtime_error("Error: Unexpected operator value");
                    }
                    //reverse_stack.pop();
                    //PartOfExpression tmp;
                    //tmp.type = PartType::operand;
                    //tmp.data.operand = result;
                    //reverse_stack.push(tmp);
                }
            }
            expression_value = reverse_stack.top().data.operand;
            return expression_value;
        }
        case ExpressionType::none:
            return 0;
    }
}

//преобразования
void Expression::convert(ExpressionType type_to_convert)
{
    switch (type)
    {
        case ExpressionType::simple:
        {
            switch (type_to_convert)
            {
                case ExpressionType::direct:
                    convertSimpleInDirect();
                    break;
                case ExpressionType::reverse:
                    convertSimpleInReverse();
                    break;
                case ExpressionType::simple:
                case ExpressionType::none:
                    break;
            }
        }
            return;
        case ExpressionType::direct:
        {
            switch (type_to_convert)
            {
                case ExpressionType::simple:
                    convertDirectInSimple();
                    break;
                case ExpressionType::reverse:
                    convertDirectInReverse();
                    break;
                case ExpressionType::direct:
                case ExpressionType::none:
                    break;
            }
        }
            return;
        case ExpressionType::reverse:
        {
            switch (type_to_convert)
            {
                case ExpressionType::simple:
                    convertReverseInSimple();
                    break;
                case ExpressionType::direct:
                    convertReverseInDirect();
                    break;
                case ExpressionType::reverse:
                case ExpressionType::none:
                    break;
            }
        }
            return;
        case ExpressionType::none:
            return;
    }
}

//преобразование простого выражения в префиксное
void Expression::convertSimpleInDirect()
{
    stack<PartOfExpression> result;
    stack<PartOfExpression> stack_operators;
    for(unsigned long i = vec.size()-1; i != -1; --i)
    {
        switch(vec.at(i).type)
        {
            case PartType::operatorPT:
            {
                if(stack_operators.empty())
                {
                    stack_operators.push(vec.at(i));
                }
                else if(priority(vec.at(i)) < priority(stack_operators.top()))
                {
                    while(priority(stack_operators.top()) > priority(vec.at(i)))
                    {
                        result.push(stack_operators.top());
                        stack_operators.pop();
                        if(stack_operators.empty())
                        {
                            break;
                        }
                    }
                    stack_operators.push(vec.at(i));
                }
                else
                {
                    stack_operators.push(vec.at(i));
                }
            }
                break;
            case PartType::operand:
                //result.push(vec.at(i));
                //break;
            case PartType::variable:
                result.push(vec.at(i));
                break;
            case PartType::bracket:
            {
                if(vec.at(i).data.operatorP == ')')
                {
                    stack_operators.push(vec.at(i));
                }
                else if(vec.at(i).data.operatorP == '(')
                {
                    while(stack_operators.top().data.operatorP != ')')
                    {
                        result.push(stack_operators.top());
                        stack_operators.pop();
                    }
                    stack_operators.pop();
                }
            }
                break;
            case PartType::none:
                return;
        }
    }
    while(!stack_operators.empty())
    {
        result.push(stack_operators.top());
        stack_operators.pop();
    }
    reset();
    type = ExpressionType::direct;
    while(!result.empty())
    {
        vec.push_back(result.top());
        result.pop();
    }
}

//преобразование простого выражения в постфиксное
void Expression::convertSimpleInReverse()
{
    vector<PartOfExpression> result;
    stack<PartOfExpression> stack_operators;
    for(const auto& part : vec)
    {
        switch (part.type)
        {
            case PartType::operatorPT:
            {
                if(!stack_operators.empty())
                {
                    while (priority(part) <= priority(stack_operators.top()))
                    {
                        result.push_back(stack_operators.top());
                        stack_operators.pop();
                        if(stack_operators.empty())
                        {
                            break;
                        }
                    }
                }
                stack_operators.push(part);
            }
                break;
            case PartType::operand:
                //result.push_back(part);
                //break;
            case PartType::variable:
                result.push_back(part);
                break;
            case PartType::bracket:
            {
                if (part.data.operatorP == ')')
                {
                    while (stack_operators.top().data.operatorP != '(')
                    {
                        result.push_back(stack_operators.top());
                        stack_operators.pop();
                        if(stack_operators.empty())
                        {
                            break;
                        }
                    }
                    stack_operators.pop();
                }
                else if (part.data.operatorP == '(')
                {
                    stack_operators.push(part);
                }
            }
                break;
            case PartType::none:
                break;
        }
    }
    while(!stack_operators.empty())
    {
        result.push_back(stack_operators.top());
        stack_operators.pop();
    }
    reset();
    type = ExpressionType::reverse;
    vec = result;
}

//преобразования прямой нотации в простое выр-ние
void Expression::convertDirectInSimple()
{
    vector<PartOfExpression> result;
    stack<PartOfExpression> stack_operators;
    stack<PartOfExpression> close_brackets;

    PartOfExpression close_bracket;
    close_bracket.type = PartType::bracket;
    close_bracket.data.operatorP = ')';

    PartOfExpression open_bracket;
    open_bracket.type = PartType::bracket;
    open_bracket.data.operatorP = '(';

    for(const auto& part : vec)
    {
        switch (part.type)
        {
            case PartType::operatorPT:
            {
                if(stack_operators.empty())
                {
                    stack_operators.push(part);
                }
                else if (result.empty())
                {
                    stack_operators.push(part);
                }
                else if(priority(part) >= priority(stack_operators.top()))
                {
                    //result.push_back(stack_operators.top());
                    //stack_operators.pop();
                    if(result.at(result.size()-1).type == PartType::operatorPT)
                    {
                        stack_operators.push(part);
                    }
                    else if(result.at(result.size()-1).data.operatorP == '(')
                    {
                        stack_operators.push(part);
                    }
                    else
                    {
                        result.push_back(stack_operators.top());
                        stack_operators.pop();
                        stack_operators.push(part);
                    }
                }
                else
                {
                    if (result.at(result.size() - 1).type == PartType::operand)
                    {
                        result.push_back(stack_operators.top());
                        stack_operators.pop();
                    }
                    result.push_back(open_bracket);
                    stack_operators.push(part);
                    close_brackets.push(close_bracket);
                }
            }
                break;
            case PartType::operand:
            case PartType::variable:
            {
                if(result.empty())
                {
                    result.push_back(open_bracket);
                    close_brackets.push(close_bracket);
                    result.push_back(part);
                }
                else if (result.at(result.size() - 1).type == PartType::operand || result.at(result.size() - 1).type == PartType::variable)
                {
                    result.push_back(stack_operators.top());
                    stack_operators.pop();
                    result.push_back(part);
                    if(!close_brackets.empty())
                    {
                        result.push_back(close_brackets.top());
                        close_brackets.pop();
                    }
                }
                else if(result.at(result.size()-1).data.operatorP == ')')
                {
                    if(!stack_operators.empty())
                    {
                        if (priority(stack_operators.top()) >= 2)
                        {
                            if(!close_brackets.empty())
                            {
                                result.push_back(close_brackets.top());
                                close_brackets.pop();
                            }
                        }
                    }
                    result.push_back(stack_operators.top());
                    stack_operators.pop();
                    result.push_back(part);
                    if(!close_brackets.empty())
                    {
                        result.push_back(close_brackets.top());
                        close_brackets.pop();
                    }
                }
                else
                {
                    result.push_back(open_bracket);
                    result.push_back(part);
                    close_brackets.push(close_bracket);
                }
            }
                break;
            case PartType::bracket:
            case PartType::none:
                break;
        }
    }

    while(!close_brackets.empty())
    {
        result.push_back(close_bracket);
        close_brackets.pop();
    }

    reset();
    type = ExpressionType::simple;
    for(const auto& part : result)
    {
        vec.push_back(part);
    }
}

//преобразования обратной нотации в простое выр-ние
void Expression::convertReverseInSimple()
{
    stack<string> stack_result;
    string operand1, operand2, tmp_string;

    for(const auto& part : vec)
    {
        switch(part.type)
        {
            case PartType::operatorPT:
                operand2 = stack_result.top();
                stack_result.pop();
                operand1 = stack_result.top();
                stack_result.pop();

                tmp_string.push_back('(');
                tmp_string.append(operand1);
                tmp_string.push_back(part.data.operatorP);
                tmp_string.append(operand2);
                tmp_string.push_back(')');

                stack_result.push(tmp_string);
                tmp_string.clear();
                break;
            case PartType::operand:
                stack_result.push(to_string(static_cast<int>(part.data.operand)));
                break;
            case PartType::variable:
                stack_result.push(string()+static_cast<char>(part.data.variable));
                break;
            case PartType::bracket:
            case PartType::none:
                break;
        }
    }
    reset();
    for(const auto& symbol : stack_result.top())
    {
        if(isdigit(symbol))
        {
            tmp_string.push_back(symbol);
        }
        else if(tmp_string.empty())
        {
            tmp_string.push_back(symbol);
            tmp_string.push_back(' ');
        }
        else if(isdigit(tmp_string.at(tmp_string.size()-1)))
        {
            tmp_string.push_back(' ');
            tmp_string.push_back(symbol);
            tmp_string.push_back(' ');
        }
        else
        {
            tmp_string.push_back(symbol);
            tmp_string.push_back(' ');
        }
    }
    set(tmp_string);
    setType();
    calculate();
}

//преобразования обратной нотации в прямую
void Expression::convertReverseInDirect()
{
    convertReverseInSimple();
    convertSimpleInDirect();
}

//преобразования прямой нотации в обратную
void Expression::convertDirectInReverse()
{
    convertDirectInSimple();
    convertSimpleInReverse();
}

string Expression::getString()
{
   string result;
    for(const auto& part: vec)
    {
        switch (part.type)
        {
            case PartType::operatorPT:
                result.append(1, static_cast<char>(part.data.operatorP));
                result.append(1, ' ');
                break;
            case PartType::operand:
                //cout << part.data.operand << " ";
                result.append(to_string(static_cast<int>(part.data.operand)));
                result.append(1, ' ');
                break;
            case PartType::variable:
                //cout << part.data.variable << " ";
                result.append(1, static_cast<char>(part.data.variable));
                result.append(1, ' ');
                break;
            case PartType::bracket:
                //cout << part.data.operatorP << " ";
                result.append(1, static_cast<char>(part.data.operatorP));
                result.append(1, ' ');
                break;
            case PartType::none:
                break;
        }
    }
    return result;
}

int priority(PartOfExpression p)
{
    switch(p.data.operatorP)
    {
        case '+': return 1;
        case '-': return 1;
        case '*': return 2;
        case '/': return 2;
        case '^': return 3;
        case ')': return 0;
        case '(': return 0;
        default: throw runtime_error("Error: priority(): Unexpected operator value");
    }
}

void doSimpleCalculation(stack<PartOfExpression>& operators, stack<PartOfExpression>& operands) //вычисление при знаке двух помещенных в стек чисел
{
    PartOfExpression tmp;
    tmp.type = PartType::operand;
    double var1, var2;

    var2 = operands.top().data.operand;
    operands.pop();
    var1 = operands.top().data.operand;
    operands.pop();

    switch (operators.top().data.operatorP)
    {
        case '+':
            tmp.data.operand = var1 + var2;
            break;
        case '-':
            tmp.data.operand = var1 - var2;
            break;
        case '*':
            tmp.data.operand = var1 * var2;
            break;
        case '/':
            tmp.data.operand = var1 / var2;
            break;
        case '^':
            tmp.data.operand = pow(var1, var2);
            break;
        default:
            throw runtime_error("Error: Unexpected operator value");
    }
    operators.pop();
    operands.push(tmp);
}

ostream& operator<< (ostream &out, const ExpressionType &type) //чтобы тип выводился строчкой 
{
    switch(type)
    {
        case ExpressionType::simple:  out << "simple";  break;
        case ExpressionType::direct:  out << "direct";  break;
        case ExpressionType::reverse: out << "reverse"; break;
        case ExpressionType::none:    out << "none";    break;
    }
    return out;
}

void Expression::print()
{
    cout << "Expression (type " << type << "):\n";
    for(const auto& part: vec)
    {
        switch (part.type)
        {
            case PartType::operatorPT:
                cout << part.data.operatorP << " ";
                break;
            case PartType::operand:
                cout << part.data.operand << " ";
                break;
            case PartType::variable:
                cout << part.data.variable << " ";
                break;
            case PartType::bracket:
                cout << part.data.operatorP << " ";
                break;
            case PartType::none:
                break;
        }
    }
}
