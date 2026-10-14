#include <iostream>
#include <format>
#include <cassert>
#include <stack>
#include <stdexcept>

#include "NFA.h"

std::string errorLocation(const std::string& msg, const std::string& s, size_t idx) {
  return std::format("{}{}\n{}^\n", msg, s, std::string(idx + msg.length(), ' '));
}

NFA NFA::fromPostfix(const std::string& expression, const std::string& symbols) {
  std::stack<NFA> stack;
  
  for(size_t i = 0; i < expression.length(); i++) {
    char c = expression[i];

    if(unaryOps.contains(c)) {
      if(stack.size() < 1)
        throw std::invalid_argument(errorLocation("expected symbol: ", expression, i));
      
      stack.top().kleeneStar();
    }
    else if(binaryOps.contains(c)) {
      if(stack.size() < 2)
        throw std::invalid_argument(errorLocation("expected symbol: ", expression, i));
        
      NFA n2 = stack.top();
      stack.pop();
      NFA& n1 = stack.top();

      if(c == '|') n1.unite(n2);
      else n1.concat(n2);
    }
    else if(symbols.contains(c)) {
      stack.push(NFA::fromSymbol(c));
    }
    else if(c == 'E')
      stack.push(NFA::fromEpsilon());
    else
      throw std::invalid_argument(errorLocation("invalid symbol: ", expression, i));
    
  }

  if(stack.size() != 1) 
    throw std::invalid_argument(
        errorLocation("unexpected end of expression: ", expression, expression.length())
        );

  return stack.top();
}

NFA NFA::fromSymbol(char symbol) {
  NFA nfa;

  size_t start = nfa.addState();
  size_t accept = nfa.addState();

  nfa.m_startState = start;
  nfa.m_acceptState = accept;

  nfa.addTransition(start, accept, symbol);

  return nfa;
} 

NFA NFA::fromEpsilon() {
  NFA nfa;

  size_t start = nfa.addState();
  nfa.m_startState = start;
  nfa.m_acceptState = start;

  return nfa;
} 

void NFA::unite(const NFA& other) {
  size_t offset = m_transitions.size();
  addStates(other.m_transitions);
  
  size_t start = addState();
  size_t accept = addState();

  addTransition(start, this->m_startState, {});
  addTransition(start, other.m_startState + offset, {});

  addTransition(this->m_acceptState, accept, {});
  addTransition(other.m_acceptState + offset, accept, {});

  m_startState = start;
  m_acceptState = accept;
} 

void NFA::concat(const NFA& other) {
  size_t offset = m_transitions.size();
  addStates(other.m_transitions);

  addTransition(m_acceptState, other.m_startState + offset, {});
  m_acceptState = other.m_acceptState + offset;
} 

void NFA::kleeneStar() {
  size_t start = addState();
  addTransition(start, m_startState, {});
  addTransition(m_acceptState, start, {});

  m_startState = start;
  m_acceptState = start;
} 

std::ostream& operator<<(std::ostream& out, const NFA& nfa) {
  out << std::format(
      "Start: q{}\nAccept: q{}\n", 
      nfa.m_startState + 1, 
      nfa.m_acceptState + 1
      );

  for(std::size_t src = 0; src < nfa.m_transitions.size(); src++) {
    for(const auto& transition : nfa.m_transitions[src]) {
      char symbol = 
        transition.m_symbol.isEpsilon() 
        ? 'E' 
        : transition.m_symbol.m_char.value();
      
      out << std::format(
          "(q{}, {}) -> q{}\n", 
          src + 1, 
          symbol, 
          transition.m_dest + 1
          );
    }
  }

  return out;
}

size_t NFA::addState() {
  m_transitions.push_back({});
  return m_transitions.size() - 1;
} 

void NFA::addStates(const std::vector<StateTransitions>& states) {
  size_t offset = m_transitions.size();
  size_t count = states.size();

  for(size_t i = 0; i < count; i++) {
    auto transitions = states[i];

    for(auto& t : transitions)
      t.m_dest += offset;

    m_transitions.push_back(std::move(transitions));
  }
} 

void NFA::addTransition(
    std::size_t source, 
    std::size_t destination, 
    Symbol symbol
    ) 
{
  assert(source < m_transitions.size());
  assert(destination < m_transitions.size());

  m_transitions[source].push_back({symbol, destination});
} 


