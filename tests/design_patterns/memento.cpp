#include "../libftpp.hpp"
#include <cassert>
#include <iostream>

class SaveablePoint : public Memento {
public:
    SaveablePoint(int x = 0, int y = 0) : _x(x), _y(y) {}

    void set(int x, int y) { _x = x; _y = y; }
    int x() const { return _x; }
    int y() const { return _y; }

protected:
    void _saveToSnapshot(Snapshot& snapshot) const override {
        snapshot.save("x", _x);
        snapshot.save("y", _y);
    }

    void _loadFromSnapshot(const Snapshot& snapshot) override {
        _x = snapshot.get<int>("x");
        _y = snapshot.get<int>("y");
    }

private:
    int _x;
    int _y;
};

extern "C" int memento_basic_test(void) {
    try {
        SaveablePoint p(1, 2);
        auto snap = p.save();
        p.set(10, 20);
        if (p.x() != 10 || p.y() != 20) return 255;
        p.load(snap);
        if (p.x() != 1 || p.y() != 2) return 255;
        return 0;
    } catch (...) {
        return 255;
    }
}
