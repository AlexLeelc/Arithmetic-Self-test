#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QRadioButton>
#include <iostream>
#include <string>
#include <vector>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:


    //生成表达式
    float numcreate();//随机数生成
    void numcreate2(int shuliang, int nummax,int choose_decimal);//控制随机数大小（nummax代表多少以内的运算，shuliang代表运算数）
    std::string Generate1(int shuliang, int nummax,int choose_decimal);//小数（可选）
    std::string Generate2(int shuliang, int nummax,int choose_decimal);//小数（可选）+负数
    std::string Generate3(int shuliang, int nummax,int choose_decimal);//小数（可选）+括号
    std::string Generate4(int shuliang, int nummax,int choose_decimal);//小数（可选）+负数+括号
    std::string Generate5(std::string str);//去掉字符串里的小数






private slots:
    void on_Start_clicked();
    void on_Next_clicked();
    void on_Previous_clicked();
    void on_Return_clicked();

private:
    Ui::Widget *ui;
    float num[1000];
    int shuliang;//运算数
    int nummax;//10\100\1000
    int choose_decimal;//是否选择小数
    int judge_negative;//是否选择负数
    int judge_brackets;//是否选择括号

    std::string Caculate_str;
    int choose_number;//选择题数量
    int judge_number;//判断题数量
    int fill_number;//填空题数量
    double result_num;//计算结果
    int Over;//判结束状态

    std::vector<std::string>All_calculation;//存放所有表达式
    std::vector<double>All_right_ans;//所有正确答案
    std::vector<double>All_user_ans;//所有用户答案
    std::vector<int>All_choose_ans;//所有选择的位置
    std::vector<int>All_judge_ans;//所有判断的位置(12)
    int current_position;//设置为当前题目所在位置
    std::vector<int>state;//设置作答状态(01)
};

#endif // WIDGET_H
