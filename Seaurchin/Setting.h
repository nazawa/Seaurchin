#pragma once

#include <Windows.h>
#include <Shlwapi.h>

#include <stdio.h>

#include <ios>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define ROOT_FILE(dir, fn) (dir + "/" + fn)

#define SU_SETTING_FILE "config.json"

//Settingモジュール初期化
void InitializeSetting(HMODULE hModule);

//設定を読み込む なければ先に書き出す
void SettingLoadSetting();

//設定を書きだす
void SettingSaveSetting();

//整数値読み込み
int SettingReadIntegerValue(std::string group, std::string key, int default = 0);

//実数値読み込み
double SettingReadFloatValue(std::string group, std::string key, double default = 0);

//文字列読み込み
std::string SettingReadStringValue(std::string group, std::string key, std::string default = 0);

//整数値書き込み
void SettingWriteIntegerValue(std::string group, std::string key, int value);

//実数値書き込み
void SettingWriteFloatValue(std::string group, std::string key, double value);

//文字列書き込み
void SettingWriteStringValue(std::string group, std::string key, std::string value);