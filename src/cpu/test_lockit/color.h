#ifndef TEST_LOCKIT_COLOR_H_
#define TEST_LOCKIT_COLOR_H_

namespace test_lockit {

// PuyoColor for test_lockit. When all puyo color is converted to this enum,
// the real PuyoColor should be used.
enum TLColor {
    TL_EMPTY = 0,
    TL_RED = 1,
    TL_BLUE = 2,
    TL_YELLOW = 3,
    TL_GREEN = 4,
    TL_OJAMA = 9,
};

}  // namespace test_lockit

#endif
