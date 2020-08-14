/*
   Copyright (C) 2012  Equinor ASA, Norway.

   The file 'job_lsf_test.c' is part of ERT - Ensemble based Reservoir Tool.

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
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <ert/util/test_work_area.hpp>
#include <ert/util/test_util.hpp>
#include <ert/job_queue/torque_driver.hpp>
#include <ert/util/util.hpp>

void test_option(torque_driver_type * driver, const char * option, const char * value) {
  test_assert_true(torque_driver_set_option(driver, option, value));
  test_assert_string_equal((const char *) torque_driver_get_option(driver, option), value);
}

void setoption_setalloptions_optionsset() {
  torque_driver_type * driver = (torque_driver_type *) torque_driver_alloc();

  test_option(driver, TORQUE_QSUB_CMD, "XYZaaa");
  test_option(driver, TORQUE_QSTAT_CMD, "xyZfff");
  test_option(driver, TORQUE_QDEL_CMD, "ZZyfff");
  test_option(driver, TORQUE_QUEUE, "superhigh");
  test_option(driver, TORQUE_NUM_CPUS_PER_NODE, "42");
  test_option(driver, TORQUE_NUM_NODES, "36");
  test_option(driver, TORQUE_KEEP_QSUB_OUTPUT, "1");
  test_option(driver, TORQUE_KEEP_QSUB_OUTPUT, "0");
  test_option(driver, TORQUE_CLUSTER_LABEL, "thecluster");
  test_option(driver, TORQUE_JOB_PREFIX_KEY, "coolJob");

  test_assert_int_equal( 0 , torque_driver_get_submit_sleep(driver));
  test_assert_NULL( torque_driver_get_debug_stream(driver) );

  test_assert_true( torque_driver_set_option( driver , TORQUE_SUBMIT_SLEEP , "0.25"));
  test_assert_int_equal( 250000 , torque_driver_get_submit_sleep(driver));

  char tmp_path[] = "/tmp/torque_debug_XXXXXX";
  // We do not strictly need the file, we are only interested in a path name
  // tmpnam is however deprecated in favor of mkstemp, and no good substitute
  // for tmpnam (with similar functionality) was found.
  int fd = mkstemp(tmp_path);

  if (fd == -1) {
    printf("Unable to create dummy log file");
    exit(1);
  }

  close(fd);
  unlink(tmp_path);

  test_assert_true( torque_driver_set_option( driver , TORQUE_DEBUG_OUTPUT , tmp_path));
  test_assert_not_NULL( torque_driver_get_debug_stream(driver) );

  test_option(driver, TORQUE_QSUB_CMD      , NULL);
  test_option(driver, TORQUE_QSTAT_CMD     , NULL);
  test_option(driver, TORQUE_QDEL_CMD      , NULL);
  test_option(driver, TORQUE_QUEUE         , NULL);
  test_option(driver, TORQUE_CLUSTER_LABEL , NULL);
  test_option(driver, TORQUE_JOB_PREFIX_KEY, NULL);

  // Setting NULL to numerical options should leave the value unchanged
  torque_driver_set_option(driver, TORQUE_NUM_CPUS_PER_NODE, NULL);
  torque_driver_set_option(driver, TORQUE_NUM_NODES        , NULL);
  torque_driver_set_option(driver, TORQUE_KEEP_QSUB_OUTPUT , NULL);
  test_assert_string_equal( (const char *) torque_driver_get_option( driver, TORQUE_NUM_CPUS_PER_NODE ), "42");
  test_assert_string_equal( (const char *) torque_driver_get_option( driver, TORQUE_NUM_NODES         ), "36");
  test_assert_string_equal( (const char *) torque_driver_get_option( driver, TORQUE_KEEP_QSUB_OUTPUT  ), "0");

  torque_driver_free(driver);
}

void setoption_set_typed_options_wrong_format_returns_false() {
  torque_driver_type * driver = (torque_driver_type *) torque_driver_alloc();
  test_assert_false(torque_driver_set_option(driver, TORQUE_NUM_CPUS_PER_NODE, "42.2"));
  test_assert_false(torque_driver_set_option(driver, TORQUE_NUM_CPUS_PER_NODE, "fire"));
  test_assert_false(torque_driver_set_option(driver, TORQUE_NUM_NODES, "42.2"));
  test_assert_false(torque_driver_set_option(driver, TORQUE_NUM_NODES, "fire"));
  test_assert_true(torque_driver_set_option(driver, TORQUE_KEEP_QSUB_OUTPUT, "true"));
  test_assert_true(torque_driver_set_option(driver, TORQUE_KEEP_QSUB_OUTPUT, "1"));
  test_assert_false(torque_driver_set_option(driver, TORQUE_KEEP_QSUB_OUTPUT, "ja"));
  test_assert_false(torque_driver_set_option(driver, TORQUE_KEEP_QSUB_OUTPUT, "22"));
  test_assert_false(torque_driver_set_option(driver, TORQUE_KEEP_QSUB_OUTPUT, "1.1"));
  test_assert_false(torque_driver_set_option(driver, TORQUE_SUBMIT_SLEEP, "X45"));
}

void getoption_nooptionsset_defaultoptionsreturned() {
  torque_driver_type * driver = (torque_driver_type *) torque_driver_alloc();
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_QSUB_CMD), TORQUE_DEFAULT_QSUB_CMD);
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_QSTAT_CMD), TORQUE_DEFAULT_QSTAT_CMD);
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_QDEL_CMD), TORQUE_DEFAULT_QDEL_CMD);
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_KEEP_QSUB_OUTPUT), "0");
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_NUM_CPUS_PER_NODE), "1");
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_NUM_NODES), "1");
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_CLUSTER_LABEL), NULL );
  test_assert_string_equal((const char *) torque_driver_get_option(driver, TORQUE_JOB_PREFIX_KEY), NULL);

  printf("Default options OK\n");
  torque_driver_free(driver);
}

void create_submit_script_script_according_to_input() {
  ecl::util::TestArea ta("submit_script");
  const char * script_filename = "qsub_script.sh";

  {
    const char ** args = (const char **) util_calloc(2, sizeof * args);
    args[0] = "/tmp/jaja/";
    args[1] = "number2arg";
    torque_job_create_submit_script(script_filename, "job_program.py", 2, args);
    free( args );
  }

  {
    FILE* file_stream = util_fopen(script_filename, "r");
    bool at_eof = false;

    char * line = util_fscanf_alloc_line(file_stream, &at_eof);
    test_assert_string_equal("#!/bin/sh", line);
    free(line);

    line = util_fscanf_alloc_line(file_stream, &at_eof);
    test_assert_string_equal("job_program.py /tmp/jaja/ number2arg", line);
    free(line);

    line = util_fscanf_alloc_line(file_stream, &at_eof);
    free(line);
    test_assert_true(at_eof);

    fclose(file_stream);
  }
}



void test_parse_invalid( ) {
  test_assert_int_equal( torque_driver_parse_status( "/file/does/not/exist" , NULL) , JOB_QUEUE_STATUS_FAILURE);
  {
    ecl::util::TestArea ta("submit");
    {
      FILE * stream = util_fopen("qstat.stdout", "w");
      fclose( stream );
    }
    test_assert_int_equal( torque_driver_parse_status( "qstat.stdout" , "a2345") , JOB_QUEUE_STATUS_FAILURE);
  }
}


int main(int argc, char ** argv) {
  getoption_nooptionsset_defaultoptionsreturned();
  setoption_setalloptions_optionsset();

  setoption_set_typed_options_wrong_format_returns_false();
  create_submit_script_script_according_to_input();
  test_parse_invalid( );
  exit(0);
}
