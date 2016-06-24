/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cctype>
#include <string>
#include <vector>

int isForwardSlash(int c);
// using a default input parameter: std::isspace
std::vector<std::string> split(const std::string& str, 
                               int delimiter(int) = std::isspace);

#endif
