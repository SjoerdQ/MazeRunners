/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#include "Topic.h"
#include "Tokenizer.h"

namespace {
int isSlash(int c)
{
    return c == '/';
}
}

Topic::Topic()
    : _topic{""}
{
}

Topic::Topic(const std::string& root)
    : _topic{root}
{
}

Topic::~Topic()
{
}

std::string Topic::operator[](const int nIndex)
{
    return split(_topic, isSlash)[nIndex];
}