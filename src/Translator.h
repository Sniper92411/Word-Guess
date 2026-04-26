#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_


#include <iostream>
#include <windows.h>
#include <unordered_map>
using namespace std;
class Translator {
    private:
        string language;
        unordered_map<string, bool> languages;
        unordered_map<string, unordered_map<string, string> > translations;
    public:
        Translator() {
            this->language = "zh_cn";
        }
        Translator(string language) {
            if(languages[language]) {
                this->language = language;
            }
            else this->language = "zh_cn";
        }
        void add_language(string language) {
            if(languages.count(language) && languages[language]) return;
            languages[language] = true;
        }
        void add_message(string language, string id, string message, bool NewLineIfDuplicated = true) {
            if(!languages.count(language) || !languages[language]) return;
            if(translations[language].count(id) == 1) {
                if(NewLineIfDuplicated) translations[language][id] += '\n';
                translations[language][id] += message;
            }
            else translations[language][id] = message;
        }
        int select(string language) {
            if(languages.count(language) && languages[language]) {
                this->language = language;
                return 0;
            }
            else {
                return 1;
            }
        }
        string translate(string id, int NewLine = 1) {
            string Lines;
            for(int i = 1;i <= NewLine;i++) Lines += '\n';
            if(translations[this->language].count(id) == 0)  return id + Lines;
            return translations[this->language][id] + Lines;
        }
        string translate(string language, string id, int NewLine = 1) {
            string Lines;
            for(int i = 1;i <= NewLine;i++) Lines += '\n';
            if(translations[language].count(id) == 0) return id + Lines;
            return translations[language][id] + Lines;
        }
};


#endif
