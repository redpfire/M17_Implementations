//--------------------------------------------------------------------
// M17 C library - m17math.c
//
// This file contains:
// - absolute difference value
// - Euclidean norm (L2) calculation for n-dimensional vectors (float)
// - soft-valued arrays to integer conversion (and vice-versa)
// - fixed-valued multiplication and division
//
// Wojciech Kaczmarski, SP5WWP
// M17 Project, 29 December 2023
//--------------------------------------------------------------------
#include <math.h>
#include "m17math.h"

/**
 * @brief Utility function returning the absolute value of a difference between
 * two fixed-point values.
 *
 * @param v1 First value.
 * @param v2 Second value.
 * @return abs(v1-v2) value.
 */
uint16_t q_abs_diff(const uint16_t v1, const uint16_t v2)
{
    if(v2 > v1) return v2 - v1;
    return v1 - v2;
}

/**
 * @brief Calculate L2 norm between two n-dimensional vectors of floats.
 * 
 * @param in1 Vector 1.
 * @param in2 Vector 2.
 * @param n Vectors' size.
 * @return float L2 norm.
 */
float eucl_norm(const float* in1, const int8_t* in2, const uint8_t n)
{
    float tmp = 0.0f;

    for(uint8_t i=0; i<n; i++)
    {
        tmp += powf(in1[i]-(float)in2[i], 2.0f);
    }

    return sqrt(tmp);
}

/**
 * @brief Convert an unsigned int into an array of soft, fixed-point values.
 * Maximum length is 16. LSB is at index 0.
 * @param out Pointer to an array of uint16_t.
 * @param in Input value.
 * @param len Input's bit length.
 */
void int_to_soft(uint16_t* out, const uint16_t in, const uint8_t len)
{
	for(uint8_t i=0; i<len; i++)
	{
		(in>>i)&1 ? (out[i]=0xFFFF) : (out[i]=0);
	}
}

/**
 * @brief Convert an array of soft, fixed-point
 * Maximum length is 16. LSB is at index 0.
 * @param in Pointer to an array of uint16_t.
 * @param len Input's length.
 * @return uint16_t Return value.
 */
uint16_t soft_to_int(const uint16_t* in, const uint8_t len)
{
	uint16_t tmp=0;

	for(uint8_t i=0; i<len; i++)
	{
		if(in[i]>0x7FFFU)
			tmp|=(1<<i);
	}

	return tmp;
}

/**
 * @brief 1st quadrant fixed point division with saturation.
 * 
 * @param a Dividend.
 * @param b Divisor.
 * @return uint16_t Quotient = a/b.
 */
uint16_t div16(const uint16_t a, const uint16_t b)
{
	uint32_t aa=(uint32_t)a<<16;
	uint32_t r=aa/b;

	return r<=0xFFFFU ? r : 0xFFFFU;
}

/**
 * @brief 1st quadrant fixed point multiplication.
 * 
 * @param a Multiplicand.
 * @param b Multiplier.
 * @return uint16_t Product = a*b.
 */
uint16_t mul16(const uint16_t a, const uint16_t b)
{
	return (uint16_t)(((uint32_t)a*b)>>16);
}

/**
 * @brief Bilinear interpolation (soft-valued expansion) for XOR.
 * 
 * @param a Input A.
 * @param b Input B.
 * @return uint16_t Output = A xor B.
 */
uint16_t soft_bit_XOR(const uint16_t a, const uint16_t b)
{
	return mul16(div16(0xFFFF-b, 0xFFFF), div16(a, 0xFFFF)) + mul16(div16(b, 0xFFFF), div16(0xFFFF-a, 0xFFFF));
}

/**
 * @brief XOR for vectors of soft-valued logic.
 * Max length is 16.
 * @param out Output vector = A xor B.
 * @param a Input vector A.
 * @param b Input vector B.
 * @param len Vectors' size.
 */
void soft_XOR(uint16_t* out, const uint16_t* a, const uint16_t* b, const uint8_t len)
{
	for(uint8_t i=0; i<len; i++)
		out[i]=soft_bit_XOR(a[i], b[i]);
}