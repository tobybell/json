#pragma once

typedef char const* json_t;

// Consume a complete value
json_t json_value(json_t, json_t end);

// Get a value from an object
json_t json_object_get(json_t, json_t end, char const* key);

// Consume whitespace
json_t json_trim(json_t, json_t end);
