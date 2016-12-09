#include <okui/config.h>
#include <okui/AnimationChain.h>

#include <stdts/optional.h>

#include <unordered_map>

namespace okui {

/**
* StateMachine provides a mechanism to construct and transition between states.
*
* The template parameter, State, should be a struct that fully encapsulates the managed state, and
* a method named "members" that returns a tuple of references to each state variable. For example:
*
*     struct State {
*         double x, y;
*         constexpr auto members() { return std::tie(x, y); }
*     };
*
* All members must have addition, scalar, and assignment operators defined.
*/
template <typename State>
class StateMachine {
public:
    /**
    * @param initialStateId the id of the initial state
    */
    explicit StateMachine(std::string initialStateId) {
        auto& stateState = _stateStates[initialStateId];
        stateState.influence.reset(1.0);
        _currentStateId = std::move(initialStateId);
    }

    /**
    * Must be called regularly for states to take effect.
    *
    * @return true if the state machine has additional work to be done. this means you should
    * continue to invoke drive() each frame. otherwise, if this method returns false, you can stop
    * invoking it until the next time you alter the state machine
    */
    bool drive();

    void setTransition(
        const std::string& fromState,
        const std::string& toState,
        Animation<double>::Duration duration,
        Animation<double>::Interpolator interpolator = interpolation::Linear<double>,
        Animation<double>::Duration delay = 0s
    ) {
        _stateStates[toState].transitionsIn[fromState] = Transition{duration, interpolator, delay};
    }

    const std::string& state() const { return _currentStateId; }

    /**
    * Transitions to a new state.
    */
    void setState(std::string id) {
        if (id == _currentStateId) { return; }
        auto& stateState = _stateStates[id];
        const auto& transition = stateState.transitionsIn[_currentStateId];
        stateState.influence.target(stateState.influence.current(), transition.delay, 1.0, transition.duration, transition.interpolator);
        auto& from = _stateStates[_currentStateId];
        from.influence.target(from.influence.current(), transition.delay, 0.0, transition.duration, transition.interpolator);
        _currentStateId = std::move(id);
    }

protected:
    /**
    * Implementations of this method should update the given state object accordingly. For example,
    * an implementation for a state machine that flies an object in from the side might look like
    * this:
    *
    *     virtual void update(stdts::string_view id, State& state, bool& isAnimated) override {
    *         state.x = id == "OffScreen" ? -100.0 : 100.0;
    *     }
    *
    * @param id the id of the state being updated
    * @param state the state object that should be updated
    * @param isAnimated set this to true if this method should be invoked continuously. this is not
    *                   necessary to animate the transitions between states (see the transition
    *                   method)
    */
    virtual void update(stdts::string_view id, State& state, bool& isAnimated) = 0;

    /**
    * Implementations of this method should apply the state to the subject. For View states, this
    * would mean invoking methods such as View::setBounds or View::setOpacity based on the state
    * variables.
    */
    virtual void apply(State state) = 0;

private:
    struct Transition {
        Animation<double>::Duration duration{0};
        Animation<double>::Interpolator interpolator = interpolation::Linear<double>;
        Animation<double>::Duration delay{0};
    };

    struct StateState {
        State state;
        AnimationChain<double> influence{0.0};
        bool needsUpdate{true};
        std::unordered_map<std::string, Transition> transitionsIn;
    };

    std::string _currentStateId;
    std::unordered_map<std::string, StateState> _stateStates;

    template <size_t... I, typename DestTuple, typename Operand>
    static auto AddTupleImpl(std::integer_sequence<size_t, I...>, DestTuple&& dest, Operand&& operand) {
        return std::make_tuple(std::plus<>()(std::get<I>(dest), std::get<I>(operand))...);
    }

    template <typename DestTuple, typename Operand>
    static auto AddTuple(DestTuple&& dest, Operand&& operand) {
        return AddTupleImpl(std::make_integer_sequence<size_t, std::tuple_size<DestTuple>::value>{}, std::forward<DestTuple>(dest), std::forward<Operand>(operand));
    }

    template <size_t... I, typename DestTuple>
    static auto ScaleTupleImpl(std::integer_sequence<size_t, I...>, DestTuple&& dest, double scalar) {
        return std::make_tuple(std::multiplies<>()(std::get<I>(dest), scalar)...);
    }

    template <typename DestTuple>
    static auto ScaleTuple(DestTuple&& dest, double scalar) {
        return ScaleTupleImpl(std::make_integer_sequence<size_t, std::tuple_size<DestTuple>::value>{}, std::forward<DestTuple>(dest), scalar);
    }
};

template <typename State>
bool StateMachine<State>::drive() {
    bool needsMoreUpdates = false;
    stdts::optional<State> state;
    double totalInfluence = 0.0;
    for (auto& kv : _stateStates) {
        const auto currentInfluence = kv.second.influence.current();
        if (currentInfluence > 0.0) {
            if (kv.second.needsUpdate) {
                kv.second.needsUpdate = false;
                update(kv.first, kv.second.state, kv.second.needsUpdate);
                if (kv.second.needsUpdate) {
                    needsMoreUpdates = true;
                }
            }
            if (state) {
                state->members() = AddTuple(state->members(), ScaleTuple(kv.second.state.members(), currentInfluence));
            } else {
                state.emplace();
                state->members() = ScaleTuple(kv.second.state.members(), currentInfluence);
            }
            totalInfluence += currentInfluence;
        }
        if (!kv.second.influence.isComplete()) {
            needsMoreUpdates = true;
        }
    }
    if (state) {
        if (totalInfluence) {
            state->members() = ScaleTuple(state->members(), 1.0 / totalInfluence);
        }
        apply(std::move(*state));
    }
    return needsMoreUpdates;
}

} // namespace okui
