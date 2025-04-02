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

int main() {
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
    check(starts_with(json, "\"approved\""));
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
