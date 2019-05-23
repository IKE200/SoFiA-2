/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Array_siz.h) - Source Finding Application          ///
/// Copyright (C) 2019 Tobias Westmeier                                  ///
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

#ifndef ARRAY_siz_H
#define ARRAY_siz_H

#include "common.h"


// ----------------------------------------------------------------- //
// Class 'Array_siz'                                                 //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a convenient way to store //
// multiple values of a specific type in an array-like structure. A  //
// new array can either be of a given size and empty (using the      //
// standard constructor) or provided with a list of comma-separated  //
// values that will be stored in the array and used to determine its //
// size using the alternative constructor).                          //
// ----------------------------------------------------------------- //

typedef CLASS Array_siz Array_siz;

// Constructor and destructor
PUBLIC Array_siz    *Array_siz_new      (const size_t size);
PUBLIC Array_siz    *Array_siz_new_str  (const char *string);
PUBLIC Array_siz    *Array_siz_copy     (const Array_siz *source);
PUBLIC void          Array_siz_delete   (Array_siz *self);

// Methods
PUBLIC const size_t *Array_siz_get_ptr  (const Array_siz *self);
PUBLIC size_t        Array_siz_get_size (const Array_siz *self);
PUBLIC void          Array_siz_push     (Array_siz *self, const size_t value);
PUBLIC size_t        Array_siz_get      (const Array_siz *self, const size_t index);
PUBLIC void          Array_siz_set      (Array_siz *self, const size_t index, const size_t value);
PUBLIC void          Array_siz_add      (Array_siz *self, const size_t index, const size_t value);

#endif
