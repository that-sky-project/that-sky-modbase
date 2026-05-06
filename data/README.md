# ThatSkyModbase Binary Descriptor
Binary descriptors are a solution for mod hot updates and game version compatibility.

Binary descriptors use JSON to describe and submit version-dependent information—such as signatures, object offsets, or vtable slots—to ModBase. Unlike the old approach of hardcoding this information into mod source code, binary descriptors greatly improve ModBase's adaptability.

Moreover, since the binary information is stored in a separate file, anyone can modify and adapt it for their own game version.

## Format
Binary descriptors use a JSON array to hold all descriptor objects, as shown in the following example:
```js
[
  {
    "type": "function",
    "name": "ModuleBarn::CallFunction()",
    "method": "e8",
    "pattern": "48 89 FA 41 B8 ?  ?  ?  ?  45 31 C9 E8 ?  ?  ?  ?  80 3D",
    "offset": "0x0C"
  },
  // ...
]
```

Each descriptor has its type, name, and resolution method, followed by method-specific details.

The table below shows the resolution methods and their required field lists (All numeric types can also accept strings to use hexadecimal numbers):

|method|fields|type|description|
|-|-|-|-|
|`"offset"`|`"offset"`|`number`|Offset relative to module base address.|
||`"module"` (optional)|`string`|Module name where the target resides.|
|`"symbol"`|`"symbol"`|`string`|Exported symbol name of the specified module.|
||`"module"` (optional)|`string`|-|
|`"direct" `&#124;`"e8" `&#124;`"ff15" `|`"pattern"`|`string`|Signature to scan.|
||`"offset"`|`number`|For "direct", it is the offset of the final result relative to the start of the signature; for "e8" or "ff15", it is the offset of the 0xE8 or 0x15 byte within the signature.|
||`"module"` (optional)|`string`|-|
|`"virtual_function"`|`"slot_index"`|`number`|Index in the virtual function table (vtable index).|

`direct`, `e8`, and `ff15` above are signature scanning methods. `direct` returns the address of the first scan result; `e8` returns the jump address of the specified `E8` (call rel32) instruction within the first scan result; `ff15` returns the jump address of the specified `FF15` (call r/m64) instruction within the scan result.

### Functions (`"type":"function"`)
A function descriptor specifies a function. It can be resolved using `offset`, `symbol`, or signature scanning.

Example:
```js
{
  "type": "function",
  "name": "ModuleBarn::CallFunction()",
  "method": "e8",
  "pattern": "48 89 FA 41 B8 ?  ?  ?  ?  45 31 C9 E8 ?  ?  ?  ?  80 3D",
  "offset": "0x0C"
}
```

### Variables (`"type":"variable"`)
A variable descriptor specifies the address of a static or global variable. It can be resolved using `offset` or signature scanning.

Example: _currently N/A_

### Virtual Functions (`"type":"virtual_function"`)
A virtual function descriptor specifies a virtual function. It can be resolved by the slot index in a vftable.

Example:
```js
{
  "type": "virtual_function",
  "name": "FmodStudioSoundSystem::LoadSoundBanks()",
  "slot_index": 12
}
```

### Member Variables (`"type":"member_variable"`)
A member variable descriptor specifies a member variable inside a struct. It can be resolved by the offset of the member in a struct.

Example:
```js
{
  "type": "member_variable",
  "name": "MetaClass::m_name",
  "method": "offset",
  "offset": "0x08"
}
```
