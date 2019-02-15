/*
  Copyright (C) 2019  Equinor ASA, Norway.
  This file  is part of ERT - Ensemble based Reservoir Tool.

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


#include <stdexcept>
#include <string>
#include <array>
#include <vector>


#include <ert/res_util/matrix.hpp>

#include <ert/res_util/es_testdata.hpp>

#define ROW_MAJOR_STORAGE true

namespace res {

namespace {

class pushd {
public:
  pushd(const std::string& path, bool mkdir = false) {
    if (!util_is_directory(path.c_str())) {
      if (mkdir)
        util_make_path(path.c_str());
    }

    if (!util_is_directory(path.c_str()))
      throw std::invalid_argument("The path: " + path + " does not exist - can not proceed");

    this->org_cwd = util_alloc_cwd();
    util_chdir(path.c_str());
  }


  ~pushd() {
    util_chdir(this->org_cwd);
    free(this->org_cwd);
  }

private:
  char * org_cwd;
};

matrix_type * alloc_load(const std::string& name, int rows, int columns) {
  if (!util_file_exists(name.c_str()))
    return NULL;

  FILE * stream = util_fopen(name.c_str(), "r");
  matrix_type * m = matrix_alloc(rows, columns);
  matrix_fscanf_data(m, ROW_MAJOR_STORAGE, stream);
  fclose(stream);

  return m;
}

void save_matrix_data(const std::string& name, const matrix_type * m) {
  FILE * stream = util_fopen(name.c_str(), "w");
  matrix_fprintf_data(m, ROW_MAJOR_STORAGE, stream);
  fclose(stream);
}


std::array<int,2> load_size() {
  int active_ens_size, active_obs_size;

  FILE * stream = fopen("size", "r");
  fscanf(stream, "%d %d", &active_ens_size, &active_obs_size);
  fclose(stream);

  return {active_ens_size, active_obs_size};
}

void save_size(int ens_size, int obs_size) {
  FILE * stream = util_fopen("size", "w");
  fprintf(stream, "%d %d\n", ens_size, obs_size);
  fclose(stream);
}

matrix_type * safe_copy(const matrix_type * m) {
  if (m)
    return matrix_alloc_copy(m);

  return nullptr;
}


matrix_type * matrix_delete_column(const matrix_type * m1, int column) {
  matrix_type * m2 = matrix_alloc(matrix_get_rows(m1), matrix_get_columns(m1) - 1);
  if (column > 0)
    matrix_copy_block(m2, 0, 0,
                      matrix_get_rows(m2), column,
                      m1, 0, 0);

  if (column < (matrix_get_columns(m1) - 1))
    matrix_copy_block(m2, 0, column,
                      matrix_get_rows(m2), matrix_get_columns(m2) - column,
                      m1, 0, column + 1);

  return m2;
}


matrix_type * matrix_delete_row(const matrix_type * m1, int row) {
  matrix_type * m2 = matrix_alloc(matrix_get_rows(m1) - 1, matrix_get_columns(m1));
  if (row > 0)
    matrix_copy_block(m2, 0, 0,
                      row, matrix_get_columns(m2),
                      m1, 0, 0);

  if (row < (matrix_get_rows(m1) - 1))
    matrix_copy_block(m2, row, 0,
                      matrix_get_rows(m2) - row, matrix_get_columns(m2),
                      m1, row + 1, 0);

  return m2;
}

matrix_type * matrix_delete_row_column(const matrix_type * m1, int row_column) {
  matrix_type * m2 = matrix_delete_column(m1, row_column);
  matrix_type * m3 = matrix_delete_row(m2, row_column);
  matrix_free(m2);
  return m3;
}


matrix_type * swap_matrix(matrix_type * old_matrix, matrix_type * new_matrix) {
  matrix_free( old_matrix );
  return new_matrix;
}


}





matrix_type * es_testdata::alloc_matrix(const std::string& fname, int rows, int columns) const {
  pushd tmp_path(this->path);

  matrix_type * m = alloc_load(fname, rows, columns);
  return m;
}


void es_testdata::save_matrix(const std::string& name, const matrix_type * m) const {
  pushd tmp_path(this->path);

  FILE * stream = util_fopen(name.c_str(), "w");
  matrix_fprintf_data(m, ROW_MAJOR_STORAGE, stream);
  fclose(stream);
}

es_testdata::es_testdata(const matrix_type* S, const matrix_type * R, const matrix_type * dObs, const matrix_type *D , const matrix_type * E)
  : S(safe_copy(S)),
    R(safe_copy(R)),
    dObs(safe_copy(dObs)),
    D(safe_copy(D)),
    E(safe_copy(E)),
    active_ens_size(matrix_get_columns(S)),
    active_obs_size(matrix_get_rows(S)),
    obs_mask(bool_vector_alloc(active_obs_size, true)),
    ens_mask(bool_vector_alloc(active_ens_size, true))
{
}

es_testdata::es_testdata(const char * path, const matrix_type* S, const matrix_type * R, const matrix_type * dObs, const matrix_type *D , const matrix_type * E)
    : path(path),
      S(safe_copy(S)),
      R(safe_copy(R)),
      dObs(safe_copy(dObs)),
      D(safe_copy(D)),
      E(safe_copy(E)),
      active_ens_size(matrix_get_columns(S)),
      active_obs_size(matrix_get_rows(S))
  {
  }

void es_testdata::deactivate_obs(int iobs) {
    if (iobs >= bool_vector_size( this->obs_mask ))
        throw std::invalid_argument("Obs number: " + std::to_string(iobs) + " out of reach");

    if (bool_vector_iget(this->obs_mask, iobs)) {
        bool_vector_iset(this->obs_mask, iobs, false);

        this->dObs = swap_matrix( this->dObs, matrix_delete_row(this->dObs, iobs));
        this->S = swap_matrix( this->S, matrix_delete_row(this->S, iobs));
        this->R = swap_matrix( this->R, matrix_delete_row_column(this->R, iobs));

        if (this->E)
          this->E = swap_matrix( this->E , matrix_delete_row(this->E, iobs));

        if (this->D)
          this->D = swap_matrix( this->D, matrix_delete_row(this->D, iobs));

        this->active_obs_size -= 1;
    }
}

void es_testdata::deactivate_realization(int iens) {
  if (iens >= bool_vector_size( this->ens_mask ))
    throw std::invalid_argument("iRealization number: " + std::to_string(iens) + " out of reach");

  if (bool_vector_iget(this->ens_mask, iens)) {
    bool_vector_iset(this->ens_mask, iens, false);

    this->S = swap_matrix( this->S, matrix_delete_column(this->S, iens));

    if (this->E)
      this->E = swap_matrix( this->E , matrix_delete_column(this->E, iens));

    if (this->D)
      this->D = swap_matrix( this->D, matrix_delete_column(this->D, iens));

    this->active_ens_size -= 1;
  }
}

es_testdata::es_testdata(const char * path) :
  path(path),
  S(nullptr),
  E(nullptr),
  R(nullptr),
  D(nullptr),
  dObs(nullptr)
{
  pushd tmp_path(this->path);

  auto size = load_size();
  this->active_ens_size = size[0];
  this->active_obs_size = size[1];

  this->S = alloc_load("S", this->active_obs_size, this->active_ens_size);
  this->E = alloc_load("E", this->active_obs_size, this->active_ens_size);
  this->R = alloc_load("R", this->active_obs_size, this->active_obs_size);
  this->D = alloc_load("D", this->active_obs_size, this->active_ens_size);
  this->dObs = alloc_load("dObs", this->active_obs_size, 2);
  this->obs_mask = bool_vector_alloc(this->active_obs_size, true);
  this->ens_mask = bool_vector_alloc(this->active_ens_size, true);
}


es_testdata::~es_testdata() {
  if (this->S)
    matrix_free(this->S);

  if (this->E)
    matrix_free(this->E);

  if (this->R)
    matrix_free(this->R);

  if (this->D)
    matrix_free(this->D);

  if (this->dObs)
    matrix_free(this->dObs);

  bool_vector_free(this->obs_mask);
  bool_vector_free(this->ens_mask);
}


void es_testdata::save(const std::string& path) const {
  pushd tmp_path(path, true);
  save_size(this->active_ens_size, this->active_obs_size);

  if (this->S)
    save_matrix_data("S", S);

  if (this->E)
    save_matrix_data("E", E);

  if (this->R)
    save_matrix_data("R", R);

  if (this->D)
    save_matrix_data("D", D);

  if (this->dObs)
    save_matrix_data("dObs", dObs);
}

void es_testdata::save() const {
  this->save(this->path);
}

/*
  This function will allocate a matrix based on data found on disk. The data on
  disk is only the actual content of the matrix, in row_major order. Before the
  matrix is constructed it is verified that the number of elements is a multiple
  of this->active_ens_size.
*/

matrix_type * es_testdata::alloc_state(const std::string& name) const {
  std::vector<double> data;
  {
    pushd tmp_path(this->path);
    FILE * stream = fopen(name.c_str(), "r");
    if (!stream)
      throw std::invalid_argument("No such state matrix: " + this->path + "/" + name);

    while (true) {
      double value;
      int read_count = fscanf(stream, "%lg", &value);
      if (read_count == 1)
        data.push_back(value);
      else
        break;
    }

    fclose(stream);
  }

  if ((data.size() % this->active_ens_size) != 0)
    throw std::invalid_argument("Number of elements in file with state informaton must be a multiple of ensemble_size: " + std::to_string(this->active_ens_size));

  int state_size = data.size() / this->active_ens_size;
  matrix_type * state = matrix_alloc(state_size, this->active_ens_size);
  for (int is=0; is < state_size; is++) {
    for (int iens=0; iens < this->active_ens_size; iens++) {
      matrix_iset(state, is, iens, data[ iens + is * this->active_ens_size ]);
    }
  }

  return state;
}

}
