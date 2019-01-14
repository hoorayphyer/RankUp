#ifndef _COMMUNICATION_HPP_
#define _COMMUNICATION_HPP_

#include "poker.hpp"
#include <vector>
#include <unordered_map>


namespace comm {
  class Node {
  private:
    struct SendProxy {
      const int& src_id;
      std::vector<Card> send_buff;

      void operator>>( int tgt_id ) &&;
    };

  protected:
    std::vector<Card> _buffer;
    int _id;

  public:
    Node( int id ) : _id(id) {}

    Node& operator<< ( const std::vector<Card>& c );
    SendProxy operator>> ( std::vector<Card> c );
  };
}





#endif
