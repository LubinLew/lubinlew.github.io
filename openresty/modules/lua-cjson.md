# Lua CJSON 2.1.0 Manual

Mark Pulford  
1st March 2012

Table of Contents

[1. Overview](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_overview)

[2. Installation](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_installation)

[2.1. Make](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_make)

[2.2. CMake](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_cmake)

[2.3. RPM](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_rpm)

[2.4. LuaRocks](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_luarocks)

[2.5. Build Options (#define)](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#build_options)

[3. API (Functions)](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_api_functions)

[3.1. Synopsis](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_synopsis)

[3.2. Module Instantiation](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_module_instantiation)

[3.3. decode](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_decode)

[3.4. decode_invalid_numbers](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#decode_invalid_numbers)

[3.5. decode_max_depth](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#decode_max_depth)

[3.6. encode](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode)

[3.7. encode_invalid_numbers](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_invalid_numbers)

[3.8. encode_keep_buffer](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_keep_buffer)

[3.9. encode_max_depth](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_max_depth)

[3.10. encode_number_precision](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_number_precision)

[3.11. encode_sparse_array](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_sparse_array)

[4. API (Variables)](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_api_variables)

[4.1. _NAME](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_name)

[4.2. _VERSION](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_version)

[4.3. null](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_null)

[5. References](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#_references)

## 1. Overview

The Lua CJSON 模块为Lua提供了 json 功能.

**Features**

- Fast, standards compliant encoding/parsing routines

- Full support for JSON with UTF-8, including decoding surrogate pairs

- Optional run-time support for common exceptions to the JSON specification (infinity, NaN,..)

- No dependencies on other libraries

**Caveats**

- UTF-16 and UTF-32 are not supported

Lua CJSON is covered by the MIT license. Review the file LICENSE for details.

The latest version of this software is available from the [Lua CJSON website](http://www.kyne.com.au/%7Emark/software/lua-cjson.php).

Feel free to email me if you have any patches, suggestions, or comments.

## 3. API (Functions)

### 3.1. Synopsis

```lua
-- Module instantiation
local cjson = require "cjson"
local cjson2 = cjson.new()
local cjson_safe = require "cjson.safe"

-- Translate Lua value to/from JSON
text = cjson.encode(value)
value = cjson.decode(text)

-- Get and/or set Lua CJSON configuration
setting = cjson.decode_invalid_numbers([setting])
setting = cjson.encode_invalid_numbers([setting])
keep = cjson.encode_keep_buffer([keep])
depth = cjson.encode_max_depth([depth])
depth = cjson.decode_max_depth([depth])
convert, ratio, safe = cjson.encode_sparse_array([convert[, ratio[, safe]]])
```

### 3.2. Module Instantiation

local cjson = require "cjson"
local cjson2 = cjson.new()
local cjson_safe = require "cjson.safe"

Import Lua CJSON via the Lua require function. Lua CJSON does not register a global module table.

The cjson module will throw an error during JSON conversion if any invalid data is encountered. Refer to [cjson.encode](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#cjson_encode) and [cjson.decode](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#cjson_decode) for details.

The cjson.safe module behaves identically to the cjson module, except when errors are encountered during JSON conversion. On error, thecjson_safe.encode and cjson_safe.decode functions will return nil followed by the error message.

`cjson.new() `can be used to instantiate an independent copy of the Lua CJSON module. The new module has a separate persistent encoding buffer, and default settings.

Lua CJSON can support Lua implementations using multiple preemptive threads within a single Lua state provided the persistent encoding buffer is not shared. This can be achieved by one of the following methods:

- Disabling the persistent encoding buffer with [cjson.encode_keep_buffer](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_keep_buffer)

- Ensuring each thread calls [cjson.encode](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode) separately (ie, treat cjson.encode as non-reentrant).

- Using a separate cjson module table per preemptive thread (cjson.new)

| Note | Lua CJSON uses strtod and snprintf to perform numeric conversion as they are usually well supported, fast and bug free. However, these functions require a workaround for JSON encoding/parsing under locales using a comma decimal separator. Lua CJSON detects the current locale during instantiation to determine and automatically implement the workaround if required. Lua CJSON should be reinitialised via cjson.new if the locale of the current process changes. Using a different locale per thread is not supported. |
| ---- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

### 3.3. decode

```lua
value = cjson.decode(json_text)
```

`cjson.decode()` 将 UTF-8 格式的 JSON 字符串反序列化为Lua 值 或者 Lua Table.

cjson.decode requires that any NULL (ASCII 0) and double quote (ASCII 34) characters are escaped within strings. All escape codes will be decoded and other bytes will be passed transparently. UTF-8 characters are not validated during decoding and should be checked elsewhere if required.

JSON null will be converted to a NULL lightuserdata value. This can be compared with cjson.null for convenience.

By default, numbers incompatible with the JSON specification (infinity, NaN, hexadecimal) can be decoded. This default can be changed with[cjson.decode_invalid_numbers](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#decode_invalid_numbers).

Example: Decoding

```lua
json_text = &#39;[ true, { "foo": "bar" } ]&#39;
value = cjson.decode(json_text)
-- Returns: { true, { foo = "bar" } }
```

| Caution | Care must be taken after decoding JSON objects with numeric keys. Each numeric key will be stored as a Lua string. Any subsequent code assuming type number may break. |
| ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

### 3.4. decode_invalid_numbers

![setting]() = cjson.decode_invalid_numbers([setting])
-- "setting" must be a boolean. Default: true.

Lua CJSON may generate an error when trying to decode numbers not supported by the JSON specification. *Invalid numbers* are defined as:

- infinity

- not-a-number (NaN)

- hexadecimal

Available settings:

true

Accept and decode *invalid numbers*. This is the default setting.

false

Throw an error when *invalid numbers* are encountered.

The current setting is always returned, and is only updated when an argument is provided.

### 3.5. decode_max_depth

depth = cjson.decode_max_depth([depth])
-- "depth" must be a positive integer. Default: 1000.

Lua CJSON will generate an error when parsing deeply nested JSON once the maximum array/object depth has been exceeded. This check prevents unnecessarily complicated JSON from slowing down the application, or crashing the application due to lack of process stack space.

An error may be generated before the depth limit is hit if Lua is unable to allocate more objects on the Lua stack.

By default, Lua CJSON will reject JSON with arrays and/or objects nested more than 1000 levels deep.

The current setting is always returned, and is only updated when an argument is provided.

### 3.6. encode

json_text = cjson.encode(value)

cjson.encode will serialise a Lua value into a string containing the JSON representation.

cjson.encode supports the following types:

- boolean

- lightuserdata (NULL value only)

- nil

- number

- string

- table

The remaining Lua types will generate an error:

- function

- lightuserdata (non-NULL values)

- thread

- userdata

By default, numbers are encoded with 14 significant digits. Refer to [cjson.encode_number_precision](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_number_precision) for details.

Lua CJSON will escape the following characters within each UTF-8 string:

- Control characters (ASCII 0 - 31)

- Double quote (ASCII 34)

- Forward slash (ASCII 47)

- Blackslash (ASCII 92)

- Delete (ASCII 127)

All other bytes are passed transparently.

| Caution | Lua CJSON will successfully encode/decode binary strings, but this is technically not supported by JSON and may not be compatible with other JSON libraries. To ensure the output is valid JSON, applications should ensure all Lua strings passed to cjson.encode are UTF-8.<br><br>Base64 is commonly used to encode binary data as the most efficient encoding under UTF-8 can only reduce the encoded size by a further ~8%. Lua Base64 routines can be found in the [LuaSocket](http://w3.impa.br/%7Ediego/software/luasocket/) and [lbase64](http://www.tecgraf.puc-rio.br/%7Elhf/ftp/lua/#lbase64) packages. |
| ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

Lua CJSON uses a heuristic to determine whether to encode a Lua table as a JSON array or an object. A Lua table with only positive integer keys of type number will be encoded as a JSON array. All other tables will be encoded as a JSON object.

Lua CJSON does not use metamethods when serialising tables.

- rawget is used to iterate over Lua arrays

- next is used to iterate over Lua objects

Lua arrays with missing entries (*sparse arrays*) may optionally be encoded in several different ways. Refer to [cjson.encode_sparse_array](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_sparse_array) for details.

JSON object keys are always strings. Hence cjson.encode only supports table keys which are type number or string. All other types will generate an error.

| Note | Standards compliant JSON must be encapsulated in either an object ({}) or an array ([]). If strictly standards compliant JSON is desired, a table must be passed to cjson.encode. |
| ---- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

By default, encoding the following Lua values will generate errors:

- Numbers incompatible with the JSON specification (infinity, NaN)

- Tables nested more than 1000 levels deep

- Excessively sparse Lua arrays

These defaults can be changed with:

- [cjson.encode_invalid_numbers](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_invalid_numbers)

- [cjson.encode_max_depth](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_max_depth)

- [cjson.encode_sparse_array](https://www.kyne.com.au/~mark/software/lua-cjson-manual.html#encode_sparse_array)

Example: Encoding

value = { true, { foo = "bar" } }
json_text = cjson.encode(value)
-- Returns: '[true,{"foo":"bar"}]'

### 3.7. encode_invalid_numbers

setting = cjson.encode_invalid_numbers([setting])
-- "setting" must a boolean or "null". Default: false.

Lua CJSON may generate an error when encoding floating point numbers not supported by the JSON specification (*invalid numbers*):

- infinity

- not-a-number (NaN)

Available settings:

true

Allow *invalid numbers* to be encoded. This will generate non-standard JSON, but this output is supported by some libraries.

"null"

Encode *invalid numbers* as a JSON null value. This allows infinity and NaN to be encoded into valid JSON.

false

Throw an error when attempting to encode *invalid numbers*. This is the default setting.

The current setting is always returned, and is only updated when an argument is provided.

### 3.8. encode_keep_buffer

keep = cjson.encode_keep_buffer([keep])
-- "keep" must be a boolean. Default: true.

Lua CJSON can reuse the JSON encoding buffer to improve performance.

Available settings:

true

The buffer will grow to the largest size required and is not freed until the Lua CJSON module is garbage collected. This is the default setting.

false

Free the encode buffer after each call to cjson.encode.

The current setting is always returned, and is only updated when an argument is provided.

### 3.9. encode_max_depth

depth = cjson.encode_max_depth([depth])
-- "depth" must be a positive integer. Default: 1000.

Once the maximum table depth has been exceeded Lua CJSON will generate an error. This prevents a deeply nested or recursive data structure from crashing the application.

By default, Lua CJSON will generate an error when trying to encode data structures with more than 1000 nested tables.

The current setting is always returned, and is only updated when an argument is provided.

Example: Recursive Lua table

a = {}; a[1] = a

### 3.10. encode_number_precision

precision = cjson.encode_number_precision([precision])
-- "precision" must be an integer between 1 and 14. Default: 14.

The amount of significant digits returned by Lua CJSON when encoding numbers can be changed to balance accuracy versus performance. For data structures containing many numbers, setting cjson.encode_number_precision to a smaller integer, for example 3, can improve encoding performance by up to 50%.

By default, Lua CJSON will output 14 significant digits when converting a number to text.

The current setting is always returned, and is only updated when an argument is provided.

### 3.11. encode_sparse_array

convert, ratio, safe = cjson.encode_sparse_array([convert[, ratio[, safe]]])
-- "convert" must be a boolean. Default: false.
-- "ratio" must be a positive integer. Default: 2.
-- "safe" must be a positive integer. Default: 10.

Lua CJSON classifies a Lua table into one of three kinds when encoding a JSON array. This is determined by the number of values missing from the Lua array as follows:

Normal

All values are available.

Sparse

At least 1 value is missing.

Excessively sparse

The number of values missing exceeds the configured ratio.

Lua CJSON encodes sparse Lua arrays as JSON arrays using JSON null for the missing entries.

An array is excessively sparse when all the following conditions are met:

- ratio > 0

- *maximum_index* > safe

- *maximum_index* > *item_count* * ratio

Lua CJSON will never consider an array to be *excessively sparse* when ratio = 0. The safe limit ensures that small Lua arrays are always encoded as sparse arrays.

By default, attempting to encode an *excessively sparse* array will generate an error. If convert is set to true, *excessively sparse* arrays will be converted to a JSON object.

The current settings are always returned. A particular setting is only changed when the argument is provided (non-nil).

Example: Encoding a sparse array

cjson.encode({ [3] = "data" })
-- Returns: '[null,null,"data"]'

Example: Enabling conversion to a JSON object

cjson.encode_sparse_array(true)
cjson.encode({ [1000] = "excessively sparse" })
-- Returns: '{"1000":"excessively sparse"}'

## 4. API (Variables)

### 4.1. _NAME

The name of the Lua CJSON module ("cjson").

### 4.2. _VERSION

The version number of the Lua CJSON module ("2.1.0").

### 4.3. null

Lua CJSON decodes JSON null as a Lua lightuserdata NULL pointer. cjson.null is provided for comparison.

## 5. References

- [RFC 4627](http://tools.ietf.org/html/rfc4627)

- [JSON website](http://www.json.org/)

Last updated 2012-03-01 22:52:58 CST
