#ifndef OBSERVER_HPP
# define OBSERVER_HPP

# include <map>
# include <vector>
# include <functional>
# include <stdexcept>

/**
 * @file observer.hpp
 * @brief Simple Observer implementation keyed by event value.
 */

template<typename TEvent>
class Observer {
public:
    /**
     * Subscribe a callback to a specific event.
     * The callback will be invoked when notify(event) is called.
     */
    void subscribe(const TEvent& event, const std::function<void()>& lambda) {
        subscribers[event].push_back(lambda);
    }

    /**
     * Notify all subscribers registered for the event.
     */
    void notify(const TEvent& event) {
        auto it = subscribers.find(event);
        if (it == subscribers.end()) return;
        for (auto &cb : it->second) {
            if (cb) cb();
        }
    }

private:
    std::map<TEvent, std::vector<std::function<void()>>> subscribers;
};

#endif // OBSERVER_HPP
