#include "communication.hpp"

namespace comm {
  struct Impl {
    // NOTE the actual implementation should also force recv
    virtual void send( int from, int to, const std::vector<Card>& cards ) = 0;
  };

  const std::unordered_map<int, Impl*> impl;
}

namespace comm {
  void Node::SendProxy::operator>>( int tgt_id ) && {
    return impl.at(tgt_id) -> send( src_id, tgt_id, send_buff );
  }

  Node& Node::operator<< ( const std::vector<Card>& c ) {
    _buffer.insert(_buffer.end(), c.begin(), c.end() );
    return *(this);
  }

  Node::SendProxy Node::operator>> ( std::vector<Card> c ) {
    return SendProxy{ _id, std::move(c) };
  }
}
