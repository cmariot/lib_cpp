#include "data_structures/data_buffer.hpp"
#include <iostream>
#include <string>
#include <cassert>

void test_seek_and_rewind() {
    DataBuffer buf;
    int a = 10;
    int b = 20;
    buf << a << b;

    int ra = 0;
    int rb = 0;

    buf >> ra;
    assert(ra == 10);
    buf.rewind();
    buf >> ra >> rb;
    assert(ra == 10);
    assert(rb == 20);

    // seek to second int
    buf.rewind();
    // skip first int (size of int)
    buf.seek(sizeof(int));
    buf >> rb;
    assert(rb == 20);
}

void test_move_semantics() {
    DataBuffer buf;
    int x = 7;
    buf << x;

    DataBuffer moved = std::move(buf);
    int rx = 0;
    moved >> rx;
    assert(rx == 7);
}

extern "C" int data_buffer_more_tests(void) {
    try {
        test_seek_and_rewind();
        test_move_semantics();
        return 0;
    } catch (...) {
        return 255;
    }
}
