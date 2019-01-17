/*
   Copyright (C) 2015  Statoil ASA, Norway.

   The file 'ies_enkf_config.h' is part of ERT - Ensemble based Reservoir Tool.

   ERT is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ERT is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
   for more details.
*/

#ifndef IES_ENKF_CONFIG_H
#define IES_ENKF_CONFIG_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
  IES_INVERSION_EXACT = 0,
  IES_INVERSION_SUBSPACE_EXACT_R = 1,
  IES_INVERSION_SUBSPACE_EE_R = 2,
  IES_INVERSION_SUBSPACE_RE = 3
} ies_inversion_type;


typedef struct ies_enkf_config_struct ies_enkf_config_type;


  ies_enkf_config_type * ies_enkf_config_alloc();
  void   ies_enkf_config_free(ies_enkf_config_type * config);

  int    ies_enkf_config_get_enkf_subspace_dimension( ies_enkf_config_type * config );
  void   ies_enkf_config_set_enkf_subspace_dimension( ies_enkf_config_type * config , int subspace_dimension);

  double ies_enkf_config_get_truncation( ies_enkf_config_type * config );
  void   ies_enkf_config_set_truncation( ies_enkf_config_type * config , double truncation);

  void ies_enkf_config_set_option_flags( ies_enkf_config_type * config , long flags);
  long ies_enkf_config_get_option_flags( const ies_enkf_config_type * config );

  double ies_enkf_config_get_ies_steplength( ies_enkf_config_type * config );
  void   ies_enkf_config_set_ies_steplength( ies_enkf_config_type * config , double ies_steplength);

  double ies_enkf_config_get_gauss_newton_conv( ies_enkf_config_type * config );
  void   ies_enkf_config_set_gauss_newton_conv( ies_enkf_config_type * config , double gauss_newton_conv);

  int    ies_enkf_config_get_max_gauss_newton_it( ies_enkf_config_type * config );
  void   ies_enkf_config_set_max_gauss_newton_it( ies_enkf_config_type * config , int max_gauss_newton_it);

  ies_inversion_type ies_enkf_config_get_ies_inversion( ies_enkf_config_type * config ) ;
  void   ies_enkf_config_set_ies_inversion( ies_enkf_config_type * config , ies_inversion_type ies_inversion ) ;

  bool   ies_enkf_config_get_ies_subspace( ies_enkf_config_type * config ) ;
  void   ies_enkf_config_set_ies_subspace( ies_enkf_config_type * config , bool ies_subspace ) ;

  bool   ies_enkf_config_get_ies_debug( ies_enkf_config_type * config ) ;
  void   ies_enkf_config_set_ies_debug( ies_enkf_config_type * config , bool ies_debug ) ;

  char * ies_enkf_config_get_ies_logfile( ies_enkf_config_type * config ) ;
  void   ies_enkf_config_set_ies_logfile( ies_enkf_config_type * config , const char * ies_logfile ) ;


#ifdef __cplusplus
}
#endif
#endif
