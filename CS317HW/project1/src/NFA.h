#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <optional>

class NFA {

private:
  struct Symbol {
    std::optional<char> m_char;
    
    Symbol() = default;
    Symbol(char c): m_char(c) {};

    bool isEpsilon() const { return !m_char.has_value(); }

    auto operator<=>(const Symbol&) const = default;
  };

  struct Transition {
    Symbol m_symbol;
    size_t m_dest; 

    auto operator<=>(const Transition&) const = default;
  };

public:
  static constexpr std::string unaryOps  = "*";
  static constexpr std::string binaryOps = "|&";

private:
  size_t m_startState;
  size_t m_acceptState;
 
  using StateTransitions = std::vector<Transition>;
  std::vector<StateTransitions> m_transitions;
  
public:
  /* @brief Creates an NFA from a postfix regular expression */
  static NFA fromPostfix(const std::string& expression, const std::string& symbols);

  /* @brief Creates an NFA that accepts a single symbol */
  static NFA fromSymbol(char symbol);

  /* @brief Creates an NFA that accepts the empty string */
  static NFA fromEpsilon();

   /*  @brief Peforms union operation between the two NFAs
   *
   *  Unite modifies the current NFA to accept strings accepted by
   *  the NFA "other"
   *
   *  @param other The NFA to union with
   *
   * */
  void unite(const NFA& other);

    
  /*  @brief Peforms concatenation operation between the two NFAs
   *
   *  Concat modifies the current NFA to accept strings in the language
   *  S1S2, where S1 is in L(this) and S2 is in L(other)
   *
   *  @param other The NFA to concat with
   * */
  void concat(const NFA& other);

  /*  @brief Peforms kleene star operation on this NFA */
  void kleeneStar();

  friend std::ostream& operator<<(std::ostream& out, const NFA& nfa);

private:
  NFA() = default;

  /* Adds a state with no transitions to the NFA and returns the state id */
  size_t addState(); 

  /* @brief Concantenates the states in states with the current states */
  void addStates(const std::vector<StateTransitions>& states);
  
  /* @brief Adds a transition from source to destination on symbol */
  void addTransition(
      std::size_t source, 
      std::size_t destination, 
      Symbol symbol
      );
  
};
