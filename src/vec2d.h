typedef struct {
    double x;
    double y;
} vec2d_t;

vec2d_t vec2d_new(double x, double y);

vec2d_t vec2d_add(vec2d_t vec0, vec2d_t vec1);

vec2d_t vec2d_sub(vec2d_t vec0, vec2d_t vec1);

vec2d_t vec2d_mul(vec2d_t vec0, vec2d_t vec1);

vec2d_t vec2d_div(vec2d_t vec0, vec2d_t vec1);

vec2d_t vec2d_add_scl(vec2d_t vec0, double scalar);

vec2d_t vec2d_sub_scl(vec2d_t vec0, double scalar);

vec2d_t vec2d_mul_scl(vec2d_t vec0, double scalar);

vec2d_t vec2d_div_scl(vec2d_t vec0, double scalar);

vec2d_t vec2d_norm(vec2d_t vec);

double vec2d_len(vec2d_t vec);
