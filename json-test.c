#include "json.h"

#include <stdlib.h>
#include <string.h>

typedef unsigned u32;
typedef char const* str;
typedef _Bool bool;

#define check(c) if (!(c)) abort()

static bool starts_with(str a, str b) {
  for (u32 i = 0; b[i]; ++i) {
    if (a[i] != b[i])
      return 0;
  }
  return 1;
}

static void test_value(str json) {
  str end = json + strlen(json);
  check(json_value(json, end) == end);
}

static void test_bad_value(str json) {
  str end = json + strlen(json);
  check(!json_value(json, end));
}

typedef struct {
  json_t next;
  char* string;
  unsigned string_size;
} json_string_alloc_result;

static json_string_alloc_result json_string_alloc(str json, str end) {
  if (!(json = json_string_begin(json, end)))
    return (json_string_alloc_result) {};
  char* buf = 0;
  u32 buf_ofs = 0;
  while (1) {
    u32 buf_size = buf_ofs + 4096;
    buf = realloc(buf, buf_size);
    json_string_result ans = json_string(json, end, buf + buf_ofs, buf + buf_size);
    if (ans.type == json_string_error) {
      free(buf);
      return (json_string_alloc_result) {};
    }
    buf_ofs = (u32) (ans.dst_out - buf);
    if (ans.type == json_string_done)
      return (json_string_alloc_result) {ans.json_out, buf, buf_ofs};
    json = ans.json_out;
  }
}

int main() {
  {
    str json = "\"hello";
    str end = json + strlen(json);
    json_string_alloc_result ans = json_string_alloc(json, end);
    check(!ans.next);
  }
  {
    str json = "\"hello\\nworld! wiehhjfh fdhsajkl q fdhsjkahewq dsh fdkahf fhsajh l \\t fhjdslhf dsahjf ksdajf hjak hfdskhjkahjfks appleeeef\\bfruit\"";
    str end = json + strlen(json);
    json_string_alloc_result ans = json_string_alloc(json, end);
    check(ans.next);
    #define expect "hello\nworld! wiehhjfh fdhsajkl q fdhsjkahewq dsh fdkahf fhsajh l \t fhjdslhf dsahjf ksdajf hjak hfdskhjkahjfks appleeeef\bfruit"
    memcmp(ans.string, expect, sizeof(expect) - 1);
    #undef expect
    free(ans.string);
  }

  {
    str val = "\"hello\\nworld!\"";
    check(json_string_equal(val, val + strlen(val), "hello\nworld!"));
  }

  test_value("{\"key\": \"value\"}");
  test_value("true");
  test_value("false");
  test_value("null");
  test_value("[4, 5]");
  test_value("432143");
  test_value("432143.3443e+54");
  test_value("[5, 6, 7, \"hi there\"]");
  test_value("{\"\": \"empty\", \"five\": [1, 2, 3, 4, 5], \"seven\": 7}");

  test_bad_value("True");
  test_bad_value("False");
  test_bad_value("NULL");
  test_bad_value("[4 5]");
  test_bad_value("[0123]");
  test_bad_value("432143.3443e++54");
  test_bad_value("{\"\": \"empty\", \"five: [1, 2, 3, 4, 5], \"seven\": 7}");

  {
    char const* json = "{\"key\": \"value\"}";
    char const* end = json + strlen(json);
    check(starts_with(json_object_get(json, end, "key"), "\"value\""));
  }
  {
    str json = "{\"\": \"empty\", \"five\": [1, 2, 3, 4, 5], \"seven\": 7}";
    str end = json + strlen(json);
    check(starts_with(json_object_get(json, end, "seven"), "7"));
  }
  {
    // Missing quote
    str json = "{\"\": \"empty\", \"five: [1, 2, 3, 4, 5], \"seven\": 7}";
    str end = json + strlen(json);
    check(!json_object_get(json, end, "seven"));
  }
  {
    ;
    str json = "{\"type\":\"request\",\"user\":{\"name\":\"John\",\"username\":\"john\"},\"project\":{\"id\":40,\"name\":\"Software\",\"visibility\":0,\"config\":null,\"homepage\":\"http://www.google.com/\"},\"attributes\":{\"assignee\":33,\"created\":\"2024-12-18 21:29:35 UTC\",\"draft\":false,\"id\":1309,\"action\":\"approved\",\"assignees\":[33]},\"labels\":[{\"id\":21,\"title\":\"Important\"},{\"id\":9,\"title\":\"Software\"}]}";
    str end = json + strlen(json);
    json = json_object_get(json, end, "attributes");
    json = json_object_get(json, end, "action");
    check(json_string_equal(json, end, "approved"));
  }
  {
    str json = "31451";
    str end = json + strlen(json);
    check(!json_object_get(json, end, "key"));
  }
  {
    str json = "true";
    str end = json + strlen(json);
    check(!json_object_get(json, end, "key"));
  }
}
