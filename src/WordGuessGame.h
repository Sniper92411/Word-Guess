#ifndef _WORDGUESSGAME_H
#define _WORDGUESSGAME_H


#include <iostream>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include "SettingList.h"
#include "Header.h"
using namespace std;
struct InteractResult { // 单次用户交互的返回结构
    string type, id; // type: 交互类型 id: 交互结果 可能可以直接转译
    int code; // 结果代码
    string GetWord_word; // 预留的返回 目前作用未知
    unordered_map<string, int> IntReturn; // 整数返回键值对
    unordered_map<string, string> StringReturn; // 一般返回键值对
    InteractResult(){}
    InteractResult(string type) {
        this->type = type;
    }
    InteractResult(string type, int code, string id) {
        this->type = type, this->code = code, this->id = id;
    }
};
class WordGuessGame {
    private:
        int status, hp, len, alphalen, count = 0, completion = 0; // 声明内部变量 游戏状态、血量、单词总长、字母占长度、猜出计数器、已猜过的词数
        string word, gamemode; // word: 谜底单词 gamemode: 游戏模式（词典模式下，rating为胜率模式，translation为翻译模式，hardcore为极限模式）
        bool show[20]; // show[i]: 位置i的字符是否显示
        unordered_map<char, bool> guessed; // 已猜过的字母
        int guessupdate(char t) { // 对猜测更新，返回匹配的字符个数
            int ret = 0;
            for(int i = 0;i < word.length();i++) {
                if(word.at(i) == t) {
                    ret++;
                    show[i] = true;
                }
            }
            return ret;
        }
    public:
        WordGuessGame() {
            this->gamemode = "default";
        }
        WordGuessGame(string gamemode) {
            if(gamemode == "rating" || gamemode == "translation" || gamemode == "hardcore") this->gamemode = gamemode;
            else this->gamemode = "default";
        }
        void SetGamemode(string gamemode) {
            if(gamemode == "rating" || gamemode == "translation" || gamemode == "hardcore") this->gamemode = gamemode;
        }
        string GetGamemode() {
            return this->gamemode;
        }
        void GiveUp() { // 放弃
            hp = 0;
        }
        int GameStage() { // 仅获取游戏进行到的阶段 用于判断游戏是否应继续或结束
            if(!status) return -2;
            if(count == alphalen) {
                return 1; // 游戏结束 玩家胜利
            }
            else if(hp <= 0) {
                hp = 0;
                return -1; // 游戏结束 玩家失败
            }
            return 0; // 游戏继续（不满足结束条件）
        }
        InteractResult Start(int StartHp = 10) { // 开始游戏
            if(status != 0) return InteractResult("Start", 0, "WordGuessGame.Start.Gameisrunning");
            status = 1; // 设置游戏状态为 进行中
            count = 0, hp = StartHp; // 初始化计数器和血量
            fill(show, show + 20, 0);
            if(this->gamemode == "translation") show[0] = 1, count = 1; // 词典 翻译模式 展示首字母
            for(char c = 'a';c <= 'z';c++) {
                guessed[c] = false;
                guessed[char(toupper(c))] = false;
            }
            word = words.at(this->completion);
            completion++; // 选择了一个单词 计数器+1
            len = alphalen = word.length();
            for(int i = 0;i < len;i++) {
                if(!isalpha(word[i])) {
                    show[i] = true;
                    alphalen--;
                }
            }
            return InteractResult("Start", 1, "WordGuessGame.Start.Success");
        }
        InteractResult End() { // 结束游戏
            if(hp != 0 && count != alphalen) return InteractResult("End", -1, "WordGuessGame.End.Invaild");
            status = 0; // 重置状态为 结束
            InteractResult Result("End");
            Result.IntReturn["remain"] = words.size() - completion;
            Result.StringReturn["word"] = word, Result.StringReturn["meaning"] = meanings[word];
            if(hp <= 0) Result.code = 0, Result.id = "WordGuessGame.End.Lose";
            if(count == alphalen) Result.code = 1, Result.id = "WordGuessGame.End.Win";
            return Result;
        }
        InteractResult GuessAlpha(char c) { // 交互操作：用户猜测一个字母
            if(this->gamemode == "translation") return InteractResult("GuessAlpha", -1, "WordGuessGame.DictionaryMode.GuessBanned");
            if(!isalpha(c)) return InteractResult("GuessAlpha", -1, "WordGuessGame.GuessAlpha.Invaild");
            if(guessed[c]) return InteractResult("GuessAlpha", -1, "WordGuessGame.GuessAlpha.Guessed");
            guessed[toupper(c)] = guessed[tolower(c)] = true;
            int uppercount = guessupdate(toupper(c));
            int lowercount = guessupdate(tolower(c));
            if(uppercount + lowercount == 0) {
                hp--;
                return InteractResult("GuessAlpha", 0, "WordGuessGame.GuessAlpha.Fail");
            }
            count += uppercount + lowercount;
            InteractResult Result("GuessAlpha", 1, "WordGuessGame.GuessAlpha.Success");
            Result.IntReturn["uppercount"] = uppercount, Result.IntReturn["lowercount"] = lowercount;
            return Result;
        }
        InteractResult GuessWord(string s) { // 交互操作：用户猜测整个单词
            InteractResult Result;
            for(char c : s) {
				if(c != ' ' && c != '-' && c != '\'' && !isalpha(c)) return InteractResult("GuessWord", -1, "WordGuessGame.GuessWord.Invaild");
			}
            if(s.length() != word.length()) return InteractResult("GuessWord", -1, "WordGuessGame.GuessWord.Invaild");
            if(s == word) {
                count = alphalen;
                return InteractResult("GuessWord", 1, "WordGuessGame.GuessWord.Success");
            }
            else {
                hp -= 2;
                return InteractResult("GuessWord", 0, "WordGuessGame.GuessWord.Fail");
            }
        }
        InteractResult ShowMeaning() {
            InteractResult Result("ShowMeaning", 1, "WordGuessGame.ShowMeaning.OK");
            Result.StringReturn["meaning"] = meanings[word];
            return Result;
        }
        InteractResult Hint() { // 交互操作：提示
            if(gamemode == "default" || gamemode == "rating") return ShowMeaning();
            else if(gamemode == "translation") { // 翻译模式提示：消耗 2 点生命值，翻开第一个未知字母
                InteractResult result("Hint", 1, "WordGuessGame.DictionaryMode.TranslationHint.Success");
                int consume = 2, i = 1;
                for(i;i < len;i++) {
                    if(!show[i] || consume == 4) {
                        break;
                    }
                    else {
                        consume++;
                    }
                }
                if(hp - consume > 0) {
                    show[i] = true, count++, hp -= consume, result.IntReturn["consume"] = consume;
                }
                else {
                    result.code = 0, result.id = "WordGuessGame.DictionaryMode.TranslationHint.Insufficient";
                }
                return result;
            }
            return InteractResult("Hint", -1, "");
        }
        string GetWordStatus() {
            string Result;
            for(int i = 0;i < len;i++) {
                if(show[i]) Result += word[i];
                else Result += '_';
            }
            return Result;
        }
        unordered_map<string, int> GetGameStatus() {
            unordered_map<string, int> Result;
            Result["hp"] = hp, Result["count"] = count, Result["len"] = len, Result["alphalen"] = alphalen;
            return Result;
        }
        string GetGuessedStatus() {
            string Result;
            for(char c = 'a';c <= 'z';c++) {
                if(guessed[c]) {
                    Result.push_back(c);
                    Result.push_back(' ');
                }
            }
            return Result;
        }
        string DebugInfo() {
            if(IntSettingList.get("Debug") == 0) return "";
            string Result;
            Result += "------Debug Info------\n";
            Result += format("Word: {}\n", word);
            Result += format("Word length and alpha count: {}, {}\n", len, alphalen);
            Result += format("HP: {}\n", hp);
            Result += format("Guessed: {}, {}\n", count, alphalen);
            Result += "Characters show or not:\n";
            for(int i = 0;i < len;i++) Result += (show[i] ? "1 " : "0 ");
            Result += "\nGuessed letters:\n";
            for(char c = 'a';c <= 'z';c++) if(guessed[c]) {
                Result.push_back(c);
                Result.push_back(' ');
            }
            Result += "\n------Debug Info------\n";
            return Result;
        }
};


#endif
