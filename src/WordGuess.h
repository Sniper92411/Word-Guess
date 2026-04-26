#include <bits/stdc++.h>
#include <fstream>
#include <windows.h>
#include <algorithm>
#include <random>
#include "Header.h"
#include "Rank.h"
#include "WordGuessGame.h"
#include "Translator.h"
using namespace std;
WordGuessGame Game;
string input;
unordered_map<string, int> GameStatus;
InteractResult result;
void Init() { // 初始化函数 在这里完成初始化程序设置、转译、单词列表
	string temp;
	ifstream ifs;
	ifs.open("settings.txt"); // 初始化设置
	if(!ifs.is_open()) {
		throw WordGuessException("Failed to open input file. Cannot find \"settings.txt\"");
		exit(1);
	}
	while(!ifs.eof()) {
		getline(ifs, temp);
		if(temp[0] == '#') continue;
		for(int i = 0;i < temp.length();i++) {
			if(temp[i] == '=') {
				string key = temp.substr(0, i), value = temp.substr(i+1);
				if(value[0] >= '0' && value[0] <= '9')
					IntSettingList.set(key, atoi(value.c_str()));
				else
					StringSettingList.set(key, value);
				break;
			}
		}
	}
	ifs.close(); // 初始化设置结束
	string language = "unknown", id, message; // 初始化语言转译
	for(int i = 0;i < languages.size();i++) {
		ifs.open(languages[i] + ".txt");
		if(!ifs.is_open()) continue;
		while(!ifs.eof()) {
			getline(ifs, temp);
			temp = replaceEscapeChar(temp);
			if(temp[0] == '#') continue;
			for(int i = 0;i < temp.length();i++) {
				if(temp[i] == ';') { // 读取到这个文件的语言
					language = temp.substr(0, i);
					translator.add_language(language);
					break;
				}
				else if(temp[i] == '=') { // 读取到一条转译内容
					id = temp.substr(1, i-1);
					message = temp.substr(i+1);
					translator.add_message(language, id, message);
					break;
				}
				else if(temp[i] == '+') { // 读取到一条附加换行内容
					id = temp.substr(1, i-1);
					message = "";
					translator.add_message(language, id, message);
					break;
				}
			}
		}
		ifs.close(); // 初始化语言转译结束
	}
	ifs.open(StringSettingList.get("CustomInputFile")); // 初始化单词
	if(!ifs.is_open()) ifs.open("words.txt");
	if(!ifs.is_open()) {
		throw WordGuessException("Failed to open input file. Check if the words input file goes wrong.");
		exit(1);
	}
	while(!ifs.eof()) {
		getline(ifs, temp);
		if(temp[0] == '#') continue;
		string wd, mn;
		for(int i = 0;i < temp.length();i++) {
			if(temp[i] == ' ') {
				wd = temp.substr(0, i);
				mn = temp.substr(i+1);
				break;
			}
		}
		if(wd.length() <= 5 || wd.length() >= 16) continue; // 单个单词长度限制：6-15（应该在制作单词表时检查完毕）
		words.push_back(wd);
		meanings[wd] = mn;
	}
	if(!words.size()) {
		throw WordGuessException("Load game failed. No contents in words input file.");
		exit(1);
	}
	ifs.close(); // 初始化单词结束
}
int GameProcess(int StartHp = 10) { // 用于控制一个游戏进程 指定StartHp以自定义初始血量
	result = Game.Start(StartHp); // 开始一局游戏
	while(true) {
		TURN: // 一个回合开始的位置
		color("white", "black");
		CLS();
		int gs = Game.GameStage(); // 判断游戏阶段 决定是否结束游戏
		GameStatus = Game.GetGameStatus();
		if(gs == -2) return 0; // 未能成功开始游戏
		if(gs != 0) { // 满足结束条件
			result = Game.End();
			return 0; // 一个游戏进程结束
		}
		if(IntSettingList.get("Debug") == 1) Print(Game.DebugInfo()); // 打印调试信息
		// 输出每轮信息
		Print(translator.translate("WordGuessGame.Text.Hello"));
		Print(Runtime_Format(translator.translate("WordGuessGame.Text.Turn_HP"), GameStatus["hp"]));
		Print(Runtime_Format(translator.translate("WordGuessGame.Text.Turn_Guessed"), GameStatus["count"], GameStatus["alphalen"]));
		if(Game.GetGamemode() != "translation") {
			Print(translator.translate("WordGuessGame.Text.Turn_GuessedStatus"));
			Print(1, Game.GetGuessedStatus());
		}
		else {
			result = Game.ShowMeaning();
			Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.TranslationMeaning"), result.StringReturn["meaning"]));
		}
		Print(translator.translate("WordGuessGame.Text.Turn_Help"));
		Print(translator.translate("WordGuessGame.Text.Turn_WordStatus"));
		Print(2, Game.GetWordStatus());
		Print(translator.translate("WordGuessGame.Text.Turn_Remind"));
		getline(cin, input);
		CLS();
		// 处理输入
		if(input.length() == 0) goto TURN; // 无输入
		if(input.length() == 1) { // 猜字母 或 特殊命令
			char Char = input[0];
			if(Char == '/') { // 放弃
				string confirm;
				Print(translator.translate("WordGuessGame.Text.GiveUp"));
				if(Game.GetGamemode() == "hardcore") Print(translator.translate("WordGuessGame.DictionaryMode.Text.HardcoreGiveUp"));
				getline(cin, confirm);
				if(confirm.length() == 1 && confirm[0] == '/') {
					CLS();
					Game.GiveUp();
				}
				goto TURN;
			}
			else if(Char >= '0' && Char <= '9') { // 特殊命令
				string Gamemode = Game.GetGamemode();
				switch(Char) {
					case '0':
						result = Game.Hint();
						if(Game.GetGamemode() == "default" || Game.GetGamemode() == "rating")
							Print(Runtime_Format(translator.translate(result.id), result.StringReturn["meaning"]));
						else if(Game.GetGamemode() == "translation")
							Print(Runtime_Format(translator.translate(result.id), result.IntReturn["consume"]));
						break;
					case '1':
						if(Gamemode == "default")
							Print(translator.translate("WordGuessGame.Help.Basic"));
						else if(Gamemode == "rating")
							Print(translator.translate("WordGuessGame.Help.DictionaryMode.Gamemode.Rating"));
						else if(Gamemode == "translation")
							Print(translator.translate("WordGuessGame.Help.DictionaryMode.Gamemode.Translation"));
						else if(Gamemode == "hardcore")
							Print(translator.translate("WordGuessGame.Help.DictionaryMode.Gamemode.Hardcore"));
						break;
					case '2':
						Print(translator.translate("WordGuessGame.Help.Rating"));
						break;
					case '4':
						Print(translator.translate("WordGuessGame.Help.Interact"));
						break;
					default:
						Print(translator.translate("WordGuessGame.Input.Invaild"));
						break;
				}
			}
			else { // 猜字母
				result = Game.GuessAlpha(Char);
				switch(result.code) {
					case 1:
						if(IntSettingList.get("DictionaryMode") != 1) color("white", "green");
						Print(Runtime_Format(translator.translate(result.id), result.IntReturn["uppercount"], char(toupper(Char)), result.IntReturn["lowercount"], char(tolower(Char))));
						break;
					case 0:
						if(IntSettingList.get("DictionaryMode") != 1) color("white", "red");
						Print(translator.translate(result.id));
						break;
					case -1:
						Print(translator.translate(result.id));
						break;
				}
			}
		}
		else { // 猜单词
			result = Game.GuessWord(input);
			Print(translator.translate(result.id));
			if(IntSettingList.get("DictionaryMode") != 1) {
				if(result.code == 1) color("white", "green");
				else if(result.code == 0) color("white", "red");
			}
		}
		// 交互结束 输出目前状态信息
		TURN_END:
		GameStatus = Game.GetGameStatus();
		Print(Runtime_Format(translator.translate("WordGuessGame.Text.InteractResult"), GameStatus["alphalen"] - GameStatus["count"]));
		PAUSE();
		cin.sync();
	}
}
int DefaultHandler() { // 默认模式的处理器
	START: // 开始一局新游戏的位置标签
	int GameResult = GameProcess();
	// 游戏结束后结算 展示评级
	Print(translator.translate(result.id));
	if(IntSettingList.get("Flash") == 1) {
		switch(result.code) {
			case 1:
				color("white", "green", 1000);
				break;
			case 0:
				color("white", "red", 1000);
				break;
		}
	}
	Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Word"), result.StringReturn["word"]));
	Wait(250);
	Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Meaning"), result.StringReturn["meaning"]));
	Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_HP"), GameStatus["hp"]));
	Wait(250);
	Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Guessed"), (result.code ? GameStatus["alphalen"] : GameStatus["count"]), GameStatus["alphalen"], ((result.code ? GameStatus["alphalen"] : GameStatus["count"]) * 100.0) / GameStatus["alphalen"]));
	Wait(250);
	Print(translator.translate("WordGuessGame.Text.End_Rating", false));
	for(int i = 1;i <= 3;i++) {
		Print('.');
		Wait(250);
	}
	Print('\n');
	if(GameStatus["hp"] >= 10) {
		color("black", "pale_blue");
		Print(Ex());
	}
	else if(GameStatus["hp"] >= 7) {
		Print(A());
	}
	else if(GameStatus["hp"] >= 4) {
		Print(B());
	}
	else if(GameStatus["hp"] >= 1) {
		Print(C());
	}
	else if(GameStatus["hp"] <= 0) {
		color("black", "pale_red");
		Print(F());
	}
	if(result.IntReturn["remain"] >= 0) {
		Print(translator.translate("WordGuessGame.Text.Continue"));
		return 0;
	}
	else {
		Print(translator.translate("WordGuessGame.Text.Finish"));
		return 1;
	}
}
int DictionaryModeHandler() { // 启用词典模式时 调用这个处理器
	int Turns = 0, remainhp = 0, totalhp = 0, complete_count = 0; // Turns: 轮次 remainhp: 当前剩余血量 totalhp: 血量上限 complete_count: 成功次数
	Print(translator.translate("WordGuessGame.DictionaryMode.Text.Load"));
	PAUSE();
	CLS();
	Print(translator.translate("WordGuessGame.DictionaryMode.Text.ModeChoose"));
	getline(cin, input); // 输入模式
	for(int i = 0;i < input.length();i++) input[i] = tolower(input[i]);
	if(input == "1" || input == "rating") Game.SetGamemode("rating");
	else if(input == "2" || input == "translation") Game.SetGamemode("translation");
	else if(input == "3" || input == "hardcore") Game.SetGamemode("hardcore");
	else Game.SetGamemode("rating");
	Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.Turns"), words.size()));
	getline(cin, input); // 输入单词轮数
	for(char c : input) {
		Turns = Turns * 10 + (c - '0');
	}
	if(Turns <= 1 || Turns > words.size()) Turns = words.size();
	cin.clear();
	cin.sync(); // 防止后续getline时 因遗留的换行符而异常结束输入
	CLS();
	Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.StartConfirm:1"), Game.GetGamemode()));
	Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.StartConfirm:2"), Turns));
	Print(translator.translate("WordGuessGame.DictionaryMode.Text.StartConfirm:3"));
	PAUSE();
	if(Game.GetGamemode() == "rating") {
		for(int i = Turns - 1;i >= 0;i--) { // 按Turns次数循环
			color();
			totalhp += 10;
			int GameResult = GameProcess(); // 单个游戏结束时 GameStatus会被修改
			remainhp += GameStatus["hp"];
			if(result.code == 1) {
				complete_count++;
				Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.TurnComplete"), i));
				color("white", "green", 1000);
			}
			else {
				Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.TurnFail"), i));
				color("white", "red", 1000);
			}
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Word"), result.StringReturn["word"]));
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Meaning"), result.StringReturn["meaning"]));
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_HP"), GameStatus["hp"]));
			Print(1, Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.CurrentHp"), remainhp, totalhp));
			PAUSE();
		}
		double percentage = remainhp * 1.0 / totalhp; // 剩余生命值百分比
		CLS(), color();
		Print(translator.translate("WordGuessGame.DictionaryMode.Text.GameFinish"));
		Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.End_HP"), remainhp, totalhp));
		Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.End_HPPercentage"), percentage * 100));
		Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.End_CompleteCount"), complete_count, Turns));
		Print(translator.translate("WordGuessGame.Text.End_Rating"));
		Wait(1500);
		if(percentage >= 0.875) {
			color("black", "pale_blue");
			Print(Ex());
			Print(translator.translate("WordGuessGame.Rating.Excellent"));
		}
		else if(percentage >= 0.75) {
			Print(A());
			Print(translator.translate("WordGuessGame.Rating.A"));
		}
		else if(percentage >= 0.6) {
			Print(B());
			Print(translator.translate("WordGuessGame.Rating.B"));
		}
		else if(percentage >= 0.3) {
			Print(C());
			Print(translator.translate("WordGuessGame.Rating.C"));
		}
		else if(percentage < 0.3) {
			color("black", "pale_red");
			Print(F());
			Print(translator.translate("WordGuessGame.Rating.Fail"));
		}
	}
	else if(Game.GetGamemode() == "translation") {
		
		while(Turns--) {
			color();
			totalhp += 10;
			int GameResult = GameProcess(); // 单个游戏结束时 GameStatus会被修改
			remainhp += GameStatus["hp"];
			if(result.code == 1) {
				complete_count++;
				Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.TurnComplete"), Turns));
				color("white", "green", 1000);
			}
			else {
				Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.TurnFail"), Turns));
				color("white", "red", 1000);
			}
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Word"), result.StringReturn["word"]));
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Meaning"), result.StringReturn["meaning"]));
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_HP"), GameStatus["hp"]));
			Print(1, Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.CurrentHp"), remainhp, totalhp));
			PAUSE();
		}
	}
	else if(Game.GetGamemode() == "hardcore") {
		bool revived = false;
		remainhp = totalhp = 15;
		for(int i = Turns - 1;i >= 0;i--) {
			color();
			int GameResult = GameProcess(remainhp); // 单个游戏结束时 GameStatus会被修改
			remainhp = GameStatus["hp"];
			if(result.code == 1) {
				complete_count++, remainhp += 2;
				remainhp = (remainhp > totalhp) ? totalhp : remainhp; // 极限模式生命值上限
				Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.TurnComplete"), i));
				color("white", "green", 1000);
			}
			else { // 极限模式生命值耗尽 进入复活流程或结束流程
				if(!revived) {
					revived = true, remainhp = 5;
					Print(translator.translate("WordGuessGame.DictionaryMode.Text.HardcoreRevive"));
					color("black", "pale_yellow", 1000);

				}
				else {
					Print(translator.translate("WordGuessGame.DictionaryMode.Text.HardcoreAbort"));
					color("white", "red", 1000);
					break;
				}
			}
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Word"), result.StringReturn["word"]));
			Print(Runtime_Format(translator.translate("WordGuessGame.Text.End_Meaning"), result.StringReturn["meaning"]));
			Print(1, Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.CurrentHp"), remainhp, totalhp));
			PAUSE();
		}
		CLS(), color();
		if(remainhp != 0) Print(translator.translate("WordGuessGame.DictionaryMode.Text.GameFinish"));
		else Print(translator.translate("WordGuessGame.DictionaryMode.Text.HardcoreAbort"));
		Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.End_HP"), remainhp, totalhp));
		Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.End_CompleteCount"), complete_count, Turns));
		Print(Runtime_Format(translator.translate("WordGuessGame.DictionaryMode.Text.End_CompletePercentage"), complete_count * 100.0 / Turns));
		Print(translator.translate("WordGuessGame.Text.End_Rating"));
		Wait(1500);
		if(complete_count == Turns) {
			color("black", "pale_blue");
			Print(Ex());
			Print(translator.translate("WordGuessGame.Rating.Excellent"));
		}
		else {
			color("black", "pale_red");
			Print(F());
			Print(translator.translate("WordGuessGame.Rating.Fail"));
		}
	}
	// 游戏结束 退出进程
	Print(translator.translate("WordGuessGame.Text.Finish"));
	PAUSE();
	return 0;
}
int Main(vector<string>& args) { // 程序主函数 程序入口
	INIT: // 程序初始化标签
	Init(); // 执行初始化
	translator.select(StringSettingList.get("Language"));
	MAIN_START: // 程序完成初始化标签
	CLS();
	color();
	// 打乱单词
	random_device rd;
	default_random_engine rng(rd());
	shuffle(words.begin(), words.end(), rng);
	// 根据设定的游戏模式 调用相应的函数 负责用户交互模块
	if(IntSettingList.get("DictionaryMode") != 1) { // 默认模式
		CLS();
		color();
		Print(translator.translate("WordGuessGame.Text.Hello"));
		Print(translator.translate("WordGuessGame.Text.Load"));
		PAUSE();
		CLS();
		while(true) {
			LOAD_DEFAULT:
			int flag = DefaultHandler(); // 若用户未关闭窗口结束程序 这个处理器会被反复调用 实现重复游戏的效果（直至进程结束或单词表猜完）
			PAUSE();
			if(flag == 1) break;
		}
	}
	else {
		DictionaryModeHandler(); // 由于词典模式的特殊性 这个处理器仅调用一次 随后程序结束
	}
	return 0;
}
