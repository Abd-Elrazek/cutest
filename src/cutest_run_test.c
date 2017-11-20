/*
 * This is the test-suite for the cutest_run program. Please take a look at it
 * for educational purposes on how to use this testing framework.
 */

#include "cutest.h"

#include "cutest_run.h"

/* For convenience 'm' is shorter to write than 'cutest_mock' */
#define m cutest_mock
/* The main() function is by default renamed but can be renamed again */
#define main MAIN

/*****************************************************************************
 * usage()
 */
test(usage_shall_print_something)
{
  usage("some_program_name");
  assert_eq(1, m.printf.call_count);
}

/*****************************************************************************
 * replace_last_parenthesis_with_0()
 */
module_test(replace_last_parenthesis_with_0_shall_replace_last_parenthesis)
{
  char buf[27];
  strcpy(buf, "this is a string with a ) ");
  replace_last_parenthesis_with_0(buf, 0);
  assert_eq("this is a string with a ", buf);
}

module_test(replace_last_parenthesis_with_0_shall_print_an_error_of_not_found)
{
  m.fprintf.func = NULL;
  char* buf = "this is a string with no parenthesis ";
  replace_last_parenthesis_with_0(buf, 0);
  assert_eq(1, m.fprintf.call_count);
  assert_eq(stderr, m.fprintf.args.arg0)
}

/*****************************************************************************
 * skip_comments()
 */
test(skip_comments_shall_return_a_positive_count_of_started_block_comments)
{
  m.strlen.func = strlen;
  assert_eq(2, skip_comments("/* /*"));
}

test(skip_comments_shall_return_a_negative_count_of_started_block_comments)
{
  m.strlen.func = strlen;
  assert_eq(2, skip_comments("*/ */"));
}

test(skip_comments_shall_return_a_0_count_of_balanced_comment_block)
{
  m.strlen.func = strlen;
  assert_eq(0, skip_comments("/* */"));
}

/*****************************************************************************
 * next_test()
 */
test(next_test_shall_check_for_test_macros_correctly)
{
  m.strlen.func = strlen;
  m.strncmp.retval = 0;

  char buf[80];

  strcpy(buf, "test(tjosan)");

  next_test(buf);

  assert_eq(1, m.strncmp.call_count);
  assert_eq(buf, m.strncmp.args.arg0);
  assert_eq("test(", m.strncmp.args.arg1);
  assert_eq(strlen("test("), m.strncmp.args.arg2);
}

test(next_test_shall_check_for_module_test_macros_correctly)
{
  m.strlen.func = strlen;
  m.strncmp.retval = 1;

  char buf[80];

  strcpy(buf, "module_test(tjosan)");

  next_test(buf);

  assert_eq(2, m.strncmp.call_count);
  assert_eq(buf, m.strncmp.args.arg0);
  assert_eq("module_test(", m.strncmp.args.arg1);
  assert_eq(strlen("module_test("), m.strncmp.args.arg2);
}

module_test(next_test_shall_return_test_name_for_test_macros)
{
  char buf[80];

  strcpy(buf, "test(tjosan)");

  struct test_s r = next_test(buf);

  assert_eq("tjosan", r.name);
  assert_eq(0, r.reset_mocks);
}

module_test(next_test_shall_return_test_name_for_module_test_macros)
{
  char buf[80];

  strcpy(buf, "module_test(tjosan)");

  struct test_s r = next_test(buf);

  assert_eq("tjosan", r.name);
  assert_eq(1, r.reset_mocks);
}

/*****************************************************************************
 * print_header()
 */
test(print_header_shall_print_something)
{
  print_header("foo", "bar", "mii");
  assert_eq(1, m.printf.call_count);
}

/*****************************************************************************
 * print_main_function_prologue()
 */
test(print_main_function_prologue_shall_print_something)
{
  print_main_function_prologue("foo");
  assert_eq(1, m.printf.call_count);
}

/*****************************************************************************
 * print_test_case_executor()
 */
test(print_test_case_executor_shall_print_something)
{
  print_test_case_executor("foo", "bar");
  assert_eq(1, m.printf.call_count);
}

/*****************************************************************************
 * print_main_function_epilogue()
 */
test(print_main_function_epilogue_shall_print_something)
{
  print_main_function_epilogue("foo");
  assert_eq(1, m.printf.call_count);
}

/*****************************************************************************
 * print_test_case_executions()
 */
test(print_test_case_executions_shall_open_the_correct_file_for_reading)
{
  m.feof.retval = 1;
  print_test_case_executions("some_file");
  assert_eq("some_file", m.fopen.args.arg0);
  assert_eq("r", m.fopen.args.arg1);
}

test(print_test_case_executions_shall_call_feof_correctly)
{
  m.fopen.retval = 0x1234;
  m.feof.retval = 1;
  print_test_case_executions("some_file");
  assert_eq(1, m.feof.call_count);
  assert_eq(0x1234, m.feof.args.arg0);
}

test(print_test_case_executions_shall_close_the_correct_file)
{
  m.fopen.retval = 0x1234;
  m.feof.retval = 1;
  print_test_case_executions("some_file");
  assert_eq(1, m.fclose.call_count);
  assert_eq(0x1234, m.fclose.args.arg0);
}

test(print_test_case_executions_shall_call_fgets_correctly)
{
  m.fopen.retval = 0x1234;
  m.feof.retval = 0;
  print_test_case_executions("some_file");
  assert_eq(1, m.fgets.call_count);
  assert_eq(0x1234, m.fgets.args.arg2);
}

static int feof_cnt = 0;
static int feof_stub(FILE *stream)
{
  (void)stream;
  feof_cnt++;
  if (feof_cnt < 10) {
    return 0;
  }
  return 1;
}

test(print_test_case_executions_shall_read_file_rows_until_eof)
{
  m.feof.func = feof_stub;
  m.fgets.retval = 0x1234;
  feof_cnt = 0;
  print_test_case_executions("some_file");
  feof_cnt = 0;
  assert_eq(10, m.feof.call_count);
  assert_eq(9, m.fgets.call_count);
}

test(print_test_case_executions_shall_quit_reading_file_if_fgets_fails)
{
  m.feof.func = feof_stub;
  m.fgets.retval = 0;
  feof_cnt = 0;
  print_test_case_executions("some_file");
  feof_cnt = 0;
  assert_eq(1, m.feof.call_count);
  assert_eq(1, m.fgets.call_count);
}

test(print_test_case_executions_shall_call_skip_comments_correctly)
{
  m.feof.func = feof_stub;
  m.fgets.retval = 0x1234;
  feof_cnt = 0;
  print_test_case_executions("some_file");
  feof_cnt = 0;
  assert_eq(9, m.skip_comments.call_count);
}

test(print_test_case_executions_shall_not_read_next_test_or_print_if_comment)
{
  m.feof.func = feof_stub;
  m.fgets.retval = 0x1234;
  m.skip_comments.retval = 1;
  feof_cnt = 0;
  print_test_case_executions("some_file");
  feof_cnt = 0;
  assert_eq(9, m.skip_comments.call_count);
  assert_eq(0, m.next_test.call_count);
  assert_eq(0, m.print_test_case_executor.call_count);
}

test(print_test_case_executions_shall_read_next_test_and_print_it)
{
  struct test_s t = {"foo", 0};
  m.feof.func = feof_stub;
  m.fgets.retval = 0x1234;
  m.next_test.retval = t;
  feof_cnt = 0;
  print_test_case_executions("some_file");
  feof_cnt = 0;
  assert_eq(9, m.print_test_case_executor.call_count);
}

test(print_test_case_executions_shall_read_next_test_not_print_it_if_no_test)
{
  struct test_s t = {NULL, 0};
  m.feof.func = feof_stub;
  m.fgets.retval = 0x1234;
  m.next_test.retval = t;
  feof_cnt = 0;
  print_test_case_executions("some_file");
  feof_cnt = 0;
  assert_eq(0, m.print_test_case_executor.call_count);
}

/*****************************************************************************
 * main()
 */

test(main_shall_do_a_sanity_check_of_argument_count_print_an_error_wrong_cnt)
{
  char* argv[] = {"program_name"};
  main(1, argv);
  assert_eq(1, m.fwrite.call_count);
  assert_eq(stderr, m.fwrite.args.arg3);
}

test(main_shall_print_usage_if_argument_count_is_not_3)
{
  char* argv[] = {"program_name"};
  main(1, argv);
  assert_eq(1, m.usage.call_count);
}

test(main_shall_return_EXIT_FAILURE_if_argument_count_is_not_3)
{
  char* argv[] = {"program_name"};
  assert_eq(EXIT_FAILURE, main(1, argv));
}


test(main_shall_check_if_test_source_file_name_exists)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  main(3, argv);
  assert_eq(1, m.file_exists.call_count);
  assert_eq("test_file", m.file_exists.args.arg0);
}

test(main_shall_check_if_mock_header_file_name_exists)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.retval = 1;
  main(3, argv);
  assert_eq(2, m.file_exists.call_count);
  assert_eq("mock_file", m.file_exists.args.arg0);
}

static int file_exists_false_test_source_file(const char* filename)
{
  if (0 == strcmp("test_file", filename)) {
    return 0;
  }
  if (0 == strcmp("mock_file", filename)) {
    return 1;
  }
  return 0;
}

static int file_exists_false_mock_header_file(const char* filename)
{
  if (0 == strcmp("test_file", filename)) {
    return 1;
  }
  if (0 == strcmp("mock_file", filename)) {
    return 0;
  }
  return 0;
}

test(main_shall_return_EXIT_FAILURE_if_test_source_file_could_not_be_found)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.func = file_exists_false_test_source_file;
  assert_eq(EXIT_FAILURE, main(3, argv));
}

test(main_shall_return_EXIT_FAILURE_if_mock_header_file_could_not_be_found)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.func = file_exists_false_mock_header_file;
  assert_eq(EXIT_FAILURE, main(3, argv));
}

test(main_shall_print_header)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.retval = 1;
  main(3, argv);
  assert_eq(1, m.print_header.call_count);
  assert_eq("program_name", m.print_header.args.arg0);
  assert_eq("test_file", m.print_header.args.arg1);
  assert_eq("mock_file", m.print_header.args.arg2);
}

test(main_shall_print_main_function_prologue)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.retval = 1;
  main(3, argv);
  assert_eq(1, m.print_main_function_prologue.call_count);
  assert_eq("test_file", m.print_main_function_prologue.args.arg0);
}

test(main_shall_print_test_case_executions)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.retval = 1;
  main(3, argv);
  assert_eq(1, m.print_test_case_executions.call_count);
  assert_eq("test_file", m.print_test_case_executions.args.arg0);
}

test(main_shall_print_main_function_epilogue)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.retval = 1;
  main(3, argv);
  assert_eq(1, m.print_main_function_epilogue.call_count);
  assert_eq("test_file", m.print_main_function_epilogue.args.arg0);
}

test(main_shall_return_EXIT_SUCCESS_if_all_went_well)
{
  char* argv[] = {"program_name", "test_file", "mock_file"};
  m.file_exists.retval = 1;
  assert_eq(EXIT_SUCCESS, main(3, argv));
}

/* Important if you rename main() */
#undef main
