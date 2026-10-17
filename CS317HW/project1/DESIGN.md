
# Overview

A regular expression validation engine to check for well formed Regular Expression in postfix notation. The engine constructs an NFA from a supplied Regular Expression. 

## Input
A txt file as a **command line argument** that contains one Regular Expression in postfix form per line.

## Symbols
- Language: { a, b, c, d, e }
- Operations: { |, & (concat), \*,  E (epsilon) }
## Output
For each RE, print the RE to *stdout* and print the corresponding NFA as specified below:
	1. Output the start state and accept state on separate lines. 
	2. Output the sorted list of transitions, one transition per line.

## Example
### Input
```example
a*b|
```
### Output
```example
Start: q6
Accept: q7
1
(q2, E) → q3
(q3, E) → q1
(q3, E) → q7
(q4, b) → q5
(q5, E) → q7
(q6, E) → q3
(q6, E) → q4
```
## Notes
- State naming starts with q1
- There can only be one accept state
### NFA Representation

NFAs will contain the start state, accept state **(!)** and transitions.

```
CLASS NFA:
	DATA:
		Start State:      Integer ID of the start state
		Number of States: Number of state
		Transitions:      List of transitions per state
		Accept State:     Integer ID of the accept state
		
	INVARIANTS:
		- Transitions[i] is a list of transitions from state I
		
	OPERATIONS:
		# Concatenates with other as if this NFA is the left operand
		concat(other):   
			add_states(other.states)
			add_transition(this.accept, other.accept + this.num_states, symbol)
			this.accept = other.accept + this.num_states
		
		# Unions this NFA with the other NFA
		union(other):
			offset = this.num_states
			add_states(other.states)
	
			new_start = add_state()
			
			add_transition(new_start, this.start, E)
			add_transition(new_start, other.start + offset, E)
			
			this.start = new_start
			
			new_accept = add_state()
			add_transition(this.accept, new_accept, E)
			add_transition(other.accept + offset, new_accept, E)
			this.accept= new_accept		
			
		# Takes the kneene star of this NFA
		kleene_star():
			s = add_state()
			add_transition(s, this.start, E)
			
			this.start = s
			add_transition(this.accept, this.start, E)
			this.accept = this.start  
		
		# Concantenates this NFAs states with the provided states
		add_states(states):
			state.n += states.len
			for(state : states)
				transitions.add(state)
		
		# Adds a transition on symbol from state1 to state2
		add_transition(state1, state2, symbol):
			transitions[state1].add((state2, symbol))
		
		# Adds a state to the transition table
		add_state()
			transitions.add({})
			this.n++
			
```

```
RECORD TRANSITION:
	DATA:
		Symbol: The symbol to transition
		To:     The integer id of the state to transition to
```

### Notes

- We must handle combining NFAs such that the states have the correct output number. To do this, we will first add all of the states from NFA2 to NFA1, effectively offset the state ID for NFA2 by the amount of states in NFA1. We then add start states and end states according to the handout.
  