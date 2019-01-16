#ifndef _REFEREE_HPP_
#define _REFEREE_HPP_

#include "declaration.hpp"
#include <vector>
#include <unordered_map>
#include std::pow

// TODO implement this whole class given the following interface
class Referee {
public:
  Pattern parse( const std::vector<Card>& cards , const Declaration& dcl){
    Pattern result;
    auto comp = [&lord = dcl.lord]( const auto& a , const auto& b){
      return compare_single(a , b ,lord);
    }
    std::sort( cards.begin() , cards.end() , comp );
    if (cards.size() == 0){
      return result;
    }
    std::vector<float> singles;
    std::vector<float> pairs;
    result.value = log10(get_value(cards[0] , dcl));
    result.min_single = result.value;
    result.max_single = log10(get_value(cards.back() , dcl));
    singles.push_back(result.value);
    for (int i = 1 ; i < cards.size() ; i++){
      curr_value = log10(get_value(cards[i] , dcl));
      result.value = max(result.value , curr_value);
      if (!singles.empty() && curr_value == singles.back()){
        singles.pop_back();
        pairs.push_back(curr_value);
      }
    }
    if (!pairs.empty()){
      result.min_pair = 2 + log10(pairs[0]);
      result.max_pair = 2 + log10(pairs.back());
    }
    int wagon_length = 0;
    float prev_value = 0;
    for (int i = 0 ; i < pairs.size() ; i++){
      if (pairs[i] == prev_value + 1){
        wagon_length = wagon_length + 1;
      }
      else{
        if (wagon_length > 1){
          result.wagon[wagon_length] ++;
          result.value = max(result.value, log10(pairs[i]*100));
          result.value = max(reslut.value , 2*wagon_length + log10(pairs[i-1]));
          if (result.min_wagon == 0 || reslut.min_wagon > 2*wagon_length + log10(pairs[i-1])){
            result.min_wagon = 2*wagon_length + log10(pairs[i-1]);
          }
          if (result.max_wagon == 0 || reslut.max_wagon < 2*wagon_length + log10(pairs[i-1])){
            result.max_wagon = 2*wagon_length + log10(pairs[i-1]);
          }
        }
        wagon_length = 1;
      }
      prev_value = pairs[i];
    }
    result.singles = singles.size();
    result.pairs = pairs.size();
    for (std::pair<int , int> element : result.wagon){
      result.pairs = result.pairs - (element.first * element.second);
    }
    r
  };

  //std::unordered_map <Rank , int> ValueDict;
  //ValueDict[Rank::_10] = 10;
  //ValueDict[Rank::_10] = 10;
  //ValueDict[Rank::_10] = 10;
  //ValueDict[Rank::_J] = 11;
  //ValueDict[Rank::_Q] = 12;
  //ValueDict[Rank::_K] = 13;
  //ValueDict[Rank::_A] = 14;
  //ValueDict[Rank::_w] = 50;
  //ValueDict[Rank::_W] = 51;
  int value_rank( Rank r ) {
    if ( Rank::_w == r ) return 50;
    if ( Rank::_W == r ) return 51;
    return 2 + r - Rank::_2;
  }

  int get_value(const Card& a , const Declaration& dcl){
    int value = 0;
    //value = ValueDict[a.Rank];
    value = value_rank(a.rank);
    if (a.suit == 4){
      return value;
    }
    value += int(a.suit == dcl.lord.suit && a.rank != dcl.lord.rank)*13;
    value += int(a.rank == dcl.lord.rank)*15;
    value += int(a.suit == dcl.lord.suit);
    return value;
  }

  int insert_pos(const std::<std::vector<Card>& cards , Card& a , const Declaration& dcl){
    int r = cards.size();
    a_value = get_value(a , dcl);
    if (r == 0){
      return 0;
    }
    int l = 0;
    int m = 0;
    while (l < r){
      m = (l + r) / 2;
      m_value = get_value(cards[m] , dcl);
      if (m_value == a_value){
        return m;
      }
      if (m_value < a_value){
        l = m + 1;
      }
      else{
        r = m;
      }
    }
    return l;
  }

  int compare_single(const Card& a ,const Card& b, const Declaration& dcl){
    return get_value(a , dcl) < get_value(b , dcl);
  }
  // TODO: select cards from ONLY one suit

  // This is to check whether other's pattern is legal
  bool is_legal( const std::vector<Card>& cards,
                 const std::vector<Card>& hand,
                 const Declaration& dcl ){
                   if (cards.empty()){
                     return 0; // cannot play nothing
                   }
                   Suit s = cards[0].suit;
                   a_pattern = parse(a , dcl);
                   h_pattern = parse(hand[s] , dcl) // I assume hand is
                 };

  bool compare( const std::vector<Card>& a, const std::vector<Card>& b,
                const Declaration& dcl ){
                  a_pattern = parse(a , dcl);
                  b_pattern = parse(b , dcl);
                  if (a_pattern == b_pattern){
                    if (a_pattern.value < b_pattern.value){
                      return 1;
                    }
                    if (a_pattern.value > b_pattern.value){
                      return -1;
                    }
                    else{
                      return 0;
                    }
                  }
                };

  bool is_lord( const Card& card, const Declaration& dcl ) {
    return ( card.suit == 4 || card.suit == dcl.lord.suit )
      || ( card.rank == dcl.lord.rank );
  }

};

#endif
