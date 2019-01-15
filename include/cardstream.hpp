#ifndef _CARDSTREAM_HPP_
#define _CARDSTREAM_HPP_

#include "poker.hpp"
#include <vector>

// NOTE: cardstream is implemented with the same stream ideas from std, except that it supports a intuitive syntactical sugar which looks like this:
//     /cstream1/ >> /card/ >> /cstream2/;
// which means from /cstream1/ deal /card/ which later is sent to /cstream2/
// NOTE: _data is implemented using std::vector, so cardstream is not the same as just std::vector. In contrast, in the two operator>>s, the type of c has to be std::vector
class cardstream {
private:
  virtual std::vector<Card> select_cards() = 0;

protected:
  std::vector<Card> _data;

public:
  // NOTE the return type is not conventional, which is to suit the cardstream flow syntax
  std::vector<Card>& operator>> ( std::vector<Card>& c );
  friend void operator>>( std::vector<Card>, cardstream& );
};

// NOTE like above, this is as a result of the cardstream flow syntax. The more conventional way is void operator<< ( cardstream& cstream, std::vector<Card> c );
// TODO this is where all magic happens. Implement this. It is the ultimate connection between all cardstreams even across device
void operator>>( std::vector<Card> c, cardstream& cstream );





#endif
