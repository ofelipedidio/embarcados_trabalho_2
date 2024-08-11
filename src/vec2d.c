#include "vec2d.h"

vec2d_t vec2d_new(double x, double y) {
    return (vec2d_t) {
        .x = x,
        .y = y,
    };
}

vec2d_t vec2d_add(vec2d_t vec0, vec2d_t vec1) {
    return (vec2d_t) {
        .x = vec0.x + vec1.x,
        .y = vec0.y + vec1.y,
    };
}

vec2d_t vec2d_sub(vec2d_t vec0, vec2d_t vec1) {
    return (vec2d_t) {
        .x = vec0.x - vec1.x,
        .y = vec0.y - vec1.y,
    };
}

vec2d_t vec2d_mul(vec2d_t vec0, vec2d_t vec1) {
    return (vec2d_t) {
        .x = vec0.x * vec1.x,
        .y = vec0.y * vec1.y,
    };
}

vec2d_t vec2d_div(vec2d_t vec0, vec2d_t vec1) {
    return (vec2d_t) {
        .x = vec0.x / vec1.x,
        .y = vec0.y / vec1.y,
    };
}

vec2d_t vec2d_add_scl(vec2d_t vec0, double scalar) {
    return (vec2d_t) {
        .x = vec0.x + scalar,
        .y = vec0.y + scalar,
    };
}

vec2d_t vec2d_sub_scl(vec2d_t vec0, double scalar) {
    return (vec2d_t) {
        .x = vec0.x - scalar,
        .y = vec0.y - scalar,
    };
}

vec2d_t vec2d_mul_scl(vec2d_t vec0, double scalar) {
    return (vec2d_t) {
        .x = vec0.x * scalar,
        .y = vec0.y * scalar,
    };
}

vec2d_t vec2d_div_scl(vec2d_t vec0, double scalar) {
    return (vec2d_t) {
        .x = vec0.x / scalar,
        .y = vec0.y / scalar,
    };
}

double _abs(double n) {
    if (n > 0) {
        return n;
    } else {
        return -n;
    }
}

double root(double n) {
    if (n < 0) {
        return 1;
    }

    double lo;
    double hi;
    double mid;
    if (n > 1) {
        lo = 1;
        hi = n/2;
    } else {
        lo = 0;
        hi = n;
    }

    for(int i = 0 ; i < 4 ; i++) {
        mid = (lo+hi)/2;
        if(_abs(mid*mid - n) < 0.001) return mid;
        if(mid*mid > n) hi = mid;
        else lo = mid;
    }
    return mid;
}

vec2d_t vec2d_norm(vec2d_t vec) {
    double vec_size = root((vec.x * vec.x) + (vec.y * vec.y));
    if (vec_size < 0.0000001) {
        return (vec2d_t) {
            .x = 0.0,
                .y = 0.0,
        };
    } else {
        return (vec2d_t) {
            .x = vec.x / vec_size,
                .y = vec.y / vec_size,
        };
    }
}

double vec2d_len(vec2d_t vec) {
    return root((vec.x * vec.x) + (vec.y * vec.y));
}
