#include <stdlib.h>
#include <string.h>

typedef unsigned u32;
typedef char const* str;
typedef _Bool bool;

#define check(c) if (!(c)) abort()
#define guard(c) if (!(c)) return 0

static bool is_whitespace(char c) {
  // space, horizontal tab, line feed, carriage return
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

str json_trim(str i, str end) {
  check(i);
  for (; i < end && is_whitespace(*i); ++i) {}
  return i;
}

static str string_rest(str i, str end) {
  for (; i < end; ++i) {
    if (*i == '"')
      return i + 1;
    if (*i == '\\')
      ++i;
  }
  return 0;
}

static str string(str i, str end) {
  check(i < end);
  guard(*i == '"');
  return string_rest(i + 1, end);
}

static str number_int(str i, str end) {
  if (*i == '-')
    guard(++i < end);
  if (*i == '0')
    return i + 1;
  guard((u32) (*i - '1') < 9);
  for (++i; i < end && (u32) (*i - '0') < 10; ++i) {}
  return i;
}

static str nonzero_digits(str i, str end) {
  check(i < end);
  guard((u32) (*i - '0') < 10);
  for (++i; i < end && (u32) (*i - '0') < 10; ++i) {}
  return i;
}

static str number(str i, str end) {
  guard(i = number_int(i, end));
  if (i < end && *i == '.')
    guard(++i < end && (i = nonzero_digits(i, end)));
  if (i < end && (*i == 'e' || *i == 'E')) {
    guard(++i < end);
    if (*i == '+' || *i == '-')
      guard(++i < end);
    return nonzero_digits(i, end);
  }
  return i;
}

str json_value(str i, str end) {
  guard(i < end);
  if (*i == '{') {
    i = json_trim(i + 1, end);
    guard(i < end);
    if (*i == '}')
      return i + 1;
    while (1) {
      guard(i = string(i, end));  // key
      i = json_trim(i, end);
      guard(i < end && *i == ':');
      i = json_trim(i + 1, end);
      guard(i = json_value(i, end));
      i = json_trim(i, end);
      guard(i < end);
      if (*i == '}')
        return i + 1;
      guard(*i == ',');
      i = json_trim(i + 1, end);
      guard(i < end);
    }
  }
  if (*i == '[') {
    i = json_trim(i + 1, end);
    guard(i < end);
    if (*i == ']')
      return i + 1;
    while (1) {
      guard(i = json_value(i, end));
      i = json_trim(i, end);
      guard(i < end);
      if (*i == ']')
        return i + 1;
      guard(*i == ',');
      i = json_trim(i + 1, end);
    }
  }
  if (*i == '"')
    return string_rest(i + 1, end);
  if (i + 4 <= end && !memcmp(i, "true", 4))
    return i + 4;
  if (i + 5 <= end && !memcmp(i, "false", 5))
    return i + 5;
  if (i + 4 <= end && !memcmp(i, "null", 4))
    return i + 4;

  return number(i, end);
}

typedef struct StringEqual {
  bool equal;
  str end;
} StringEqual;

static char escape_code(char c) {
  switch (c) {
    case '"': return '"';
    case '\\': return '\\';
    case 'b': return 0x8;
    case 'f': return 0xc;
    case 'n': return 0xa;
    case 'r': return 0xd;
    case 't': return 0x9;
    default: return 0;
  }
}

static str string_equal(str i, str end, str key, bool* equal) {
  guard(i < end && *i == '"');
  ++i;
  for (; *key; ++i, ++key) {
    guard(i < end);
    if (*i == '"')
      return (*equal = 0), i + 1;
    if (*i == '\\') {
      guard(++i < end);
      if (escape_code(*i) != *key)
        return (*equal = 0), string_rest(i + 1, end);
    } else if (*i != *key) {
      return (*equal = 0), string_rest(i + 1, end);
    }
  }
  guard(i < end);
  if (*i != '"')
    return (*equal = 0), string_rest(i, end);
  return (*equal = 1), i + 1;
}

str json_object_get(str i, str end, str key) {
  guard(i < end && *i == '{');
  i = json_trim(i + 1, end);
  while (1) {
    bool equal;
    guard(i = string_equal(i, end, key, &equal));
    i = json_trim(i, end);
    guard(i < end && *i == ':');
    i = json_trim(i + 1, end);
    guard(i < end);
    if (equal)
      return i;  // yes this is our key
    guard(i = json_value(i, end));
    i = json_trim(i, end);
    guard(i < end && *i == ',');
    i = json_trim(i + 1, end);
  }
}
