#include "widget.h"
#include "ui_widget.h"
#include <QPixmap>
#include <QMovie>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#include <random>
#include <string>
#include <QString>
#include <sstream>
#include <stack>
#include <QPushButton>
#include <QDialogButtonBox>
#include <fstream>
#include <iterator>
#include <vector>


using namespace std;
int  k=0,j=1;


//**************************************************************************************************************************
//计算表达式的值
template<typename T>
class Stack {
public:
    void clear();
    bool push(const T item);
    bool pop(T& item);
    bool showTop(T& item);
    bool isEmpty();
    bool isFull();
};

template<typename T>
class arrayStack : public Stack<T> {
private:
    int maxSize;
    int top;
    T* stack;
public:
    arrayStack(int size) {
        maxSize = size;
        top = -1;
        stack = new T[maxSize];
    }

    arrayStack() {
        top = -1;
    }

    void clear() {
        top = -1;
    }

    bool push(const T item) {
        if (top == maxSize - 1) {
            cout << "stack overflow" << endl;
            return false;
        }
        else {
            stack[++top] = item;
            return true;
        }
    }

    bool pop(T& item) {
        if (top == -1) {
            cout << "the stack is empty when trying to pop" << endl;
            return false;
        }
        else {
            item = stack[top--];
            return true;
        }
    }

    void pop() {
        top--;
    }

    bool showTop(T& item) {
        if (top == -1) {
            cout << "the stack is empty" << endl;
            return false;
        }
        else {
            item = stack[top];
            return true;
        }
    }

    bool isEmpty() {
        return top == -1;
    }
};

template<class T>
T stringToNum(const string& str) {
    istringstream iss(str);
    T num;
    iss >> num;
    return num;
}

//优先级编码
int priority(char c) {
    if (c <= '9' && c >= '0')
        return 5;
    else if (c <= 'z' && c >= 'a')
        return 5;
    else if (c <= 'Z' && c >= 'A')
        return 5;
    else if (c == '.')
        return 6;
    else {
        switch (c) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '(':
            return 4;
        case ')':
            return 3;
        default:
            return 0;
        }
    }
}

bool isOperator(char c) {
    char op[5] = { '+', '-', '*', '/', ')' };
    int i;
    for (i = 0; i < 5; ++i) {
        if (c == op[i])
            return true;
    }
    return false;
}

bool isNumber(char c) {
    if (c <= '9' && c >= '0') {
        return true;
    }
    return false;
}

void addSpace(string& str) {
    str += ' ';
}

//添加0，解决负数错误问题
void format(string& str) {
    for (int i = 0; i < str.length(); i++)
        if (str[i] == '-')
            if (i == 0)
                str.insert(0, 1, '0');
            else if (str[i - 1] == '(')
                str.insert(i, 1, '0');
}

//中缀转后缀
int convert(string InfixExp, string& PostfixExp) {
    arrayStack<char>* s = new arrayStack<char>(50);
    int i;
    s->push('#'); //存入栈底后缀表达式结尾
    for (i = 0; i < InfixExp.length(); i++) {
        switch (priority(InfixExp[i])) {
        case 6:   //如果是小数点，直接输出
        case 5:   //如果是数字，直接输出
            PostfixExp += InfixExp[i];
            if (isOperator(InfixExp[i + 1]) || i == InfixExp.length() - 1) {
                addSpace(PostfixExp);
            }
            break;
        case 4:   //左括号直接入栈
            s->push('(');
            break;
        case 3:  //右括号，判空，依次弹出直到左括号
            if (s->isEmpty()) {
                s->clear();
                cout << "brackets do not match" << endl;
                return 1;
            }
            else {
                char temp;
                s->showTop(temp);
                while (temp != '(' && !s->isEmpty()) {
                    PostfixExp += temp;
                    addSpace(PostfixExp);
                    s->pop();
                    s->showTop(temp);
                }
            }
            s->pop();
            break;
        case 2: //当输入运算符
        case 1:
            char top;
            s->showTop(top);
            while (!s->isEmpty() && top != '(' && (priority(top) >= priority(InfixExp[i]))) {
                PostfixExp += top;
                addSpace(PostfixExp);
                s->pop();
                s->showTop(top);
            }
            s->push(InfixExp[i]);
            break;
        }
    }
    //输出剩余栈中的元素
    while (!s->isEmpty()) {
        char temp;
        s->pop(temp);
        PostfixExp += temp;
    }
    return 0;
}
/*利用后缀表达式计算值
1）遇到操作数，直接入栈；

2）遇到操作符，则将栈顶两个操作数取出计算，再将结果入栈；做减法与除法时特别要注意（第一个栈顶为第二操作数，第二栈顶才是第一操作数）

3）最后将栈中最后的一个元素返回即为所求值；
*/

double calculate(string PostfixExp) {
    arrayStack<double>* s = new arrayStack<double>(50);
    string temp;
    double num;
    int i = 0;
    double top;
    temp = "";
    while (PostfixExp[i] != '#') {
        //如果是数
        if (isNumber(PostfixExp[i]) || PostfixExp[i] == '.') {
            temp += PostfixExp[i];
        }
        //如果是空格，则数字结束（多位处理）
        else if (PostfixExp[i] == ' ' && !temp.empty() && !isOperator(PostfixExp[i])) {
            num = stringToNum<double>(temp);
            s->push(num);
            temp = "";
        }
        //如果是运算符
        else if (isOperator(PostfixExp[i])) {
            double result = 0, operand1, operand2;
            s->pop(operand1);
            s->pop(operand2);
            switch (PostfixExp[i]) {
            case '+':
                result = operand2 + operand1;
                break;
            case '-':
                result = operand2 - operand1;
                break;
            case '*':
                result = operand2 * operand1;
                break;
            case '/':
                result = operand2 / operand1;
                break;
            }
            s->push(result);
        }
        i++;
    }
    s->showTop(top);
    return top;
}
double Cal_result(string str)
{
    format(str);
    string temp;
    convert(str, temp);//求后缀表达式
    return calculate(temp);
}
//计算完毕，调用方法：Cal_result(string)
//**************************************************************************************************************************
//生成表达式
std::random_device rd;
float Widget::numcreate()//随机数生成
{
    //std::random_device rd;//真随机数，在MinGW内仍为伪随机
    //qDebug()<<rd()<<" ";
    return rd();//注意这里取随机数的方法和vs2019略有不同，引子需在外
}

void Widget::numcreate2(int shuliang, int nummax,int choose_decimal)//控制随机数大小（nummax代表多少以内的运算，shuliang代表运算数）
{
    int i;
    for (i = 0; i < shuliang; i++)
    {
        num[i] = numcreate();//每次都会重新取随机数
        //cout << "[" << num[i] << "]";
        for (;;)
        {
            if (num[i] > nummax || num[i] < -nummax)
            {
                num[i] = num[i] / 10;
            }
            else
            {
                break;
            }
        }
    }
    if(choose_decimal==0)
    for (int i = 0; i < shuliang; i++)
    {
        num[i] = (int)num[i];
    }
}

string Widget::Generate1(int shuliang, int nummax,int choose_decimal)//小数
{
    numcreate2(shuliang, nummax, choose_decimal);
    string Gen_result,temp;
    srand((unsigned)time(NULL));
    for (int i = 0; i < shuliang; i++)
    {
        if (i != shuliang - 1)
        {
            int tempNum = (rand() % (4 - 1 + 1)) + 1;
            if (tempNum == 1)
            {
                temp = to_string(num[i]) + "+";
                Gen_result += temp;
            }
            else if (tempNum == 2)
            {
                temp = to_string(num[i]) + "-";
                Gen_result += temp;
            }
            else if (tempNum == 3)
            {
                temp = to_string(num[i]) + "*";
                Gen_result += temp;
            }
            else if (tempNum == 4)
            {
                temp = to_string(num[i]) + "/";
                Gen_result += temp;
            }
        }
        else
        {
            temp = to_string(num[i]);
            Gen_result += temp;
        }
    }
    return Gen_result;
}

string Widget::Generate2(int shuliang, int nummax,int choose_decimal)//小数+负数
{
    numcreate2(shuliang, nummax, choose_decimal);
    srand((unsigned)time(NULL));
    for (int i = 0; i < shuliang; i++)
    {
        int temp = (rand() % (2 - 1 + 1)) + 1;
        if (temp == 1)
        {
            num[i] = 0 - num[i];
        }
    }
    string Gen_result, temp;
    for (int i = 0; i < shuliang; i++)
    {
        if (i != shuliang - 1)
        {
            int tempNum = (rand() % (4 - 1 + 1)) + 1;
            if (tempNum == 1)
            {
                if(num[i]<0)
                    temp = "("+to_string(num[i]) + ")+";//负数用(-1)类似的表示
                else
                temp = to_string(num[i]) + "+";
                Gen_result += temp;
            }
            else if (tempNum == 2)
            {
                if (num[i] < 0)
                    temp = "(" + to_string(num[i]) + ")-";
                else
                temp = to_string(num[i]) + "-";
                Gen_result += temp;
            }
            else if (tempNum == 3)
            {
                if (num[i] < 0)
                    temp = "(" + to_string(num[i]) + ")*";
                else
                temp = to_string(num[i]) + "*";
                Gen_result += temp;
            }
            else if (tempNum == 4)
            {
                if (num[i] < 0)
                    temp = "(" + to_string(num[i]) + ")/";
                else
                temp = to_string(num[i]) + "/";
                Gen_result += temp;
            }
        }
        else
        {
            if (num[i] < 0)
                temp = "(" + to_string(num[i]) + ")";
            else
            temp = to_string(num[i]);
            Gen_result += temp;
        }
    }
    return Gen_result;
}

string Widget::Generate3(int shuliang, int nummax,int choose_decimal)//小数+括号
{
    numcreate2(shuliang, nummax, choose_decimal);
    string Gen_result, temp;
    srand((unsigned)time(NULL));
    int count = 0;//用于统计未匹配的(
    for (int i = 0; i < shuliang; i++)
    {
        if (i != shuliang - 1)
        {
            int tempNum = (rand() % (4 - 1 + 1)) + 1;
            int judge1 = (rand() % (2 - 1 + 1)) + 1;//为1生成(
            int judge2 = (rand() % (2 - 1 + 1)) + 1;//为1生成)
            if (tempNum == 1)
            {
                if (judge1 == 1)//在数字左边才能加(
                {
                    temp = "(" + to_string(num[i]) + "+";
                    count++;
                }
                else if (count > 0 && judge2 == 1)//当前数字加了(就不再数字后加)
                {
                    temp = to_string(num[i]) + ")+";
                    count--;
                }
                else
                    temp = to_string(num[i]) + "+";
                Gen_result += temp;
            }
            else if (tempNum == 2)
            {
                if (judge1 == 1)
                {
                    temp = "(" + to_string(num[i]) + "-";
                    count++;
                }
                else if (count > 0 && judge2 == 1)
                {
                    temp = to_string(num[i]) + ")-";
                    count--;
                }
                else
                temp = to_string(num[i]) + "-";
                Gen_result += temp;
            }
            else if (tempNum == 3)
            {
                if (judge1 == 1)
                {
                    temp = "(" + to_string(num[i]) + "*";
                    count++;//增加(就会增加一个未匹配数
                }
                else if (count > 0 && judge2 == 1)
                {
                    temp = to_string(num[i]) + ")*";
                    count--;
                }
                else
                temp = to_string(num[i]) + "*";
                Gen_result += temp;
            }
            else if (tempNum == 4)
            {
                if (judge1 == 1)
                {
                    temp = "(" + to_string(num[i]) + "/";
                    count++;
                }
                else if (count > 0 && judge2 == 1)
                {
                    temp = to_string(num[i]) + ")/";
                    count--;
                }
                else
                temp = to_string(num[i]) + "/";
                Gen_result += temp;
            }
        }
        else
        {
            temp = to_string(num[i]);
            if (count > 0)
            {
                for (int j = 0; j < count; j++)
                    temp += ")";//最后看看还有多少(未匹配的直接加)
            }
            Gen_result += temp;
        }
    }
    return Gen_result;
}

string Widget::Generate4(int shuliang, int nummax,int choose_decimal)//小数+括号+负数
{
    numcreate2(shuliang, nummax, choose_decimal);
    string Gen_result, temp;
    srand((unsigned)time(NULL));
    for (int i = 0; i < shuliang; i++)//随机生成负数
    {
        int temp0 = (rand() % (2 - 1 + 1)) + 1;
        if (temp0 == 1)
        {
            num[i] = 0 - num[i];
        }
    }
    int count = 0;//用于统计未匹配的(
    for (int i = 0; i < shuliang; i++)
    {
        if (i != shuliang - 1)
        {
            int tempNum = (rand() % (4 - 1 + 1)) + 1;
            int judge1 = (rand() % (2 - 1 + 1)) + 1;//为1生成(
            int judge2 = (rand() % (2 - 1 + 1)) + 1;//为1生成)
            if (tempNum == 1)
            {
                if (judge1 == 1)//在数字左边才能加(
                {
                    if (num[i] < 0)
                        temp = "((" + to_string(num[i]) + ")+";
                    else
                        temp = "(" + to_string(num[i]) + "+";
                    count++;
                }
                else if (count > 0 && judge2 == 1)//当前数字加了(就不再数字后加)
                {
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + "))+";
                    else
                        temp = to_string(num[i]) + ")+";
                    count--;
                }
                else
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + ")+";
                    else
                        temp = to_string(num[i]) + "+";
                Gen_result += temp;
            }
            else if (tempNum == 2)
            {
                if (judge1 == 1)
                {
                    if (num[i] < 0)
                        temp = "((" + to_string(num[i]) + ")-";
                    else
                        temp = "(" + to_string(num[i]) + "-";
                    count++;
                }
                else if (count > 0 && judge2 == 1)
                {
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + "))-";
                    else
                        temp = to_string(num[i]) + ")-";
                    count--;
                }
                else
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + ")-";
                    else
                        temp = to_string(num[i]) + "-";
                Gen_result += temp;
            }
            else if (tempNum == 3)
            {
                if (judge1 == 1)
                {
                    if (num[i] < 0)
                        temp = "((" + to_string(num[i]) + ")*";
                    else
                        temp = "(" + to_string(num[i]) + "*";
                    count++;
                }
                else if (count > 0 && judge2 == 1)
                {
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + "))*";
                    else
                        temp = to_string(num[i]) + ")*";
                    count--;
                }
                else
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + ")*";
                    else
                        temp = to_string(num[i]) + "*";
                Gen_result += temp;
            }
            else if (tempNum == 4)
            {
                if (judge1 == 1)
                {
                    if (num[i] < 0)
                        temp = "((" + to_string(num[i]) + ")/";
                    else
                        temp = "(" + to_string(num[i]) + "/";
                    count++;
                }
                else if (count > 0 && judge2 == 1)
                {
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + "))/";
                    else
                        temp = to_string(num[i]) + ")/";
                    count--;
                }
                else
                    if (num[i] < 0)
                        temp = "(" + to_string(num[i]) + ")/";
                    else
                        temp = to_string(num[i]) + "/";
                Gen_result += temp;
            }
        }
        else
        {
            if (num[i] < 0)
                temp = "(" + to_string(num[i]) + ")";
            else
                temp = to_string(num[i]);
            if (count > 0)
            {
                for (int j = 0; j < count; j++)
                    temp += ")";//最后看看还有多少(未匹配的直接加)
            }
            Gen_result += temp;
        }
    }
    return Gen_result;
}

string Widget::Generate5(string str)//去掉小数
{
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '.')
        {
            str.erase(i,1);//注意擦除后位置发生变化
            int j;
            for (j = i ; j < str.size() && str[j] == 48; )
            {
                //cout << "<" << str << ">";
                str.erase(j,1);
            }
        }
    }
    return str;
}
//**************************************************************************************************************************
//界面

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
}

Widget::~Widget()
{
    delete ui;
}

/*
1.QString转换String

string s = qstr.toStdString();

2.String转换QString

QString qstr2 = QString::fromStdString(s);
*/

void Widget::on_Start_clicked()
{
    if(ui->stackedWidget->currentIndex()==0)
   {
       if(this->ui->horizontalSlider_nummax->value()==1)
    nummax=10;
    else if(this->ui->horizontalSlider_nummax->value()==2)
       nummax=100;
    else
        nummax=1000;
    shuliang=this->ui->spinBox_shuliang->value();
    choose_number=this->ui->spinBox_choose->value();
    fill_number=this->ui->spinBox_fill->value();
    judge_number=this->ui->spinBox_judge->value();
    Over=0;
    if(ui->checkBox_negative->isChecked()==false)
    {
        judge_negative=0;//Yes不生成
    }
    else if(ui->checkBox_negative->isChecked()==true)
    {
        judge_negative=1;//qDebug() << "(Button Cancle has been clicked.)";
    }
    if(ui->checkBox_decimal->isChecked()==false)
    {
        choose_decimal=0;//qDebug() << "(Button OK has been clicked.)";
    }
    else if(ui->checkBox_decimal->isChecked()==true)
    {
        choose_decimal=1;//qDebug() << "(Button Cancle has been clicked.)";
    }
    if(ui->checkBox_decimal->isChecked()==false)
    {
        judge_brackets=0;//qDebug() << "(Button OK has been clicked.)";
    }
    else if(ui->checkBox_decimal->isChecked()==true)
    {
        judge_brackets=1;//qDebug() << "(Button Cancle has been clicked.)";
    }
    //cout<<this->ui->buttonBox_negative->button( QDialogButtonBox::Yes)->judge_negative;
    //cout<<nummax<<" "<<shuliang<<" "<<choose_number<<" "<<fill_number<<" "<<judge_number;

        //qDebug()<<judge_negative<<" "<<judge_brackets<<" "<<choose_decimal;

    //第一页初始化一下第二页的第一题
    srand((unsigned) time(NULL));
    for(int i=0;i<choose_number;i++)
    {
        int right_answer=(rand() % (3 - 1 + 1)) + 1;
        All_choose_ans.push_back(right_answer);
    }
    for(int i=0;i<judge_number;i++)
    {
        int right_answer=(rand() % (2 - 1 + 1)) + 1;
        All_judge_ans.push_back(right_answer);
    }
    for(int i=0;i<choose_number+judge_number+fill_number;i++)//对用户作答结果进行初始化（所有答案将会默认赋值-0.0001）
    {
        All_user_ans.push_back(-0.0001);
    }
    for(int i=0;i<choose_number+judge_number+fill_number;i++)//对用户作答状态进行初始化（所有答案将会默认赋值0）
    {
        state.push_back(0);
    }
    for(int i=0;i<choose_number+judge_number+fill_number;i++)
    {
        if(judge_negative==0&&judge_brackets==0&&choose_decimal==0)//不选负数也不选括号也不选小数
        {
            Caculate_str=Generate5(Generate1(shuliang, nummax,choose_decimal));
            //cout<<Caculate_str;
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
        }
        if(judge_negative==1&&judge_brackets==0&&choose_decimal==0)//选负数不选括号也不选小数
        {
            Caculate_str=Generate5(Generate2(shuliang, nummax,choose_decimal));
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
        }
        if(judge_negative==0&&judge_brackets==1&&choose_decimal==0)//不选负数选括号也不选小数
        {
            Caculate_str=Generate5(Generate3(shuliang, nummax,choose_decimal));
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
        }
        if(judge_negative==0&&judge_brackets==0&&choose_decimal==1)//不选负数也不选括号选小数
        {
            //qDebug()<<"test";
            Caculate_str=Generate1(shuliang, nummax,choose_decimal);
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
            //cout<<Caculate_str<<" ";
        }
        if(judge_negative==1&&judge_brackets==1&&choose_decimal==0)//选负数选括号不选小数
        {
            Caculate_str=Generate5(Generate4(shuliang, nummax,choose_decimal));
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
        }
        if(judge_negative==1&&judge_brackets==0&&choose_decimal==1)//选负数不选括号选小数
        {
            Caculate_str=Generate2(shuliang, nummax,choose_decimal);
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
        }
        if(judge_negative==0&&judge_brackets==1&&choose_decimal==1)//不选负数选括号选小数
        {
            Caculate_str=Generate3(shuliang, nummax,choose_decimal);
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
        }
        if(judge_negative==1&&judge_brackets==1&&choose_decimal==1)//选负数选括号选小数
        {
            Caculate_str=Generate4(shuliang, nummax,choose_decimal);
            All_calculation.push_back(Caculate_str);
            result_num=Cal_result(Caculate_str);
            All_right_ans.push_back(result_num);
        }
    }
    //对下一页初始化
    current_position=0;//等到下一次再按next时已经到第二题了
    QString QT_Caculate_str=QString::fromStdString(All_calculation[0]);
    QString QT_tihao=QString::number(1);
    ui->label_1->setText("第"+QT_tihao+"题："+QT_Caculate_str+"=");

    //QString QT_right_answer=QString::number(right_answer);
    //设置答案
    QString QT_result_num=QString::number(All_right_ans[0]);
    QString QT_error_num1=QString::number(All_right_ans[0]+1);
    QString QT_error_num2=QString::number(All_right_ans[0]-1);

    if(All_choose_ans[0]==1)
    {
        ui->radioButton_11->setText("A."+QT_result_num);
        ui->radioButton_12->setText("B."+QT_error_num1);
        ui->radioButton_13->setText("C."+QT_error_num2);
    }
    else if(All_choose_ans[0]==2)
    {
        ui->radioButton_11->setText("A."+QT_error_num1);
        ui->radioButton_12->setText("B."+QT_result_num);
        ui->radioButton_13->setText("C."+QT_error_num2);
    }
    else if(All_choose_ans[0]==3)
    {
        ui->radioButton_11->setText("A."+QT_error_num1);
        ui->radioButton_12->setText("B."+QT_error_num2);
        ui->radioButton_13->setText("C."+QT_result_num);
    }

}
    if(ui->stackedWidget->currentIndex()!=0)
    {
        double score=0,total_score=choose_number+fill_number+judge_number;
        ui->stackedWidget->setCurrentIndex(4);//提交了开始打分切末页
        for(int i=0;i<choose_number+fill_number+judge_number;i++)
        {
            if(All_user_ans[i]==All_right_ans[i])
            {
                score++;
            }
        }
        QString QT_score=QString::number(score);
        QString QT_total=QString::number(total_score);
        QString QT_rate=QString::number(score/total_score);
        ui->Score->setText("一共"+QT_total+"题，你的得分是"+QT_score+"，得分率为"+QT_rate);
        ui->Start->setText("查看得分");
        //提交后进行文件独写
        std::ofstream output_file("D:\\Result.txt");
        std::ostream_iterator<std::string> output_iterator(output_file, "\n");
        vector<string>temp;
        for(int i=0;i<choose_number+fill_number+judge_number;i++)
        {
            string right_ans=to_string(All_right_ans[i]);
            string user_ans=to_string(All_user_ans[i]);
            if(All_user_ans[i]==All_right_ans[i])
            temp.push_back("你的题目："+All_calculation[i]+"=，正确答案："+right_ans+"，你的答案："+user_ans+"，回答正确");
            else
                temp.push_back("你的题目："+All_calculation[i]+"=，正确答案："+right_ans+"，你的答案："+user_ans+"，回答错误");

        }
        std::copy(temp.begin(), temp.end(), output_iterator);
    }
    if(ui->stackedWidget->currentIndex()==0)
    {
        ui->stackedWidget->setCurrentIndex(1);//切下一页
        ui->Start->setText("提交");
    }

}
void Widget::on_Next_clicked()
{
    //先记录上一次的答案
    if(ui->stackedWidget->currentIndex()==0)
        return;
    if(current_position<choose_number)//表示是在选择题内
    {
        ui->stackedWidget->setCurrentIndex(1);//保证页数对
        if(ui->radioButton_11->isChecked())
        {
            state[current_position]=1;//当前状态已作答
            if(All_choose_ans[current_position]==1)
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;
            }
        }
        else if(ui->radioButton_12->isChecked())
        {
            state[current_position]=1;//当前状态已作答
            if(All_choose_ans[current_position]==1)//代表用户选择第2但实际就是第1对
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;
            }
            else if(All_choose_ans[current_position]==2)//代表用户选择第2但实际就是第2对
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]-1;
            }
        }
        else if(ui->radioButton_13->isChecked())
        {
            state[current_position]=1;//当前状态已作答
            if(All_choose_ans[current_position]==3)
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]-1;
            }
        }
    }
    else if(current_position>=choose_number&&current_position<choose_number+fill_number)//表示在填空题内
    {
        ui->stackedWidget->setCurrentIndex(2);
        if(ui->lineEdit_1->text().isEmpty()==0)//如果空格不为空
        {
            state[current_position]=1;//当前状态已作答
            //设置答案
            QString QT_user_num=ui->lineEdit_1->text();
            //QString::number(All_right_ans[current_position]);
            All_user_ans[current_position]=QT_user_num.toDouble();
        }
    }
    else if(current_position>=choose_number+fill_number&&current_position<choose_number+judge_number+fill_number)//表示在判断题内
    {
        ui->stackedWidget->setCurrentIndex(3);//保证页数对
        if(ui->radioButton_31->isChecked())//如果用户选择了正确
        {
            //qDebug()<<"1";
            state[current_position]=1;//当前状态已作答
            if(All_judge_ans[current_position-choose_number-fill_number]==1)//答案也是正确
            {
                All_user_ans[current_position]=All_right_ans[current_position];//那用户回答正确
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;
            }
        }
        else if(ui->radioButton_32->isChecked())//如果用户选择了错误
        {
           // qDebug()<<"2";
            state[current_position]=1;//当前状态已作答
            if(All_judge_ans[current_position-choose_number-fill_number]==1)//答案却是正确
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;//那用户回答错误
            }
            else if(All_judge_ans[current_position-choose_number-fill_number]==2)
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
        }
    }


    //再准备下一题更新在界面
    if(current_position!=choose_number+judge_number+fill_number-1)//末尾终止
    current_position++;
    if(current_position<choose_number)//表示是在选择题内
        {
            ui->stackedWidget->setCurrentIndex(1);//保证页数对
            QString QT_Caculate_str=QString::fromStdString(All_calculation[current_position]);
            QString QT_tihao=QString::number(current_position+1);
            ui->label_1->setText("第"+QT_tihao+"题："+QT_Caculate_str+"=");

            //QString QT_right_answer=QString::number(right_answer);
            //设置答案
            QString QT_result_num=QString::number(All_right_ans[current_position]);
            QString QT_error_num1=QString::number(All_right_ans[current_position]+1);
            QString QT_error_num2=QString::number(All_right_ans[current_position]-1);

            if(All_choose_ans[current_position]==1)
            {
                ui->radioButton_11->setText("A."+QT_result_num);
                ui->radioButton_12->setText("B."+QT_error_num1);
                ui->radioButton_13->setText("C."+QT_error_num2);
            }
            else if(All_choose_ans[current_position]==2)
            {
                ui->radioButton_11->setText("A."+QT_error_num1);
                ui->radioButton_12->setText("B."+QT_result_num);
                ui->radioButton_13->setText("C."+QT_error_num2);
            }
            else if(All_choose_ans[current_position]==3)
            {
                ui->radioButton_11->setText("A."+QT_error_num1);
                ui->radioButton_12->setText("B."+QT_error_num2);
                ui->radioButton_13->setText("C."+QT_result_num);
            }
            if(state[current_position]==1)//下一题已作答，则让作答结果显示出来
            {
                if(All_choose_ans[current_position]==1)
                {
                    if(All_user_ans[current_position]==All_right_ans[current_position])
                    {
                        ui->radioButton_11->setChecked(true);
                    }
                    else if(All_user_ans[current_position]==All_right_ans[current_position]+1)//如果正确答案在位置1，但用户的答案比正确答案大1，那说明用户选择的B
                    {
                        ui->radioButton_12->setChecked(true);
                    }
                    else
                        ui->radioButton_13->setChecked(true);
                }
                else if(All_choose_ans[current_position]==2)
                {
                    if(All_user_ans[current_position]==All_right_ans[current_position])
                    {
                        ui->radioButton_12->setChecked(true);
                    }
                    else if(All_user_ans[current_position]==All_right_ans[current_position]+1)//如果正确答案在位置2，但用户的答案比正确答案大1，那说明用户选择的A
                    {
                        ui->radioButton_11->setChecked(true);
                    }
                    else
                        ui->radioButton_13->setChecked(true);
                }
                else if(All_choose_ans[current_position]==3)
                {
                    if(All_user_ans[current_position]==All_right_ans[current_position])
                    {
                        ui->radioButton_13->setChecked(true);
                    }
                    else if(All_user_ans[current_position]==All_right_ans[current_position]+1)//如果正确答案在位置3，但用户的答案比正确答案大1，那说明用户选择的A
                    {
                        ui->radioButton_11->setChecked(true);
                    }
                    else
                        ui->radioButton_12->setChecked(true);
                }
            }
            if(Over==1)
            {
                QString QT_Right_ans=QString::number(All_right_ans[current_position]);
                ui->Right_ans_1->setText("正确答案是："+QT_Right_ans);
            }
        }
        else if(current_position>=choose_number&&current_position<choose_number+fill_number)//表示在填空题内
        {
            //qDebug()<<current_position;
            ui->stackedWidget->setCurrentIndex(2);//保证页数对
            QString QT_Caculate_str=QString::fromStdString(All_calculation[current_position]);
            QString QT_tihao=QString::number(current_position+1);
            ui->label_2->setText("第"+QT_tihao+"题："+QT_Caculate_str+"=");
            if(state[current_position]==1)//如果前面已经作答了，要让结果显示出来
            {
                QString QT_temp=QString::number(All_user_ans[current_position]);
                ui->lineEdit_1->setText(QT_temp);
            }
            if(Over==1)
            {
                QString QT_Right_ans=QString::number(All_right_ans[current_position]);
                ui->Right_ans_2->setText("正确答案是："+QT_Right_ans);
            }
        }
        else if(current_position>=choose_number+fill_number&&current_position<choose_number+judge_number+fill_number)//表示在判断题内
        {
        ui->stackedWidget->setCurrentIndex(3);//保证页数对;
        QString QT_Caculate_str=QString::fromStdString(All_calculation[current_position]);
        QString QT_tihao=QString::number(current_position+1);
        QString QT_result_num=QString::number(All_right_ans[current_position]);
        QString QT_error_num1=QString::number(All_right_ans[current_position]+1);
        if(All_judge_ans[current_position-choose_number-fill_number]==1)//为1表示答案给了正确
        {
            ui->label_3->setText("第"+QT_tihao+"题："+QT_Caculate_str+"="+QT_result_num);
        }
        else
            ui->label_3->setText("第"+QT_tihao+"题："+QT_Caculate_str+"="+QT_error_num1);

        if(state[current_position]==1)//下一题已作答，则让作答结果显示出来
        {
            if(All_judge_ans[current_position-choose_number-fill_number]==1)//如果答案是正确的情况下
            {

                if(All_user_ans[current_position]==All_right_ans[current_position])//用户答案和标准答案一致，给第一个按钮标记
                {
                    ui->radioButton_31->setChecked(true);
                }
                else
                {
                    ui->radioButton_32->setChecked(true);
                }
            }
            else if(All_judge_ans[current_position-choose_number-fill_number]==2)//如果答案是错误的情况下
            {

                if(All_user_ans[current_position]==All_right_ans[current_position])//用户答案和标准答案一致，给第2个按钮标记
                {
                    ui->radioButton_32->setChecked(true);
                }
                else
                {
                    ui->radioButton_31->setChecked(true);
                }
            }
            }
        if(Over==1)
        {
            QString QT_Right_ans=QString::number(All_right_ans[current_position]);
            ui->Right_ans_3->setText("正确答案是："+QT_Right_ans);
        }
        }
}

void Widget::on_Previous_clicked()
{
    if(ui->stackedWidget->currentIndex()==0)
        return;
    //先记录下一题的答案
    if(current_position<choose_number)//表示是在选择题内
    {
        ui->stackedWidget->setCurrentIndex(1);//保证页数对
        if(ui->radioButton_11->isChecked())
        {
            state[current_position]=1;//当前状态已作答
            if(All_choose_ans[current_position]==1)
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;
            }
        }
        else if(ui->radioButton_12->isChecked())
        {
            state[current_position]=1;//当前状态已作答
            if(All_choose_ans[current_position]==1)//代表用户选择第2但实际就是第1对
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;
            }
            else if(All_choose_ans[current_position]==2)//代表用户选择第2但实际就是第2对
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]-1;
            }
        }
        else if(ui->radioButton_13->isChecked())
        {
            state[current_position]=1;//当前状态已作答
            if(All_choose_ans[current_position]==3)
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]-1;
            }
        }
    }
    else if(current_position>=choose_number&&current_position<choose_number+fill_number)//表示在填空题内
    {
        ui->stackedWidget->setCurrentIndex(2);
        if(ui->lineEdit_1->text().isEmpty()==0)//如果空格不为空
        {
            state[current_position]=1;//当前状态已作答
            //设置答案
            QString QT_user_num=ui->lineEdit_1->text();
            //QString::number(All_right_ans[current_position]);
            All_user_ans[current_position]=QT_user_num.toDouble();
        }
    }
    else if(current_position>=choose_number+fill_number&&current_position<choose_number+judge_number+fill_number)//表示在判断题内
    {
        ui->stackedWidget->setCurrentIndex(3);//保证页数对
        if(ui->radioButton_31->isChecked())//如果用户选择了正确
        {
            state[current_position]=1;//当前状态已作答
            if(All_judge_ans[current_position-choose_number-fill_number]==1)//答案也是正确
            {
                All_user_ans[current_position]=All_right_ans[current_position];//那用户回答正确
            }
            else
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;
            }
        }
        else if(ui->radioButton_32->isChecked())//如果用户选择了错误
        {
            state[current_position]=1;//当前状态已作答
            if(All_judge_ans[current_position-choose_number-fill_number]==1)//答案却是正确
            {
                All_user_ans[current_position]=All_right_ans[current_position]+1;//那用户回答错误
            }
            else if(All_judge_ans[current_position-choose_number-fill_number]==2)
            {
                All_user_ans[current_position]=All_right_ans[current_position];
            }
        }
    }


    //再准备上一题更新在界面
    if(current_position!=0)
    current_position--;
    if(current_position<choose_number)//表示是在选择题内
        {
            ui->stackedWidget->setCurrentIndex(1);//保证页数对
            QString QT_Caculate_str=QString::fromStdString(All_calculation[current_position]);
            QString QT_tihao=QString::number(current_position+1);
            ui->label_1->setText("第"+QT_tihao+"题："+QT_Caculate_str+"=");

            //QString QT_right_answer=QString::number(right_answer);
            //设置答案
            QString QT_result_num=QString::number(All_right_ans[current_position]);
            QString QT_error_num1=QString::number(All_right_ans[current_position]+1);
            QString QT_error_num2=QString::number(All_right_ans[current_position]-1);

            if(All_choose_ans[current_position]==1)
            {
                ui->radioButton_11->setText("A."+QT_result_num);
                ui->radioButton_12->setText("B."+QT_error_num1);
                ui->radioButton_13->setText("C."+QT_error_num2);
            }
            else if(All_choose_ans[current_position]==2)
            {
                ui->radioButton_11->setText("A."+QT_error_num1);
                ui->radioButton_12->setText("B."+QT_result_num);
                ui->radioButton_13->setText("C."+QT_error_num2);
            }
            else if(All_choose_ans[current_position]==3)
            {
                ui->radioButton_11->setText("A."+QT_error_num1);
                ui->radioButton_12->setText("B."+QT_error_num2);
                ui->radioButton_13->setText("C."+QT_result_num);
            }
            if(state[current_position]==1)//上一题已作答，则让作答结果显示出来
            {
                if(All_choose_ans[current_position]==1)
                {
                    if(All_user_ans[current_position]==All_right_ans[current_position])
                    {
                        ui->radioButton_11->setChecked(true);
                    }
                    else if(All_user_ans[current_position]==All_right_ans[current_position]+1)//如果正确答案在位置1，但用户的答案比正确答案大1，那说明用户选择的B
                    {
                        ui->radioButton_12->setChecked(true);
                    }
                    else
                        ui->radioButton_13->setChecked(true);
                }
                else if(All_choose_ans[current_position]==2)
                {
                    if(All_user_ans[current_position]==All_right_ans[current_position])
                    {
                        ui->radioButton_12->setChecked(true);
                    }
                    else if(All_user_ans[current_position]==All_right_ans[current_position]+1)//如果正确答案在位置2，但用户的答案比正确答案大1，那说明用户选择的A
                    {
                        ui->radioButton_11->setChecked(true);
                    }
                    else
                        ui->radioButton_13->setChecked(true);
                }
                else if(All_choose_ans[current_position]==3)
                {
                    if(All_user_ans[current_position]==All_right_ans[current_position])
                    {
                        ui->radioButton_13->setChecked(true);
                    }
                    else if(All_user_ans[current_position]==All_right_ans[current_position]+1)//如果正确答案在位置3，但用户的答案比正确答案大1，那说明用户选择的A
                    {
                        ui->radioButton_11->setChecked(true);
                    }
                    else
                        ui->radioButton_12->setChecked(true);
                }
            }
            if(Over==1)
            {
                QString QT_Right_ans=QString::number(All_right_ans[current_position]);
                ui->Right_ans_1->setText("正确答案是："+QT_Right_ans);
            }
        }
        else if(current_position>=choose_number&&current_position<choose_number+fill_number)//表示在填空题内
        {
        //qDebug()<<current_position;
        ui->stackedWidget->setCurrentIndex(2);//保证页数对
        QString QT_Caculate_str=QString::fromStdString(All_calculation[current_position]);
        QString QT_tihao=QString::number(current_position+1);
        ui->label_2->setText("第"+QT_tihao+"题："+QT_Caculate_str+"=");
        if(state[current_position]==1)//如果前面已经作答了，要让结果显示出来
        {
            QString QT_temp=QString::number(All_user_ans[current_position]);
            ui->lineEdit_1->setText(QT_temp);
        }
        if(Over==1)
        {
            QString QT_Right_ans=QString::number(All_right_ans[current_position]);
            ui->Right_ans_2->setText("正确答案是："+QT_Right_ans);
        }
        }
        else if(current_position>=choose_number+fill_number&&current_position<choose_number+judge_number+fill_number)//表示在判断题内
        {
        ui->stackedWidget->setCurrentIndex(3);//保证页数对;
        QString QT_Caculate_str=QString::fromStdString(All_calculation[current_position]);
        QString QT_tihao=QString::number(current_position+1);
        QString QT_result_num=QString::number(All_right_ans[current_position]);
        QString QT_error_num1=QString::number(All_right_ans[current_position]+1);
        if(All_judge_ans[current_position-choose_number-fill_number]==1)//为1表示答案给了正确
        {
            ui->label_3->setText("第"+QT_tihao+"题："+QT_Caculate_str+"="+QT_result_num);
        }
        else
            ui->label_3->setText("第"+QT_tihao+"题："+QT_Caculate_str+"="+QT_error_num1);

        if(state[current_position]==1)//下一题已作答，则让作答结果显示出来
        {
            if(All_judge_ans[current_position-choose_number-fill_number]==1)//如果答案是正确的情况下
            {
                if(All_user_ans[current_position]==All_right_ans[current_position])//用户答案和标准答案一致，给第一个按钮标记
                {
                    ui->radioButton_31->setChecked(true);
                }
                else
                {
                    ui->radioButton_32->setChecked(true);
                }
            }
            else if(All_judge_ans[current_position-choose_number-fill_number]==2)//如果答案是错误的情况下
            {
                if(All_user_ans[current_position]==All_right_ans[current_position])//用户答案和标准答案一致，给第2个按钮标记
                {
                    ui->radioButton_32->setChecked(true);
                }
                else
                {
                    ui->radioButton_31->setChecked(true);
                }
            }
           }
        if(Over==1)
        {
            QString QT_Right_ans=QString::number(All_right_ans[current_position]);
            ui->Right_ans_3->setText("正确答案是："+QT_Right_ans);
        }
       }
}
void Widget::on_Return_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    //每次回来做一次初始化
    //回到第一题会不显示答案，让他提前显示，只显示一次
    current_position=0;
    QString QT_ans1=QString::number(All_right_ans[current_position]);
    QString QT_Caculate_str=QString::fromStdString(All_calculation[current_position]);
    QString QT_tihao=QString::number(current_position+1);
    ui->label_1->setText("第"+QT_tihao+"题："+QT_Caculate_str+"=");

    //QString QT_right_answer=QString::number(right_answer);
    //设置答案
    QString QT_result_num=QString::number(All_right_ans[current_position]);
    QString QT_error_num1=QString::number(All_right_ans[current_position]+1);
    QString QT_error_num2=QString::number(All_right_ans[current_position]-1);

    if(All_choose_ans[current_position]==1)
    {
        ui->radioButton_11->setText("A."+QT_result_num);
        ui->radioButton_12->setText("B."+QT_error_num1);
        ui->radioButton_13->setText("C."+QT_error_num2);
        if(All_user_ans[current_position]==All_right_ans[current_position])
        {
            ui->radioButton_11->setChecked(true);
        }
        else if(All_user_ans[current_position]==All_right_ans[current_position]+1)
            ui->radioButton_12->setChecked(true);
        else
            ui->radioButton_13->setChecked(true);
    }
    else if(All_choose_ans[current_position]==2)
    {
        ui->radioButton_11->setText("A."+QT_error_num1);
        ui->radioButton_12->setText("B."+QT_result_num);
        ui->radioButton_13->setText("C."+QT_error_num2);
        if(All_user_ans[current_position]==All_right_ans[current_position])
        {
            ui->radioButton_12->setChecked(true);
        }
        else if(All_user_ans[current_position]==All_right_ans[current_position]+1)
            ui->radioButton_11->setChecked(true);
        else
            ui->radioButton_13->setChecked(true);
    }
    else if(All_choose_ans[current_position]==3)
    {
        ui->radioButton_11->setText("A."+QT_error_num1);
        ui->radioButton_12->setText("B."+QT_error_num2);
        ui->radioButton_13->setText("C."+QT_result_num);
        if(All_user_ans[current_position]==All_right_ans[current_position])
        {
            ui->radioButton_13->setChecked(true);
        }
        else if(All_user_ans[current_position]==All_right_ans[current_position]+1)
            ui->radioButton_11->setChecked(true);
        else
            ui->radioButton_12->setChecked(true);
    }
    ui->Right_ans_1->setText("正确答案是："+QT_ans1);
    Over=1;//判结束状态，显示答案
}

