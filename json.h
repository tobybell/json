#pragma once

typedef char const* json_t;

// Consume a complete value
json_t json_value(json_t, json_t end);

// Get a value from an object
json_t json_object_get(json_t, json_t end, char const* key);

// Get a value from an array
json_t json_array_get(json_t, json_t end, unsigned index);

// Consume whitespace
json_t json_trim(json_t, json_t end);

// Get the length of the string literal
unsigned json_string_length(json_t, json_t end);

json_t json_string_begin(json_t, json_t end);

typedef struct {
  enum {
    json_string_error,
    json_string_done,
    json_string_not_done,
  } type;
  json_t json_out;
  char* dst_out;
} json_string_result;

// Get value as string, writing contents to `dst` until reaching `dst_end`.
json_string_result json_string(json_t json, json_t end, char* dst, char* dst_end);

_Bool json_string_equal(json_t, json_t end, char const* c_string);

// Get value as number. Returns NaN if parsing failed.
double json_number(json_t, json_t end);

// Get value as bool. Returns:
//   0 -> parsing failed
//   1 -> false
//   2 -> true
unsigned char json_bool(json_t, json_t end);

// Get value as null. Returns:
//   0 -> parsing failed
//   1 -> null
unsigned char json_null(json_t, json_t end);
