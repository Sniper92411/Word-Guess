#ifndef _HEADER_H_
#define _HEADER_H_


#include <windows.h>
#include "SettingList.h"
#include "Translator.h"
using namespace std;
// #define Runtime_Format(x, ...) format(runtime_format(x), __VA_ARGS__)
Translator translator;
SettingList<int> IntSettingList;
SettingList<string> StringSettingList;
vector<string> words, languages = {"zh_cn", "en_us"}; // 全局读取 words: 所有单词 languages: 可用的语言：中文、英文;
unordered_map<string, string> meanings;
constexpr void CLS() {
	system("cls");
}
constexpr void PAUSE() {
	system("pause");
}
void Wait(int milisecond, bool forced = false) { // 等待若干毫秒
	// if(!forced && IntSettingList.get("Debug") == 1) return;
	Sleep(milisecond);
}
template<typename... Strings>
void Print(const Strings&... contents) { // 打印 contents 中的（多个）内容 没有换行
	(std::cout << ... << contents);
}
template<typename... Strings>
void Print(int newLines, const Strings&... contents) { // 打印 contents 中的（多个）内容 并附加若干换行
	(std::cout << ... << contents);
	while(newLines--) std::cout << endl;
}
template<typename... Elements>
string Runtime_Format(const string id, const Elements&... elements) {
	return std::format(runtime_format(id), elements...);
}
void color() { // 切换颜色 未提供任何参数：重置颜色为启动时的颜色
	system("color 07");
}
void color(string text, string background, bool forced = false) { // 切换颜色 指定顺序为文本，背景
	if(!forced && IntSettingList.get("Flash") != 1) return;
	// 背景
	string cmd = "color ";
	if(background == "black") cmd += '0';
	else if(background == "blue") cmd += '1';
	else if(background == "green") cmd += '2';
	else if(background == "light_green") cmd += '3';
	else if(background == "red") cmd += '4';
	else if(background == "purple") cmd += '5';
	else if(background == "yellow") cmd += '6';
	else if(background == "white") cmd += '7';
	else if(background == "grey") cmd += '8';
	else if(background == "pale_blue") cmd += '9';
	else if(background == "pale_green") cmd += 'A';
	else if(background == "pale_light_green") cmd += 'B';
	else if(background == "pale_red") cmd += 'C';
	else if(background == "pale_purple") cmd += 'D';
	else if(background == "pale_yellow") cmd += 'E';
	else if(background == "bright_white") cmd += 'F';
	// 文字
	if(text == "black") cmd += '0';
	else if(text == "blue") cmd += '1';
	else if(text == "green") cmd += '2';
	else if(text == "light_green") cmd += '3';
	else if(text == "red") cmd += '4';
	else if(text == "purple") cmd += '5';
	else if(text == "yellow") cmd += '6';
	else if(text == "white") cmd += '7';
	else if(text == "grey") cmd += '8';
	else if(text == "pale_blue") cmd += '9';
	else if(text == "pale_green") cmd += 'A';
	else if(text == "pale_light_green") cmd += 'B';
	else if(text == "pale_red") cmd += 'C';
	else if(text == "pale_purple") cmd += 'D';
	else if(text == "pale_yellow") cmd += 'E';
	else if(text == "bright_white") cmd += 'F';
	system(cmd.c_str());
}
void color(string text, string background, int milisecond, bool forced = false) { // 切换颜色 指定顺序为文本，背景 持续若干毫秒
	if(!forced && IntSettingList.get("Flash") != 1) return;
	// 背景
	string cmd = "color ";
	if(background == "black") cmd += '0';
	else if(background == "blue") cmd += '1';
	else if(background == "green") cmd += '2';
	else if(background == "light_green") cmd += '3';
	else if(background == "red") cmd += '4';
	else if(background == "purple") cmd += '5';
	else if(background == "yellow") cmd += '6';
	else if(background == "white") cmd += '7';
	else if(background == "grey") cmd += '8';
	else if(background == "pale_blue") cmd += '9';
	else if(background == "pale_green") cmd += 'A';
	else if(background == "pale_light_green") cmd += 'B';
	else if(background == "pale_red") cmd += 'C';
	else if(background == "pale_purple") cmd += 'D';
	else if(background == "pale_yellow") cmd += 'E';
	else if(background == "bright_white") cmd += 'F';
	// 文字
	if(text == "black") cmd += '0';
	else if(text == "blue") cmd += '1';
	else if(text == "green") cmd += '2';
	else if(text == "light_green") cmd += '3';
	else if(text == "red") cmd += '4';
	else if(text == "purple") cmd += '5';
	else if(text == "yellow") cmd += '6';
	else if(text == "white") cmd += '7';
	else if(text == "grey") cmd += '8';
	else if(text == "pale_blue") cmd += '9';
	else if(text == "pale_green") cmd += 'A';
	else if(text == "pale_light_green") cmd += 'B';
	else if(text == "pale_red") cmd += 'C';
	else if(text == "pale_purple") cmd += 'D';
	else if(text == "pale_yellow") cmd += 'E';
	else if(text == "bright_white") cmd += 'F';
	system(cmd.c_str());
	Wait(milisecond);
	system("color 07");
}
void UserError(string id, int mtime = 2000) { // 用户错误
	CLS;
	Print(translator.translate("WordGuessGame.UserError"), translator.translate(id));
	Print(translator.translate(id));
	Wait(mtime);
}
string replaceEscapeChar(string str) { // 将读入被重复转义的字符重新转义生效
    int pos = 0;
    while ((pos = str.find("\\n", pos)) != std::string::npos) {
        str.replace(pos, 2, "\n");
        pos += 1;
    }
	return str;
}
// void showhelp(int type) { // 由于引入了新的帮助输出方式 这个函数已弃用 将在一个稳定版本后删除
// 	switch(type) {
// 		case 1:
// 			system("cls");
// 			cout << "欢迎游玩猜词游戏!" << endl;
// 			cout << "游戏基本规则: 程序会给你一个单词的长度，通过猜这个单词里可能有的字母，最终猜出单词。" << endl;
// 			cout << "猜测机制: 输入单个字母后，系统会检查单词中所有的这个字母的大小写，如果均没有，则错误，反之则正确。" << endl;
// 			cout << "          简而言之，输入了一个字母就能猜测对应的所有大写和小写字母，且系统将记录猜过的字母，无需重复猜测。" << endl;
// 			cout << "生命值: 初始生命值为10点，每猜错一个字母-1，猜词失败-2，归零时游戏失败。";
// 			cout << "胜负判断: 猜出单词即为胜利，生命值归零即为失败。\n" << endl;
// 			cout << "进阶技巧: 如果你在猜测过程中通过已知字母知道了具体的单词，直接输入整个单词并确认，系统将判断正误。但失败也将扣除2点生命值。\n" << endl;
// 			cout << "提示: 当且仅当生命值低于6，未知字母有3个及以上时，提示将解锁。提示仅可使用一次。" << endl;
// 			cout << "提示效果: 输入0来使用提示，提示将显示单词中的所有与 第一个未知字母 相同的大小写字母。\n" << endl;
// 			cout << "等阶与评级系统: 单局游戏结束后，系统将根据剩余生命值和猜出字母个数给出等阶和评级。" << endl;
// 			cout << "查看关于等阶/评级的进一步信息，请返回并输入2。\n" << endl;
// 			cout << "游戏运行机制: 单局游戏结束后，按任意键直接开始下一局，退出游戏请直接关闭应用。" << endl;
// 			PAUSE;
// 			break;
// 		case 2:
// 			system("cls");
// 			cout << "剩余生命值  评级" << endl;
// 			cout << "10--9       Excellent" << endl;
// 			cout << "8--6        A" << endl;
// 			cout << "5--4        B" << endl;
// 			cout << "3--1        C" << endl;
// 			cout << "0           Fail" << endl;
// 			cout << "其他        Unknown" << endl;
// 			PAUSE;
// 			break;
// 		case 3:
// 			system("cls");
// 			cout << "How to open commend mode: enter \"-\"" << endl;
// 			cout << "Argument format: <Name:Type>" << endl;
// 			cout << "Mandatory / Optional arguments: <Mandatory>, [Optional]" << endl;
// 			cout << "Command usage:" << endl;
// 			cout << "-var <key:string> <value> : Assign variable <key:string> of <value>. Assignable variables: hp, hintUsed." << endl;
// 			PAUSE;
// 			break;
// 	}
// }


#endif
