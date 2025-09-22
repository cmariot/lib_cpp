/**
 * @file observer.hpp
 * @brief Minimal header-only Observer (publish-subscribe) helper.
 *
 * This small utility provides a synchronous publish/subscribe helper
 * keyed by a user-defined event type (`TEvent`). It is intentionally
 * minimal and suitable for single-threaded or externally-synchronized
 * usage. See notes below for extensions (unsubscribe tokens, thread
 * safety, payloads).
 */

#ifndef LIBFTPP_DESIGN_PATTERNS_OBSERVER_HPP
#define LIBFTPP_DESIGN_PATTERNS_OBSERVER_HPP

#include <map>
#include <vector>
#include <functional>

/**
 * @brief Minimal publish/subscribe helper keyed by an event type.
 *
 * @tparam TEvent Key type used to group subscriptions (enum, int,
 *         std::string, ...).
 *
 * Usage:
 * @code{.cpp}
 * Observer<int> obs;
 * obs.subscribe(42, [](){}); // register an empty lambda for event 42
 * obs.notify(42);            // invoke all callbacks registered for 42
 * @endcode
 *
 * Notes:
 * - Callbacks are stored as `std::function<void()>` and are invoked
 *   synchronously from `notify()` in registration order.
 * - This class is NOT thread-safe. If you need concurrent subscribe/
 *   notify, protect it externally with a mutex or implement a copy-on-
 *   write strategy inside the class.
 * - To support unsubscribe, extend `subscribe()` so it returns an ID or
 *   token and add an `unsubscribe(token)` method. See project `pool`
 *   and other headers for example token/handle patterns.
 */
template<typename TEvent>
class Observer {

    public:

        /// Callback function type stored for each subscription.
        using Callback = std::function<void()>;

        /**
         * @brief Register a callback for `event`.
         *
         * The callback is copied/moved into the internal storage. Callbacks
         * are invoked in registration order when `notify(event)` is called.
         *
         * @param event Event key to subscribe to.
         * @param cb    Callable invoked when the event is notified.
         */
        void subscribe(const TEvent& event, Callback cb) {
            subscribers_[event].push_back(std::move(cb));
        }

        /**
         * @brief Notify all subscribers of `event`.
         *
         * If no subscribers are registered for `event` this is a no-op.
         * Exceptions thrown by callbacks are propagated to the caller.
         *
         * @param event Event key to notify.
         */
        void notify(const TEvent& event) const {
            auto it = subscribers_.find(event);
            if (it == subscribers_.end()) return;
            for (const auto &cb : it->second) {
                if (cb) cb();
            }
        }

        /**
         * @brief Remove all subscriptions.
         *
         * Useful for teardown during unit tests or when the observer is
         * being destroyed while callbacks must be released.
         */
        void clear() { subscribers_.clear(); }

    private:

        std::map<TEvent, std::vector<Callback>> subscribers_;
        
};

#endif // LIBFTPP_DESIGN_PATTERNS_OBSERVER_HPP
