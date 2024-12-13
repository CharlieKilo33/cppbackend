#include "../model/typedef.h"

namespace model {

    bool operator == (const Speed& lhs, const Speed& rhs) {
        return lhs.vx == rhs.vx && lhs.vy == rhs.vy;
    }

}