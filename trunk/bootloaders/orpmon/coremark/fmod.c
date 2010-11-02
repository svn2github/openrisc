
typedef unsigned int u_int32_t;
typedef signed int int32_t;

// Big endian struct...
typedef union {
	double value;
	struct {
		u_int32_t msw;
		u_int32_t lsw;
	} parts;
} ieee_double_shape_type;

/*
typedef union
{
  double value;
  struct
  {
    u_int32_t lsw;
    u_int32_t msw;
  } parts;
} ieee_double_shape_type;

*/
/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
do {								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
do {								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
} while (0)

/* A union which permits us to convert between a float and a 32 bit
   int.  */

typedef union {
	float value;
	u_int32_t word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

static const double one = 1.0;

double modf(double x, double *iptr)
{
	int32_t i0, i1, j0;
	u_int32_t i;
	EXTRACT_WORDS(i0, i1, x);
	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;	/* exponent of x */
	if (j0 < 20) {		/* integer part in high x */
		if (j0 < 0) {	/* |x|<1 */
			INSERT_WORDS(*iptr, i0 & 0x80000000, 0);	/* *iptr = +-0 */
			return x;
		} else {
			i = (0x000fffff) >> j0;
			if (((i0 & i) | i1) == 0) {	/* x is integral */
				u_int32_t high;
				*iptr = x;
				GET_HIGH_WORD(high, x);
				INSERT_WORDS(x, high & 0x80000000, 0);	/* return +-0 */
				return x;
			} else {
				INSERT_WORDS(*iptr, i0 & (~i), 0);
				return x - *iptr;
			}
		}
	} else if (j0 > 51) {	/* no fraction part */
		u_int32_t high;
		*iptr = x * one;
		GET_HIGH_WORD(high, x);
		INSERT_WORDS(x, high & 0x80000000, 0);	/* return +-0 */
		return x;
	} else {		/* fraction part in low x */
		i = ((u_int32_t) (0xffffffff)) >> (j0 - 20);
		if ((i1 & i) == 0) {	/* x is integral */
			u_int32_t high;
			*iptr = x;
			GET_HIGH_WORD(high, x);
			INSERT_WORDS(x, high & 0x80000000, 0);	/* return +-0 */
			return x;
		} else {
			INSERT_WORDS(*iptr, i0, i1 & (~i));
			return x - *iptr;
		}
	}
}
