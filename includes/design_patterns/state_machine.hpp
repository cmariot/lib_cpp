#ifndef STATE_MACHINE_HPP
# define STATE_MACHINE_HPP

# include <map>
# include <set>
# include <functional>
# include <stdexcept>

/**
 * @file state_machine.hpp
 * @brief Simple finite state machine template keyed by TState.
 */

template<typename TState>
class StateMachine {
public:
    StateMachine() : currentSet(false) {}

    void addState(const TState& state) {
        states.insert(state);
    }

    void addTransition(const TState& startState, const TState& finalState, const std::function<void()>& lambda) {
        if (!states.count(startState) || !states.count(finalState))
            throw std::invalid_argument("State not registered");
        transitions[{startState, finalState}] = lambda;
    }

    void addAction(const TState& state, const std::function<void()>& lambda) {
        if (!states.count(state)) throw std::invalid_argument("State not registered");
        actions[state] = lambda;
    }

    void transitionTo(const TState& state) {
        if (!states.count(state)) throw std::invalid_argument("State not registered");
        if (!currentSet) {
            current = state;
            currentSet = true;
            return;
        }
        auto it = transitions.find({current, state});
        if (it == transitions.end()) throw std::runtime_error("Transition not registered");
        if (it->second) it->second();
        current = state;
    }

    void update() {
        if (!currentSet) throw std::runtime_error("No current state");
        auto it = actions.find(current);
        if (it == actions.end()) throw std::runtime_error("No action registered for current state");
        if (it->second) it->second();
    }

private:
    std::set<TState> states;
    bool currentSet;
    TState current;
    std::map<std::pair<TState, TState>, std::function<void()>> transitions;
    std::map<TState, std::function<void()>> actions;
};

#endif // STATE_MACHINE_HPP
