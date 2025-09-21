#include "../libftpp.hpp"
#include <cassert>

extern "C" int state_machine_basic_test(void) {
    try {
        StateMachine<int> sm;
        sm.addState(0);
        sm.addState(1);
        bool transitioned = false;
        sm.addTransition(0, 1, [&](){ transitioned = true; });
        bool updated = false;
        sm.addAction(1, [&](){ updated = true; });
        sm.transitionTo(0);
        sm.transitionTo(1);
        if (!transitioned) return 255;
        // update should call the action for current state
        sm.update();
        if (!updated) return 255;
        bool threw = false;
        try { StateMachine<int> sm2; sm2.update(); } catch (...) { threw = true; }
        if (!threw) return 255;
        return 0;
    } catch (...) {
        return 255;
    }
}
