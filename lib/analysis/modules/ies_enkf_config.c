/*
   Copyright (C) 2015  Statoil ASA, Norway.

   The file 'ies_enkf_config.c' is part of ERT - Ensemble based Reservoir Tool.

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


#include <ert/util/util.hpp>
#include <ert/util/type_macros.hpp>

#include <ert/analysis/std_enkf.hpp>
#include <ert/analysis/analysis_module.hpp>

#include <ies_enkf_config.h>




#define INVALID_SUBSPACE_DIMENSION     -1
#define INVALID_TRUNCATION             -1
#define DEFAULT_ENKF_TRUNCATION        0.98
#define DEFAULT_ENKF_SUBSPACE_DIMENSION     INVALID_SUBSPACE_DIMENSION

#define DEFAULT_IES_STEPLENGTH         0.70
#define DEFAULT_GAUSS_NEWTON_CONV      0.0001

#define DEFAULT_IES_SUBSPACE           false
#define DEFAULT_IES_INVERSION               1
#define DEFAULT_IES_LOGFILE            "ies.log"
#define DEFAULT_IES_DEBUG              false



#define IES_ENKF_CONFIG_TYPE_ID 196402021

struct ies_enkf_config_struct {
  UTIL_TYPE_ID_DECLARATION;
  double    truncation;            // Controlled by config key: ENKF_TRUNCATION_KEY
  int       subspace_dimension;    // Controlled by config key: ENKF_SUBSPACE_DIMENSION_KEY (-1: use Truncation instead)
  long      option_flags;
  double    ies_steplength;        // Controlled by config key: DEFAULT_IES_STEPLENGTH_KEY
  double    gauss_newton_conv;     // Controlled by config key: DEFAULT_GAUSS_NEWTON_CONV_KEY
  bool      ies_subspace;          // Controlled by config key: DEFAULT_IES_SUBSPACE
  int       ies_inversion;              // Controlled by config key: DEFAULT_IES_INVERSION
  char    * ies_logfile;           // Controlled by config key: DEFAULT_IES_LOGFILE
  bool      ies_debug;             // Controlled by config key: DEFAULT_IES_DEBUG
};


ies_enkf_config_type * ies_enkf_config_alloc() {
  ies_enkf_config_type * config = util_malloc( sizeof * config );
  UTIL_TYPE_ID_INIT( config , IES_ENKF_CONFIG_TYPE_ID );
  config->ies_logfile = NULL;
  ies_enkf_config_set_truncation( config , DEFAULT_ENKF_TRUNCATION);
  ies_enkf_config_set_enkf_subspace_dimension( config , DEFAULT_ENKF_SUBSPACE_DIMENSION);
  ies_enkf_config_set_option_flags( config , ANALYSIS_NEED_ED + ANALYSIS_UPDATE_A + ANALYSIS_ITERABLE + ANALYSIS_SCALE_DATA);
  ies_enkf_config_set_ies_steplength( config , DEFAULT_IES_STEPLENGTH );
  ies_enkf_config_set_gauss_newton_conv( config , DEFAULT_GAUSS_NEWTON_CONV );
  ies_enkf_config_set_ies_subspace( config , DEFAULT_IES_SUBSPACE );
  ies_enkf_config_set_ies_inversion( config , DEFAULT_IES_INVERSION );
  ies_enkf_config_set_ies_logfile( config , DEFAULT_IES_LOGFILE );
  ies_enkf_config_set_ies_debug( config , DEFAULT_IES_DEBUG );

  return config;
}

/*------------------------------------------------------------------------------------------------*/
/* TRUNCATION -> SUBSPACE_DIMENSION */
double ies_enkf_config_get_truncation( ies_enkf_config_type * config ) {
  return config->truncation;
}

void ies_enkf_config_set_truncation( ies_enkf_config_type * config , double truncation) {
  config->truncation = truncation;
  if (truncation > 0.0)
    config->subspace_dimension = INVALID_SUBSPACE_DIMENSION;
}

/*------------------------------------------------------------------------------------------------*/
/* SUBSPACE_DIMENSION -> TRUNCATION */
int ies_enkf_config_get_enkf_subspace_dimension( ies_enkf_config_type * config ) {
  return config->subspace_dimension;
}

void ies_enkf_config_set_enkf_subspace_dimension( ies_enkf_config_type * config , int subspace_dimension) {
  config->subspace_dimension = subspace_dimension;
  if (subspace_dimension > 0)
    config->truncation = INVALID_TRUNCATION;
}

/*------------------------------------------------------------------------------------------------*/
/* OPTION_FLAGS */

long ies_enkf_config_get_option_flags( const ies_enkf_config_type * config ) {
  return config->option_flags;
}

void ies_enkf_config_set_option_flags( ies_enkf_config_type * config , long flags) {
  config->option_flags = flags;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_STEPLENGTH */
double ies_enkf_config_get_ies_steplength( ies_enkf_config_type * config ) {
   return config->ies_steplength;
}
void ies_enkf_config_set_ies_steplength( ies_enkf_config_type * config , double ies_steplength) {
   config->ies_steplength = ies_steplength;
}

/*------------------------------------------------------------------------------------------------*/
/* GAUSS_NEWTON_CONV */
double ies_enkf_config_get_gauss_newton_conv( ies_enkf_config_type * config ) {
   return config->gauss_newton_conv;
}
void ies_enkf_config_set_gauss_newton_conv( ies_enkf_config_type * config , double gauss_newton_conv ) {
   config->gauss_newton_conv = gauss_newton_conv;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_INVERSION          */
int ies_enkf_config_get_ies_inversion( ies_enkf_config_type * config ) {
   return config->ies_inversion;
}
void ies_enkf_config_set_ies_inversion( ies_enkf_config_type * config , int ies_inversion ) {
   config->ies_inversion = ies_inversion;
}


/*------------------------------------------------------------------------------------------------*/
/* IES_SUBSPACE      */
bool ies_enkf_config_get_ies_subspace( ies_enkf_config_type * config ) {
   return config->ies_subspace;
}
void ies_enkf_config_set_ies_subspace( ies_enkf_config_type * config , bool ies_subspace ) {
   config->ies_subspace = ies_subspace;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_DEBUG         */
bool ies_enkf_config_get_ies_debug( ies_enkf_config_type * config ) {
   return config->ies_debug;
}
void ies_enkf_config_set_ies_debug( ies_enkf_config_type * config , bool ies_debug ) {
   config->ies_debug = ies_debug;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_LOGFILE       */
char * ies_enkf_config_get_ies_logfile( ies_enkf_config_type * config ) {
   return config->ies_logfile;
}
void ies_enkf_config_set_ies_logfile( ies_enkf_config_type * config , const char * ies_logfile ) {
   config->ies_logfile = util_realloc_string_copy( config->ies_logfile , ies_logfile );
//   config->ies_logfile = util_alloc_string_copy( ies_logfile );
}

/*------------------------------------------------------------------------------------------------*/
/* FREE_CONFIG */
void ies_enkf_config_free(ies_enkf_config_type * config) {
  free( config );
}
