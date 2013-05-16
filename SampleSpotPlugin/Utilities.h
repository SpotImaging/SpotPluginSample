#pragma once
#include <cmath>

/// Summary
///   Rounds floating point numbers to nearest integer, where ties round away from zero
///   For example, 23.5 gets rounded to 24, and -23.5 gets rounded to -24.
///   This method also treats positive and negative values symmetrically,
///   and therefore is free of overall bias if the original numbers are positive or negative with equal probability.
///   However, this rule will still introduce a positive bias for positive numbers, and a negative bias for the negative ones.
/// Argument
///   value - the value to be converted
/// Template Argument
///   FloatType - The floating point data type
/// Returns
///   The converted value
template<typename FloatType>
FloatType round_to_nearest_awayzero(FloatType value )
{
   using std::ceil;
   using std::floor;

   return (value < static_cast<FloatType>(0.0)) ? 
      ceil( value - static_cast<FloatType>(0.5) ) :
      floor( value + static_cast<FloatType>(0.5) );
}
