/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#ifndef TOPIC_H
#define TOPIC_H

#include <string>
#include <vector>

class Topic
{
public:
  Topic();
  Topic(const std::string& root);
  Topic(const Topic& other) = default;
  virtual ~Topic();
  Topic& add(std::string& item) { _topic += "/" + item;
                                  return *this; }
  Topic& add(const char* pItem) { _topic += ("/" + std::string(pItem));
                                  return *this; }
  const char* c_str() const { return _topic.c_str(); }
  std::string operator[] (const int nIndex);
private:
  std::string _topic;
};

#endif // TOPIC_H
