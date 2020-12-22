/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.2.1 (statistics_SFX.c) - Source Finding Application          ///
/// Copyright (C) 2020 Tobias Westmeier                                  ///
/// ____________________________________________________________________ ///
///                                                                      ///
/// Address:  Tobias Westmeier                                           ///
///           ICRAR M468                                                 ///
///           The University of Western Australia                        ///
///           35 Stirling Highway                                        ///
///           Crawley WA 6009                                            ///
///           Australia                                                  ///
///                                                                      ///
/// E-mail:   tobias.westmeier [at] uwa.edu.au                           ///
/// ____________________________________________________________________ ///
///                                                                      ///
/// This program is free software: you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation, either version 3 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                      ///
/// This program is distributed in the hope that it will be useful,      ///
/// but WITHOUT ANY WARRANTY; without even the implied warranty of       ///
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         ///
/// GNU General Public License for more details.                         ///
///                                                                      ///
/// You should have received a copy of the GNU General Public License    ///
/// along with this program. If not, see http://www.gnu.org/licenses/.   ///
/// ____________________________________________________________________ ///
///                                                                      ///


// WARNING: This is a template that needs to be instantiated before use.
//          Do not edit template instances, as they are auto-generated!


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "statistics_SFX.h"



// -------------------------------- //
// Check if data array contains NaN //
// -------------------------------- //

// --------------------------------------------------------- //
// Check if data contains NaN                                //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)      data - Pointer to the input data array         //
//   (2)      size - Size of the input data array            //
//                                                           //
// Returns:                                                  //
//                                                           //
//   True if NaN found, false otherwise.                     //
//                                                           //
// Description:                                              //
//                                                           //
//   Checks if the data array contains values of Not a Num-  //
//   ber (NaN).                                              //
// --------------------------------------------------------- //

int contains_nan_SFX(const DATA_T *data, const size_t size)
{
	const DATA_T *ptr = data + size;
	while(ptr --> data) if(IS_NAN(*ptr)) return 1;
	return 0;
}



// --------------------------------------------------------- //
// Check if data contains Inf                                //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)      data - Pointer to the input data array         //
//   (2)      size - Size of the input data array            //
//   (3)  flag_inf - If true, set Inf values to NaN          //
//                                                           //
// Returns:                                                  //
//                                                           //
//   True if Inf found, false otherwise.                     //
//                                                           //
// Description:                                              //
//                                                           //
//   Checks if the data array contains values of infinity    //
//   (Inf). If flag_inf is set to true, all values of Inf    //
//   will be replaced with NaN.                              //
// --------------------------------------------------------- //

int contains_inf_SFX(DATA_T *data, const size_t size, const bool flag_inf)
{
	DATA_T *ptr = data + size;
	bool inf_found = false;
	
	while(ptr --> data)
	{
		if(isinf(*ptr))
		{
			if(flag_inf)
			{
				inf_found = true;
				*ptr = NAN;
			}
			else return true;
		}
	}
	
	return inf_found;
}



// --------------------------------------------------------- //
// Maximum and minimum                                       //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)      data - Pointer to the input data array         //
//   (2)      size - Size of the input data array            //
//   (3) value_max - Pointer to variable for holding maximum //
//   (4) value_min - Pointer to variable for holding minimum //
//                                                           //
// Returns:                                                  //
//                                                           //
//   No return value.                                        //
//                                                           //
// Description:                                              //
//                                                           //
//   Determines the maximum and minimum value in the input   //
//   data array simultaneously. This is faster that calling  //
//   the max() and min() functions separately in situations  //
//   where both values are required. If the array contains   //
//   only NaN, value_min and value_max are both set to NaN.  //
// --------------------------------------------------------- //

void max_min_SFX(const DATA_T *data, const size_t size, DATA_T *value_max, DATA_T *value_min)
{
	const DATA_T *tmp1;
	const DATA_T *tmp2;
	const DATA_T *data2 = data + 1;
	const DATA_T *ptr = data + size - 1;
	const DATA_T *result_min;
	const DATA_T *result_max;
	
	// Find last non-NaN element
	while(IS_NAN(*ptr) && data <-- ptr);
	result_max = ptr;
	
	if(IS_NAN(*result_max))
	{
		// Data array only contains NaN
		*value_max = NAN;
		*value_min = NAN;
		return;
	}
	
	if(ptr == data)
	{
		// Data array only contains one non-NaN value
		*value_max = *result_max;
		*value_min = *result_max;
		return;
	}
	
	// Find second-to-last non-NaN element
	if(--ptr > data) while(IS_NAN(*ptr) && data <-- ptr);
	result_min = ptr;
	
	if(IS_NAN(*result_min))
	{
		// Data array only contains one non-NaN value
		*value_max = *result_max;
		*value_min = *result_max;
		return;
	}
	
	// Swap min and max if necessary
	if(*result_min > *result_max)
	{
		tmp1 = result_min;
		result_min = result_max;
		result_max = tmp1;
	}
	
	// Iterate over remaining array to update min and max
	while(ptr > data2)
	{
		tmp1 = --ptr;
		tmp2 = --ptr;
		
		// Bizarrely, the following faster by more than a factor of 2
		// than first checking if tmp1 > tmp2, presumably due to branch
		// predictor optimisation.
		if(*tmp1 > *result_max || *tmp2 > *result_max)
		{
			if(*tmp2 > *tmp1) result_max = tmp2;
			else result_max = tmp1;
		}
		if(*tmp1 < *result_min || *tmp2 < *result_min)
		{
			if(*tmp2 < *tmp1) result_min = tmp2;
			else result_min = tmp1;
		}
	}
	
	// Ensure that we didn't miss the first array element
	if(ptr > data)
	{
		tmp1 = --ptr;
		if(*tmp1 > *result_max) result_max = tmp1;
		if(*tmp1 < *result_min) result_min = tmp1;
	}
	
	*value_max = *result_max;
	*value_min = *result_min;
	
	return;
}



// --------------------------------------------------------- //
// Maximum                                                   //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the input data array              //
//   (2) size - Size of the input data array                 //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Maximum value of the input array. NaN will be returned  //
//   for NaN-only input arrays.                              //
//                                                           //
// Description:                                              //
//                                                           //
//   Determines the maximum value in the input data array.   //
//   If the array contains only NaN, then NaN is returned.   //
// --------------------------------------------------------- //

DATA_T max_SFX(const DATA_T *data, const size_t size)
{
	const DATA_T *result = data + size - 1;
	while(IS_NAN(*result) && data <-- result);
	const DATA_T *ptr = result;
	
	while(ptr --> data) if(*ptr > *result) result = ptr;
	
	return *result;
}



// --------------------------------------------------------- //
// Minimum                                                   //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the input data array              //
//   (2) size - Size of the input data array                 //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Minimum value of the input array. NaN will be returned  //
//   for NaN-only input arrays.                              //
//                                                           //
// Description:                                              //
//                                                           //
//   Determines the minimum value in the input data array.   //
//   If the array contains only NaN, then NaN is returned.   //
// --------------------------------------------------------- //

DATA_T min_SFX(const DATA_T *data, const size_t size)
{
	const DATA_T *result = data + size - 1;
	while(IS_NAN(*result) && data <-- result);
	const DATA_T *ptr = result;
	
	while(ptr --> data) if(*ptr < *result) result = ptr;
	
	return *result;
}



// --------------------------------------------------------- //
// Sum and mean                                              //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the input data array              //
//   (2) size - Size of the input data array                 //
//   (3) mean - If true, return the mean; otherwise the sum  //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Sum or mean of the data array. NaN will be returned for //
//   NaN-only input arrays.                                  //
//                                                           //
// Description:                                              //
//                                                           //
//   Depending on the value of mean, either the sum or the   //
//   mean of the non-NaN elements of the input array is      //
//   returned. Two convenient wrapper functions, sum() and   //
//   mean(), have been provided to explicitly return the sum //
//   and the mean of the array, respectively.                //
// --------------------------------------------------------- //

double summation_SFX(const DATA_T *data, const size_t size, const bool mean)
{
	double result = 0.0;
	size_t counter = 0;
	const DATA_T *ptr = data + size;
	
	while(ptr --> data)
	{
		if(IS_NOT_NAN(*ptr))
		{
			result += *ptr;
			++counter;
		}
	}
	
	if(counter) return mean ? result / counter : result;
	return NAN;
}
double sum_SFX(const DATA_T *data, const size_t size) { return summation_SFX(data, size, false); }
double mean_SFX(const DATA_T *data, const size_t size) { return summation_SFX(data, size, true); }



// --------------------------------------------------------- //
// N-th moment about value                                   //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)  data - Pointer to the input data array             //
//   (2)  size - Size of the input data array                //
//   (3) order - Order of the moment to be calculated        //
//   (4) value - Value about which to calculate the moment   //
//                                                           //
// Returns:                                                  //
//                                                           //
//   N-th moment of the data array. NaN will be returned for //
//   NaN-only input arrays.                                  //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the N-th moment of the input data array. The //
//   N-th moment is defined as                               //
//                                                           //
//     sum((x - value)^N) / n                                //
//                                                           //
//   where the summation is over all n data values, x, of    //
//   the input array that are not NaN. The most sensible use //
//   would be to calculate the moment about the mean.        //
// --------------------------------------------------------- //

double moment_SFX(const DATA_T *data, const size_t size, unsigned int order, const double value)
{
	if(order == 0) return 1.0;
	
	double result = 0.0;
	size_t counter = 0;
	double tmp;
	unsigned int i;
	const DATA_T *ptr = data + size;
	
	while(ptr --> data)
	{
		if(IS_NOT_NAN(*ptr))
		{
			tmp = *ptr - value;
			for(i = order; --i;) tmp *= (*ptr - value);
			result += tmp;
			++counter;
		}
	}
	
	return counter ? result / counter : NAN;
}



// --------------------------------------------------------- //
// Moments 2, 3 and 4 about value                            //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)  data - Pointer to the input data array             //
//   (2)  size - Size of the input data array                //
//   (3) value - Value about which to calculate the moments  //
//   (4)    m2 - Pointer to a variable for storing moment 2  //
//   (5)    m3 - Pointer to a variable for storing moment 3  //
//   (6)    m4 - Pointer to a variable for storing moment 4  //
//                                                           //
// Returns:                                                  //
//                                                           //
//   No return value.                                        //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the 2nd, 3rd and 4th moment of a data array  //
//   simultaneously. This will enable faster calculation of  //
//   skewness and kurtosis, both of which depend on multiple //
//   moments of the same data array. Moments are defined in  //
//   the same way as in function moment().                   //
// --------------------------------------------------------- //

void moments_SFX(const DATA_T *data, const size_t size, const double value, double *m2, double *m3, double *m4)
{
	const DATA_T *ptr = data + size;
	double result2 = 0.0;
	double result3 = 0.0;
	double result4 = 0.0;
	size_t counter = 0;
	double tmp, tmp2;
	
	while(ptr --> data)
	{
		if(IS_NOT_NAN(*ptr))
		{
			tmp = *ptr - value;
			tmp2 = tmp * tmp;
			result2 += tmp2;
			result3 += tmp2 * tmp;
			result4 += tmp2 * tmp2;
			++counter;
		}
	}
	
	if(counter)
	{
		*m2 = result2 / counter;
		*m3 = result3 / counter;
		*m4 = result4 / counter;
	}
	else
	{
		*m2 = NAN;
		*m3 = NAN;
		*m4 = NAN;
	}
	
	return;
}



// --------------------------------------------------------- //
// Standard deviation about value                            //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)    data - Pointer to the input data array           //
//   (2)    size - Size of the input data array              //
//   (3)   value - Value about which to calculate the        //
//                 standard deviation                        //
//   (4) cadence - Can be set to > 1 to speed up algorithm   //
//   (4)   range - Flux range to be used. Can be Negative    //
//                 (-1), Full (0) or Positive (1).           //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Standard deviation about the specified value of the     //
//   elements in the input data array. NaN will be returned  //
//   for NaN-only input arrays.                              //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the standard deviation about a user-speci-   //
//   fied value of all elements in the input data array that //
//   are not NaN. The standard deviation is defined as       //
//                                                           //
//     sqrt[sum(x - value)^2 / n]                            //
//                                                           //
//   where the summation is over all n non-NaN elements, x,  //
//   of the input array.                                     //
// --------------------------------------------------------- //

double std_dev_val_SFX(const DATA_T *data, const size_t size, const double value, const size_t cadence, const int range)
{
	const DATA_T *ptr = data + size;
	const DATA_T *ptr2 = data + cadence - 1;
	double result = 0.0;
	size_t counter = 0;
	double tmp;
	
	while(ptr > ptr2)
	{
		ptr -= cadence;
		
		if((range == 0 && IS_NOT_NAN(*ptr)) || (range < 0 && *ptr < 0.0) || (range > 0 && *ptr > 0.0))
		{
			tmp = (*ptr - value);
			result += tmp * tmp;
			++counter;
		}
	}
	
	return counter ? sqrt(result / counter) : NAN;
}



// --------------------------------------------------------- //
// Standard deviation about mean                             //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the input data array              //
//   (2) size - Size of the input data array                 //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Standard deviation about the mean of the elements in    //
//   the input data array. NaN will be returned for NaN-only //
//   input arrays.                                           //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the standard deviation about the mean of all //
//   elements in the input data array that are not NaN. The  //
//   standard deviation is defined as                        //
//                                                           //
//     sqrt{sum[x - mean(x)]^2 / n}                          //
//                                                           //
//   where the summation is over all n non-NaN elements, x,  //
//   of the input array.                                     //
// --------------------------------------------------------- //

double std_dev_SFX(const DATA_T *data, const size_t size)
{
	return std_dev_val_SFX(data, size, mean_SFX(data, size), 1, 0);
}



// --------------------------------------------------------- //
// N-th smallest element in array                            //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the data array to be sorted       //
//   (2) size - Size of the input array                      //
//   (3)    n - n-th smallest value will be returned         //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Value of the n-th smallest array element.               //
//                                                           //
// Description:                                              //
//                                                           //
//   Partially sorts the input data array until the n-th-    //
//   smallest element is at the n-th position within the     //
//   array. After sorting, all elements below position n     //
//   will be smaller than or equal to the n-th-smallest      //
//   element, while all elements above position n will be    //
//   greater than or equal to the n-th-smallest element. The //
//   value of the n-th-smallest element is returned.         //
//   Note that this function is not NaN-safe and will modify //
//   the original data array.                                //
// --------------------------------------------------------- //

DATA_T nth_element_SFX(DATA_T *data, const size_t size, const size_t n)
{
	DATA_T *l = data;
	DATA_T *m = data + size - 1;
	DATA_T *ptr = data + n;
	
	while(l < m)
	{
		DATA_T value = *ptr;
		DATA_T *i = l;
		DATA_T *j = m;
		
		do
		{
			while(*i < value) ++i;
			while(value < *j) --j;
			
			if(i <= j)
			{
				DATA_T tmp = *i;
				*i = *j;
				*j = tmp;
				++i;
				--j;
			}
		} while(i <= j);
		
		if(j < ptr) l = i;
		if(ptr < i) m = j;
	}
	
	return *ptr;
}



// --------------------------------------------------------- //
// Median                                                    //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the data array to be sorted       //
//   (2) size - Size of the input array                      //
//   (3) fast - If true, will return approximate median for  //
//              even-sized arrays. If false, the exact       //
//              median will be returned, which takes longer. //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Median of the data array values.                        //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the exact median of the input data array.    //
//   NOTE that this function is not NaN-safe and will modify //
//   the original data array.                                //
// --------------------------------------------------------- //

DATA_T median_SFX(DATA_T *data, const size_t size, const bool fast)
{
	const DATA_T value = nth_element_SFX(data, size, size / 2);
	return IS_ODD(size) || fast ? value : (value + max_SFX(data, size / 2)) / 2.0;
}

// Same, but does not alter the data array.

DATA_T median_safe_SFX(const DATA_T *data, const size_t size, const bool fast)
{
	DATA_T *data_copy = (DATA_T *)memory(MALLOC, size, sizeof(DATA_T));
	memcpy(data_copy, data, size * sizeof(DATA_T));
	const DATA_T result = median_SFX(data_copy, size, fast);
	free(data_copy);
	return result;
}



// --------------------------------------------------------- //
// Median absolute deviation from value                      //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)  data   - Pointer to the data array to be sorted    //
//   (2)  size   - Size of the input array                   //
//   (3) value   - Value about which to calculate the MAD    //
//   (4) cadence - Can be set to > 1 to speed up algorithm   //
//   (5)   range - Flux range to be used. Can be Negative    //
//                 (-1), Full (0) or Positive (1).           //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Median absolute deviation of the array values from the  //
//   specified data value.                                   //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the median absolute deviation (MAD) of the   //
//   data array values from a user-specified value. The MAD  //
//   is defined as                                           //
//                                                           //
//     median(|x - value|)                                   //
//                                                           //
//   where x denotes the data values from the input array.   //
//   NOTE that this function is NaN-safe and will not modify //
//   the original data array.                                //
// --------------------------------------------------------- //

DATA_T mad_val_SFX(const DATA_T *data, const size_t size, const DATA_T value, const size_t cadence, const int range)
{
	// Create copy of data array with specified range and cadence
	const size_t data_copy_size = (range == 0) ? (size / cadence) : (size / (2 * cadence));
	DATA_T *data_copy = (DATA_T *)memory(MALLOC, data_copy_size, sizeof(DATA_T));
	
	// Some settings
	const DATA_T *ptr = data + size;
	DATA_T *ptr_copy = data_copy;
	size_t counter = 0;
	
	// Copy |*ptr - value| into array copy
	while((ptr -= cadence) > data && counter < data_copy_size)
	{
		if((range < 0 && *ptr < 0.0) || (range == 0 && IS_NOT_NAN(*ptr)) || (range > 0 && *ptr > 0.0))
		{
			ptr_copy[counter] = fabs(*ptr - value);
			++counter;
		}
	}
	
	// Determine median
	const DATA_T result = median_SFX(data_copy, counter, false);
	
	// Clean up
	free(data_copy);
	
	return result;
}



// --------------------------------------------------------- //
// Median absolute deviation                                 //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the data array to be sorted       //
//   (2) size - Size of the input array                      //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Median absolute deviation of the array values.          //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the median absolute deviation (MAD) of the   //
//   data array values. The MAD is defined as                //
//                                                           //
//     median(|x - median(x)|)                               //
//                                                           //
//   where x denotes the data values from the input array.   //
//   In the case of normally distributed random data values  //
//   the standard deviation of the data values about the     //
//   mean can be deduced by multiplying the MAD with the     //
//   constant MAD_TO_STD.                                    //
//   NOTE that this function is not NaN-safe and will modify //
//   the original data array.                                //
// --------------------------------------------------------- //

DATA_T mad_SFX(DATA_T *data, const size_t size)
{
	return mad_val_SFX(data, size, median_SFX(data, size, false), 1, 0);
}



// --------------------------------------------------------- //
// Robust noise measurement for contiguous data array        //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//    (1) data - Pointer to the data array.                  //
//    (2) size - Size of the array.                          //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Robust noise measurement of the data.                   //
//                                                           //
// Description:                                              //
//                                                           //
//   Uses a robust way of measuring the noise within the     //
//   specified data array based on the median absolute de-   //
//   viation (MAD). The following assumptions and conditions //
//   apply:                                                  //
//                                                           //
//    - The noise is Gaussian and centred on zero.           //
//    - Only negative data points will be used in the noise  //
//      measurement.                                         //
//    - The algorithm is NAN-safe and discards all NANs.     //
//                                                           //
//   The result of the median of the absolute values of the  //
//   negative elements in the array will be returned. If no  //
//   valid data are found, NaN will instead be returned.     //
// --------------------------------------------------------- //

DATA_T robust_noise_SFX(const DATA_T *data, const size_t size)
{
	// Allocate memory for 1D data copy
	DATA_T *data_copy = (DATA_T *)memory(MALLOC, size, sizeof(DATA_T));
	DATA_T *ptr_copy = data_copy;
	
	// Copy values of negative elements
	for(const DATA_T *ptr = data + size; ptr --> data;) if(*ptr < 0.0) *ptr_copy++ = *ptr;
	
	// Calculate pseudo-median
	const size_t size_copy = ptr_copy - data_copy;
	const DATA_T result = size_copy ? -MAD_TO_STD * nth_element_SFX(data_copy, size_copy, size_copy / 2) : NAN;
	// NOTE: Multiplication by -1 because values are all negative.
	
	// Clean up and return result
	free(data_copy);
	return result;
}



// Same, but using positive and negative values

DATA_T robust_noise_2_SFX(const DATA_T *data, const size_t size)
{
	// Allocate memory for 1D data copy
	DATA_T *data_copy = (DATA_T *)memory(MALLOC, size, sizeof(DATA_T));
	DATA_T *ptr_copy = data_copy;
	
	// Copy values of all non-NaN elements
	for(const DATA_T *ptr = data + size; ptr --> data;)
	{
		if(*ptr < 0.0) *ptr_copy++ = -(*ptr);
		else if(*ptr >= 0.0) *ptr_copy++ = *ptr;
	}
	
	// Calculate pseudo-median
	const size_t size_copy = ptr_copy - data_copy;
	const DATA_T result = size_copy ? MAD_TO_STD * nth_element_SFX(data_copy, size_copy, size_copy / 2) : NAN;
	
	// Clean up and return result
	free(data_copy);
	return result;
}



// --------------------------------------------------------- //
// Robust noise measurement in region of 3D array            //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the data array.                   //
//   (2) nx   - Size of the data array in x.                 //
//   (3) ny   - Size of the data array in y.                 //
//   (4) x1   - Lower boundary of region in x.               //
//   (5) x2   - Upper boundary of region in x.               //
//   (6) y1   - Lower boundary of region in y.               //
//   (7) y2   - Upper boundary of region in y.               //
//   (8) z1   - Lower boundary of region in z.               //
//   (9) z2   - Upper boundary of region in z.               //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Robust noise measurement within the specified region.   //
//                                                           //
// Description:                                              //
//                                                           //
//   Uses a robust way of measuring the noise within the     //
//   specified region of a 3D data array based on the median //
//   absolute deviation (MAD). The following assumptions     //
//   and conditions apply:                                   //
//                                                           //
//    - The array is contiguous in x and least contiguous    //
//      along the z axis.                                    //
//    - The noise is Gaussian and centred on zero.           //
//    - Only negative data points will be used in the noise  //
//      measurement.                                         //
//    - The algorithm is NAN-safe and discards all NANs.     //
//                                                           //
//   The result of the median of the absolute values of the  //
//   negative elements in the region will be returned. If no //
//   valid data are found, NaN will instead be returned.     //
// --------------------------------------------------------- //

DATA_T robust_noise_in_region_SFX(const DATA_T *data, const size_t nx, const size_t ny, const size_t x1, const size_t x2, const size_t y1, const size_t y2, const size_t z1, const size_t z2)
{
	// Allocate memory for 1D data copy
	DATA_T *data_copy = (DATA_T *)memory(MALLOC, (x2 - x1 + 1) * (y2 - y1 + 1) * (z2 - z1 + 1), sizeof(DATA_T));
	DATA_T *ptr = data_copy;
	DATA_T value;
	
	// Copy absolute values of negative elements
	for(size_t z = z1; z <= z2; ++z)
	{
		for(size_t y = y1; y <= y2; ++y)
		{
			for(size_t x = x1; x <= x2; ++x)
			{
				value = data[x + nx * (y + ny * z)];
				if(value < 0.0) *ptr++ = -value;
			}
		}
	}
	
	// Calculate median
	const size_t size = ptr - data_copy;
	const DATA_T result = size ? MAD_TO_STD * nth_element_SFX(data_copy, size, size / 2) : NAN;
	
	// Clean up
	free(data_copy);
	return result;
}



// --------------------------------------------------------- //
// Create histogram from data array                          //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data     - Pointer to the data array to be sorted.  //
//   (2) size     - Size of the input array.                 //
//   (3) n_bins   - Number of bins of the histogram.         //
//   (4) data_min - Lower flux limit of the histogram.       //
//   (5) data_max - Upper flux limit of the histogram.       //
//   (6) cadence - Cadence for generation of histogram. A    //
//                 cadence of N means that only every N-th   //
//                 data point will be used.                  //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Pointer to the generated histogram.                     //
//                                                           //
// Description:                                              //
//                                                           //
//   Generates a histogram of the data values in 'data' with //
//   the parameters as specified by the user. A pointer to   //
//   the generated histogram will be returned. NOTE that it  //
//   is the responsibility of the user to de-allocate the    //
//   memory occupied by the histogram once it is no longer   //
//   required (using free()) to ensure that there are no me- //
//   mory leaks. This function is NaN-safe, and NaN values   //
//   will simply be ignored (due to >= and <= comparison).   //
// --------------------------------------------------------- //

size_t *create_histogram_SFX(const DATA_T *data, const size_t size, const size_t n_bins, const DATA_T data_min, const DATA_T data_max, const size_t cadence)
{
	// Allocate memory
	size_t *histogram = (size_t *)memory(CALLOC, n_bins, sizeof(size_t));
	
	// Basic setup
	const DATA_T slope = (DATA_T)(n_bins - 1) / (data_max - data_min);
	const DATA_T *ptr  = data + size;
	const DATA_T tmp   = 0.5 - slope * data_min;  // The 0.5 is needed for correct rounding later on.
	
	// Generate histogram
	while((ptr -= cadence) > data)
	{
		if(*ptr >= data_min && *ptr <= data_max)
		{
			size_t bin = (size_t)(slope * *ptr + tmp);
			++histogram[bin];
		}
	}
	
	return histogram;
}



// --------------------------------------------------------- //
// Gaussian fit to histogram                                 //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data    - Pointer to the data array to be sorted.   //
//   (2) size    - Size of the input array.                  //
//   (3) cadence - Cadence for generation of histogram. A    //
//                 cadence of N means that only every N-th   //
//                 data point will be used.                  //
//   (4) range   - Flux range to use in histogram. Can be    //
//                 -1, 0 or +1 to use only negative pixels,  //
//                 all pixels, or only positive pixels, re-  //
//                 spectively.                               //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Standard deviation from Gaussian fit.                   //
//                                                           //
// Description:                                              //
//                                                           //
//   Generates a histogram from the data values in the input //
//   array and fits a Gaussian function to that histogram to //
//   determine the standard deviation. This is useful for    //
//   measuring the noise level in the data cube.             //
//                                                           //
//   The user can choose which pixels from the data cube     //
//   contribute to the histogram (positive, negative, or all //
//   pixels and every N-th pixel as controlled by cadence).  //
//   For the purpose of fitting, the histogram range will be //
//   set such that the second moment fills an optimal frac-  //
//   tion of the histogram width. A linear regression to the //
//   logarithmic histogram, ln(h(x)) = a x^2 + b, will then  //
//   be performed to derive the standard deviation from the  //
//   slope of the fit. The advantage of this technique is    //
//   that the regression can be solved analytically, making  //
//   the algorithm extremely fast. The disadvantage is that  //
//   the weighting of histogram bins will be higher near the //
//   centre of the flux distribution and lower in the wings, //
//   although this can actually be an advantage, as the      //
//   wings are often affected by non-Gaussian effects such   //
//   as flux from actual objects.                            //
// --------------------------------------------------------- //

DATA_T gaufit_SFX(const DATA_T *data, const size_t size, const size_t cadence, const int range)
{
	// Determine maximum and minimum
	DATA_T data_max = 0.0;
	DATA_T data_min = 0.0;
	max_min_SFX(data, size, &data_max, &data_min);
	
	if(data_min >= 0.0 || data_max <= 0.0)
	{
		warning("Maximum is not greater than minimum.");
		return NAN;
	}
	
	// Determine data range
	if(range < 0)
	{
		if(data_min >= 0.0)
		{
			warning("Minimum is not less than zero.");
			return NAN;
		}
		data_max = 0.0;
	}
	else if(range > 0)
	{
		if(data_max <= 0.0)
		{
			warning("Maximum is not greater than zero.");
			return NAN;
		}
		data_min = 0.0;
	}
	else
	{
		const DATA_T limit = fabs(data_min) < fabs(data_max) ? fabs(data_min) : fabs(data_max);
		data_min = -limit;
		data_max = limit;
	}
	
	// Create initial histogram
	const size_t n_bins = 101;
	size_t origin = n_bins / 2;
	if(range < 0) origin = n_bins - 1;
	else if(range > 0) origin = 0;
	const DATA_T inv_optimal_mom2 = 5.0 / n_bins;  // Require standard deviation to cover 1/5th of the histogram for optimal results
	size_t *histogram = create_histogram_SFX(data, size, n_bins, data_min, data_max, cadence);
	
	// Calculate second moment
	DATA_T mom0 = 0.0;
	DATA_T mom1 = 0.0;
	DATA_T mom2 = 0.0;
	
	for(size_t i = n_bins; i--;)
	{
		mom0 += histogram[i];
		mom1 += histogram[i] * (DATA_T)i;
	}
	mom1 /= mom0;
	
	for(size_t i = n_bins; i--;) mom2 += histogram[i] * (mom1 - i) * (mom1 - i);
	mom2 = sqrt(mom2 / mom0);
	
	// Ensure that 2nd moment is equal to optimal_mom2 for optimal fitting
	if(range < 0) data_min *= mom2 * inv_optimal_mom2;
	else if(range > 0) data_max *= mom2 * inv_optimal_mom2;
	else
	{
		data_min *= mom2 * inv_optimal_mom2;
		data_max *= mom2 * inv_optimal_mom2;
	}
	
	// Regenerate histogram with new scaling
	free(histogram);
	histogram = create_histogram_SFX(data, size, n_bins, data_min, data_max, cadence);
	
	// Fit Gaussian function using linear regression
	// (excluding first and last point in case of edge effects)
	DATA_T mean_x = 0.0;
	DATA_T mean_y = 0.0;
	size_t counter = 0;
	
	for(size_t i = n_bins - 1; i --> 1;)
	{
		if(histogram[i])
		{
			long int ii = i - origin;
			mean_x += (DATA_T)(ii * ii);
			mean_y += log((DATA_T)(histogram[i]));
			++counter;
		}
	}
	
	mean_x /= counter;
	mean_y /= counter;
	
	DATA_T upper_sum = 0.0;
	DATA_T lower_sum = 0.0;
	
	for(size_t i = n_bins - 1; i --> 1;)
	{
		if(histogram[i])
		{
			long int ii = i - origin;
			const DATA_T x = (DATA_T)(ii * ii);
			const DATA_T y = log((DATA_T)(histogram[i]));
			upper_sum += (x - mean_x) * (y - mean_y);
			lower_sum += (x - mean_x) * (x - mean_x);
		}
	}
	
	// Determine standard deviation from slope
	const DATA_T sigma = sqrt(-0.5 * lower_sum / upper_sum) * (data_max - data_min) / (n_bins - 1);
	
	// Clean up
	free(histogram);
	
	return sigma;
}



// --------------------------------------------------------- //
// Skewness                                                  //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the data array to be sorted       //
//   (2) size - Size of the input array                      //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Skewness of the array values. NaN will be returned for  //
//   NaN-only input arrays.                                  //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the skewness of the input data array values. //
//   Skewness is defined as                                  //
//                                                           //
//     mom3 / sqrt(mom2^3)                                   //
//                                                           //
//   where mom2 and mom3 are the second and third moment of  //
//   the data as returned by the moment() and moments()      //
//   functions. The skewness of normally distributed data    //
//   with a mean of zero should be 0.                        //
// --------------------------------------------------------- //

double skewness_SFX(const DATA_T *data, const size_t size)
{
	double m2, m3, m4;
	moments_SFX(data, size, mean_SFX(data, size), &m2, &m3, &m4);
	return m3 / sqrt(m2 * m2 * m2);
}



// --------------------------------------------------------- //
// Kurtosis                                                  //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the data array to be sorted       //
//   (2) size - Size of the input array                      //
//                                                           //
// Returns:                                                  //
//                                                           //
//   Kurtosis of the array values. NaN will be returned for  //
//   NaN-only input arrays.                                  //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the kurtosis of the input data array values. //
//   Kurtosis is defined as                                  //
//                                                           //
//     mom4 / mom2^2                                         //
//                                                           //
//   where mom2 and mom4 are the second and fourth moment of //
//   the data as returned by the moment() and moments()      //
//   functions. The kurtosis of normally distributed data    //
//   with a mean of zero should be 3.                        //
// --------------------------------------------------------- //

double kurtosis_SFX(const DATA_T *data, const size_t size)
{
	double m2, m3, m4;
	moments_SFX(data, size, mean_SFX(data, size), &m2, &m3, &m4);
	return m4 / (m2 * m2);
}



// --------------------------------------------------------- //
// Skewness and kurtosis                                     //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data - Pointer to the data array to be sorted       //
//   (2) size - Size of the input array                      //
//   (3) skew - Pointer to variable for returning skewness   //
//   (4) kurt - Pointer to variable for returning kurtosis   //
//                                                           //
// Returns:                                                  //
//                                                           //
//   No return value.                                        //
//                                                           //
// Description:                                              //
//                                                           //
//   Calculates the skewness and kurtosis of the input data  //
//   array simultaneously. This is faster than calculating   //
//   them individually in situations where both are needed.  //
//   See the skewness() and kurtosis() functions for more    //
//   details on how these parameters are defined. Both skew  //
//   and kurt will be set to NaN if the array only contains  //
//   NaN values.                                             //
// --------------------------------------------------------- //

void skew_kurt_SFX(const DATA_T *data, const size_t size, double *skew, double *kurt)
{
	double m2, m3, m4;
	moments_SFX(data, size, mean_SFX(data, size), &m2, &m3, &m4);
	*skew = m3 / sqrt(m2 * m2 * m2);
	*kurt = m4 / (m2 * m2);
	return;
}



// --------------------------------------------------------- //
// 1D boxcar filter                                          //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)          data - Pointer to data array to be         //
//                       filtered.                           //
//   (2)     data_copy - Pointer to data array to be used    //
//                       for storing a copy of the data dur- //
//                       ring filtering. Its size must be    //
//                       equal to size + 2 * filter_radius.  //
//   (3)          size - Size of input array.                //
//   (4) filter_radius - Radius of boxcar filter.            //
//                                                           //
// Returns:                                                  //
//                                                           //
//   No return value.                                        //
//                                                           //
// Description:                                              //
//                                                           //
//   Applies a boxcar filter to the data array. NOTE that    //
//   this will modify the original data array. NaN values    //
//   will be set to 0 prior to filtering only if replace_nan //
//   is set to true (this is to avoid a check within this    //
//   function for reasons of speed). Values outside of the   //
//   boundaries of the array are assumed to be 0.            //
// --------------------------------------------------------- //

void filter_boxcar_1d_SFX(DATA_T *data, DATA_T *data_copy, const size_t size, const size_t filter_radius)
{
	// Define filter size
	const size_t filter_size = 2 * filter_radius + 1;
	const DATA_T inv_filter_size = 1.0 / filter_size;
	size_t i;
	
	// Make copy of data, taking care of NaN
	for(i = size; i--;) data_copy[filter_radius + i] = FILTER_NAN(data[i]);
	
	// Fill overlap regions with 0
	for(i = filter_radius; i--;) data_copy[i] = data_copy[size + filter_radius + i] = 0.0;
	
	// Apply boxcar filter to last data point
	data[size - 1] = 0.0;
	for(i = filter_size; i--;) data[size - 1] += data_copy[size + i - 1];
	data[size - 1] *= inv_filter_size;
	
	// Recursively apply boxcar filter to all previous data points
	for(i = size - 1; i--;) data[i] = data[i + 1] + (data_copy[i] - data_copy[filter_size + i]) * inv_filter_size;
	
	return;
}



// --------------------------------------------------------- //
// 2D Gaussian filter                                        //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1)          data - Pointer to data array to be         //
//                       filtered.                           //
//   (2)     data_copy - Pointer to data array to be used    //
//                       for storing a single column of the  //
//                       data array. Its size must be equal  //
//                       to size_y.                          //
//   (3)      data_row - Pointer to data array to be used by //
//                       the boxcar filter to be employed.   //
//                       Its size must be equal to size_x +  //
//                       2 * filter_radius.                  //
//   (4)      data_col - Pointer to data array to be used by //
//                       the boxcar filter to be employed.   //
//                       Its size must be equal to size_y +  //
//                       2 * filter_radius.                  //
//   (5)        size_x - Size of the first dimension of the  //
//                       input data array.                   //
//   (6)        size_y - Size of the second dimension of the //
//                       input data array.                   //
//   (7)        n_iter - Number of iterations of boxcar fil- //
//                       tering to be carried out.           //
//   (8) filter_radius - Radius of the boxcar filter to be   //
//                       applied. The filter width will be   //
//                       defined as 2 * filter_radius + 1.   //
//                                                           //
// Returns:                                                  //
//                                                           //
//   No return value.                                        //
//                                                           //
// Description:                                              //
//                                                           //
//   Applies a pseudo-Gaussian filter to the two-dimensional //
//   data array by running a series of n_iter boxcar filters //
//   of radius filter_radius across the data array in both   //
//   dimensions. The function optimal_filter_size(...) can   //
//   be used to automatically determine the required number  //
//   of iterations and boxcar filter radius for a given      //
//   standard deviation of the Gaussian.                     //
//                                                           //
//   For reasons of speed several data arrays must have been //
//   pre-allocated and passed on to this function:           //
//                                                           //
//   - data_copy: Used to store a single column of the input //
//                data. Must be of size size_y.              //
//   - data_row:  Used to store a copy of the data passed on //
//                to the boxcar filter. Must be of size      //
//                size_x + 2 * filter_radius.                //
//   - data_col:  Used to store a copy of the data passed on //
//                to the boxcar filter. Must be of size      //
//                size_y + 2 * filter_radius.                //
//                                                           //
//   The sole purpose of having these array created extern-  //
//   ally and then passed on to the function is to improve   //
//   the speed of the algorithm in cases where it needs to   //
//   be invoked a large number of times.                     //
//                                                           //
//   Note that the function will only be able to approximate //
//   a Gaussian, with the approximation becoming better for  //
//   a larger number of iterations of the boxcar filter. 3-4 //
//   iterations should already provide a reasonable approxi- //
//   mation for the purpose of image filtering. Also note    //
//   that the value of the standard deviation of the Gauss-  //
//   ian will only be approximated by the optimal number of  //
//   iterations and filter radius, usually to within several //
//   percent.                                                //
// --------------------------------------------------------- //

void filter_gauss_2d_SFX(DATA_T *data, DATA_T *data_copy, DATA_T *data_row, DATA_T *data_col, const size_t size_x, const size_t size_y, const size_t n_iter, const size_t filter_radius)
{
	// Set up a few variables
	const size_t size_xy = size_x * size_y;
	DATA_T *ptr = data + size_xy;
	DATA_T *ptr2;
	
	// Run row filter (along x-axis)
	// This is straightforward, as the data are contiguous in x.
	while(ptr > data)
	{
		ptr -= size_x;
		for(size_t i = n_iter; i--;) filter_boxcar_1d_SFX(ptr, data_row, size_x, filter_radius);
	}
	
	// Run column filter (along y-axis)
	// This is more complicated, as the data are non-contiguous in y.
	for(size_t x = size_x; x--;)
	{
		// Copy data into column array
		ptr = data + size_xy - size_x + x;
		ptr2 = data_copy + size_y;
		while(ptr2 --> data_copy)
		{
			*ptr2 = *ptr;
			ptr -= size_x;
		}
		
		// Apply filter
		for(size_t i = n_iter; i--;) filter_boxcar_1d_SFX(data_copy, data_col, size_y, filter_radius);
		
		// Copy column array back into data array
		ptr = data + size_xy - size_x + x;
		ptr2 = data_copy + size_y;
		while(ptr2 --> data_copy)
		{
			*ptr = *ptr2;
			ptr -= size_x;
		}
	}
	
	return;
}



// --------------------------------------------------------- //
// Shift and subtract data from itself                       //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) data          - Data array to be processed.         //
//   (2) size          - Size of the data array.             //
//   (3) shift         - Number of positions by which to     //
//                       shift before subtraction.           //
//                                                           //
// Returns:                                                  //
//                                                           //
//   No return value.                                        //
//                                                           //
// Description:                                              //
//                                                           //
//   The function will subtract a shifted copy of the data   //
//   array from itself. Processing will start from the end   //
//   such that data[size - 1] -= data[size - 1 - shift],     //
//   etc., until position data + shift is reached. The first //
//   shift elements will be left unchanged. NOTE that this   //
//   function will modify the input data array!              //
// --------------------------------------------------------- //

void shift_and_subtract_SFX(DATA_T *data, const size_t size, const size_t shift)
{
	for(DATA_T *ptr = data + size; ptr --> data + shift;) *ptr -= *(ptr - shift);
	return;
}



// --------------------------------------------------------- //
// Determine optimal boxcar filter size for Gaussian filter  //
// --------------------------------------------------------- //
//                                                           //
// Arguments:                                                //
//                                                           //
//   (1) sigma         - Standard deviation of the Gaussian. //
//   (2) filter_radius - Radius of the boxcar filter that is //
//                       to be used in approximation.        //
//   (3) n_iter        - Number of iteration of boxcar fil-  //
//                       tering to be used in approximation. //
//                                                           //
// Returns:                                                  //
//                                                           //
//   No return value.                                        //
//                                                           //
// Description:                                              //
//                                                           //
//   Based on the specified standard deviation of the Gauss- //
//   ian filter, this function will determine the optimal    //
//   radius and number of iterations for the boxcar filter   //
//   that is to be used to approximate the Gaussian kernel.  //
//   Both 'filter_radius' and 'n_iter' are pointers and will //
//   be set to the correct values by this function. They can //
//   then be fed into the Gaussian filtering function.       //
// --------------------------------------------------------- //

void optimal_filter_size_SFX(const double sigma, size_t *filter_radius, size_t *n_iter)
{
	*n_iter = 0;
	*filter_radius = 0;
	double tmp = -1.0;
	size_t i;
	
	for(i = BOXCAR_MIN_ITER; i <= BOXCAR_MAX_ITER; ++i)
	{
		const double radius = sqrt((3.0 * sigma * sigma / i) + 0.25) - 0.5;
		const double diff = fabs(radius - floor(radius + 0.5));
		
		if(tmp < 0.0 || diff < tmp)
		{
			tmp = diff;
			*n_iter = i;
			*filter_radius = (size_t)(radius + 0.5);
		}
	}
	
	// Print some information
	/*const double sigma_approx = sqrt((double)(*n_iter) * ((2.0 * (double)(*filter_radius) + 1.0) * (2.0 * (double)(*filter_radius) + 1.0) - 1.0) / 12.0);
	message("Requested filter size:    sigma = %.2f\n", sigma);
	message("Approximated filter size: sigma = %.2f\n", sigma_approx);
	message("  using N = %zu and R = %zu\n", *n_iter, *filter_radius);*/
	
	return;
}



// ----------------------------------------------------------------- //
// Fit ellipse to moment-0 map                                       //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) moment_map - Pointer to moment-0 map                        //
//   (2) count_map  - Pointer to map containing number of channels   //
//                    for each pixel in moment map.                  //
//   (3) size_x     - Size of moment map in x.                       //
//   (4) size_y     - Size of moment map in y.                       //
//   (5) centroid_x - x position of source centroid relative to      //
//                    moment map boundary.                           //
//   (6) centroid_y - y position of source centroid relative to      //
//                    moment map boundary.                           //
//   (7) rms        - RMS noise level of the data.                   //
//   (8) ell_maj    - Pointer to variable for ellipse major axis.    //
//   (9) ell_min    - Pointer to variable for ellipse minor axis.    //
//  (10) ell_pa     - Pointer to variable for ellipse pos. angle.    //
//  (11) ell3s_maj  - Same, but for 3-sigma ellipse.                 //
//  (12) ell3s_min  - Same, but for 3-sigma ellipse.                 //
//  (13) ell3s_pa   - Same, but for 3-sigma ellipse.                 //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Function for fitting an ellipse to the specified moment-zero    //
//   map. The moment and count map arrays must be contiguous in x.   //
//   The source centroid must be relative to the boundaries of the   //
//   moment map. The ellipse fit is carried out using 2nd-order mo-  //
//   ment analysis.                                                  //
//   Two types of ellipse are fitted: the first one is a fit to the  //
//   flux-weighted positive pixels in the moment maps, while the se- //
//   cond fit is to all pixels greater than 3 times the specified    //
///  RMS noise level with equal weights. The results will be written //
//   to the user-specified pointer variables.                        //
// ----------------------------------------------------------------- //

void moment_ellipse_fit_SFX(const DATA_T *moment_map, const size_t *count_map, const size_t size_x, const size_t size_y, const DATA_T centroid_x, const DATA_T centroid_y, const DATA_T rms, DATA_T *ell_maj, DATA_T *ell_min, DATA_T *ell_pa, DATA_T *ell3s_maj, DATA_T *ell3s_min, DATA_T *ell3s_pa)
{
	DATA_T ell_momX    = 0.0;
	DATA_T ell_momY    = 0.0;
	DATA_T ell_momXY   = 0.0;
	DATA_T ell_sum     = 0.0;
	DATA_T ell3s_momX  = 0.0;
	DATA_T ell3s_momY  = 0.0;
	DATA_T ell3s_momXY = 0.0;
	DATA_T ell3s_sum   = 0.0;
	
	for(size_t y = 0; y < size_y; ++y)
	{
		for(size_t x = 0; x < size_x; ++x)
		{
			DATA_T value = moment_map[x + size_x * y];
			size_t count = count_map [x + size_x * y];
			
			if(value > 0.0)
			{
				// Calculate moments for flux-weighted ellipse fitting
				// NOTE: Only positive pixels considered here!
				ell_momX  += ((DATA_T)x - centroid_x) * ((DATA_T)x - centroid_x) * value;
				ell_momY  += ((DATA_T)y - centroid_y) * ((DATA_T)y - centroid_y) * value;
				ell_momXY += ((DATA_T)x - centroid_x) * ((DATA_T)y - centroid_y) * value;
				ell_sum += value;
				
				if(value > 3.0 * rms * sqrt((DATA_T)count))
				{
					// Calculate moments for 3-sigma ellipse fitting
					ell3s_momX  += ((DATA_T)x - centroid_x) * ((DATA_T)x - centroid_x);
					ell3s_momY  += ((DATA_T)y - centroid_y) * ((DATA_T)y - centroid_y);
					ell3s_momXY += ((DATA_T)x - centroid_x) * ((DATA_T)y - centroid_y);
					ell3s_sum += 1.0;
				}
			}
		}
	}
	
	if(ell_sum > 0.0)
	{
		ell_momX  /= ell_sum;
		ell_momY  /= ell_sum;
		ell_momXY /= ell_sum;
		*ell_pa  = 0.5 * atan2(2.0 * ell_momXY, ell_momX - ell_momY);
		*ell_maj = sqrt(2.0 * (ell_momX + ell_momY + sqrt((ell_momX - ell_momY) * (ell_momX - ell_momY) + 4.0 * ell_momXY * ell_momXY)));
		*ell_min = sqrt(2.0 * (ell_momX + ell_momY - sqrt((ell_momX - ell_momY) * (ell_momX - ell_momY) + 4.0 * ell_momXY * ell_momXY)));
		*ell_pa  = *ell_pa   * 180.0 / M_PI - 90.0;
		while(*ell_pa < -90.0) *ell_pa += 180.0;
	}
	
	if(ell3s_sum > 0.0)
	{
		ell3s_momX  /= ell3s_sum;
		ell3s_momY  /= ell3s_sum;
		ell3s_momXY /= ell3s_sum;
		*ell3s_pa  = 0.5 * atan2(2.0 * ell3s_momXY, ell3s_momX - ell3s_momY);
		*ell3s_maj = sqrt(2.0 * (ell3s_momX + ell3s_momY + sqrt((ell3s_momX - ell3s_momY) * (ell3s_momX - ell3s_momY) + 4.0 * ell3s_momXY * ell3s_momXY)));
		*ell3s_min = sqrt(2.0 * (ell3s_momX + ell3s_momY - sqrt((ell3s_momX - ell3s_momY) * (ell3s_momX - ell3s_momY) + 4.0 * ell3s_momXY * ell3s_momXY)));
		*ell3s_pa = *ell3s_pa * 180.0 / M_PI - 90.0;
		while(*ell3s_pa < -90.0) *ell3s_pa += 180.0;
	}
	
	// NOTE: Converting PA from radians to degrees.
	// NOTE: Subtracting 90 deg from PA, because astronomers like to have 0 deg pointing up.
	// NOTE: This means that PA will no longer have the mathematically correct orientation!
	// NOTE: PA should then be between -90 deg (right) and +90 deg (left), with 0 deg pointing up.
	// WARNING: PA will be relative to the pixel grid, not the coordinate system!
	
	return;
}



// ----------------------------------------------------------------- //
// Determine w20 and w50 line widths from spectrum                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) spectrum   - Pointer to spectrum.                           //
//   (2) size       - Size of spectrum.                              //
//   (3) maximum    - Maximum (peak) of spectrum.                    //
//   (4) w20        - Pointer to variable for w20 line width.        //
//   (5) w50        - Pointer to variable for w50 line width.        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Function for measuring the w20 and w50 line width of the speci- //
//   fied spectrum. Both are measured by moving inwards from the     //
//   edges of the spectrum until the point where the flux density    //
//   increased above 20% (or 50%) of the peak flux density. Linear   //
//   interpolation is used to improved the accuracy of the measured  //
//   line widths. The results will be written to the user-specified  //
//   w20 and w50 pointers.                                           //
// ----------------------------------------------------------------- //

void spectral_line_width_SFX(const DATA_T *spectrum, const size_t size, DATA_T *w20, DATA_T *w50)
{
	// Auxiliary parameters
	size_t index;
	DATA_T maximum = -INFINITY;
	
	// Determine maximum
	for(size_t i = size; i--;) if(spectrum[i] > maximum) maximum = spectrum[i];
	
	// Determine w20 from spectrum (moving inwards)
	for(index = 0; index < size && spectrum[index] < 0.2 * maximum; ++index);
	if(index < size)
	{
		*w20 = (DATA_T)index;
		if(index > 0) *w20 -= (spectrum[index] - 0.2 * maximum) / (spectrum[index] - spectrum[index - 1]);
		for(index = size - 1; index < size && spectrum[index] < 0.2 * maximum; --index); // index is unsigned
		*w20 = (DATA_T)index - *w20;
		if(index < size - 1) *w20 += (spectrum[index] - 0.2 * maximum) / (spectrum[index] - spectrum[index + 1]);
	}
	else
	{
		*w20 = 0.0;
		warning("Failed to measure w20.");
	}
	
	// Determine w50 from spectrum (moving inwards)
	for(index = 0; index < size && spectrum[index] < 0.5 * maximum; ++index);
	if(index < size)
	{
		*w50 = (DATA_T)index;
		if(index > 0) *w50 -= (spectrum[index] - 0.5 * maximum) / (spectrum[index] - spectrum[index - 1]);
		for(index = size - 1; index < size && spectrum[index] < 0.5 * maximum; --index); // index is unsigned
		*w50 = (DATA_T)index - *w50;
		if(index < size - 1) *w50 += (spectrum[index] - 0.5 * maximum) / (spectrum[index] - spectrum[index + 1]);
	}
	else
	{
		*w50 = 0.0;
		warning("Failed to measure w50.");
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Determine kinematic major axis of galaxy                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) centroidX  - Array of x centroid positions per channel.     //
//   (2) centroidY  - Array of y centroid positions per channel.     //
//   (3) sum        - Array of summed flux densities per channel.    //
//   (4) size       - Size of centroidX, centroidY and sum arrays.   //
//   (5) first      - Index of first valid centroid position.        //
//   (6) last       - Index of last valid centroid position.         //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Position angle of kinematic major axis.                         //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Function for measuring the position angle of the kinematic      //
//   major axis of a galaxy from an array of flux-weighted centroid  //
//   measurement per spectral channel. The position angle is deter-  //
//   minedby fitting a straight line to the centroid positions using //
//   orthogonal (Deming) regression. Missing or invalid centroids in //
//   certain channels can be indicated by setting the corresponding  //
//   value in the sum array to zero. The resulting position angle    //
//   will point towards the side of the galaxy that sits at the up-  //
//   per end of the channel range occupied by the galaxy.            //
// ----------------------------------------------------------------- //

DATA_T kin_maj_axis_SFX(const DATA_T *centroidX, const DATA_T *centroidY, const DATA_T *sum, const size_t size, const size_t first, const size_t last)
{
	// Fit a straight line to the set of centroids (using user-defined weights)
	DATA_T sumW   = 0.0;
	DATA_T sumX   = 0.0;
	DATA_T sumY   = 0.0;
	DATA_T sumXX  = 0.0;
	DATA_T sumYY  = 0.0;
	DATA_T sumXY  = 0.0;
	DATA_T weight = 1.0;
	
	// Using Deming (orthogonal) regression:
	for(size_t i = 0; i < size; ++i)
	{
		if(sum[i] > 0.0)
		{
			// Set the desired weights here (defaults to 1 otherwise):
			weight = sum[i] * sum[i];
			
			sumW += weight;
			sumX += weight * centroidX[i];
			sumY += weight * centroidY[i];
		}
	}
	
	sumX /= sumW;
	sumY /= sumW;
	
	for(size_t i = 0; i < size; ++i)
	{
		if(sum[i] > 0.0)
		{
			// Set the desired weights here (defaults to 1 otherwise):
			weight = sum[i] * sum[i];
			
			sumXX += weight * (centroidX[i] - sumX) * (centroidX[i] - sumX);
			sumYY += weight * (centroidY[i] - sumY) * (centroidY[i] - sumY);
			sumXY += weight * (centroidX[i] - sumX) * (centroidY[i] - sumY);
		}
	}
	
	const DATA_T slope = (sumYY - sumXX + sqrt((sumYY - sumXX) * (sumYY - sumXX) + 4.0 * sumXY * sumXY)) / (2.0 * sumXY);
	//const DATA_T inter = sumY - slope * sumX;
	
	// Calculate position angle of kinematic major axis:
	DATA_T pa = atan(slope);
	
	// Check orientation of approaching/receding side of disc:
	DATA_T fullAngle = atan2(centroidY[last] - centroidY[first], centroidX[last] - centroidX[first]);
	
	// Correct for full angle and astronomers' favourite definition of PA:
	DATA_T difference = fabs(atan2(sin(fullAngle) * cos(pa) - cos(fullAngle) * sin(pa), cos(fullAngle) * cos(pa) + sin(fullAngle) * sin(pa)));
	if(difference > M_PI / 2.0)
	{
		pa += M_PI;
		difference -= M_PI;
	}
	
	//if(fabs(difference) > M_PI / 6.0) flagWarp = true;     // WARNING: Warping angle of 30° hard-coded here!
	
	pa = 180.0 * pa / M_PI - 90.0;
	while(pa <    0.0) pa += 360.0;
	while(pa >= 360.0) pa -= 360.0;
	// NOTE: PA should now be between 0° (pointing up) and 360° and refer to the side of the
	//       disc that occupies the upper end of the channel range covered by the source.
	//       PAs are relative to the coordinate system imposed by the centroid arrays.
	
	return pa;
}
