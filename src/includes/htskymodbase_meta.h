// ----------------------------------------------------------------------------
// HTSkyModBase
// - A Simple mod developed for exporting in-game funtions of Sky: CotL.
// <https://www.github.com/HTMonkeyG/HTSkyModBase>
//
// MIT License
//
// Copyright (c) 2025 HTMonkeyG
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ----------------------------------------------------------------------------
// - htskymodbase_meta.h
// MetaSystem C API of Sky.
//
// - Why C API?
// To achieve maximum compatibility, if the program only uses trivial structs,
// it can be written in C and compiled with any compiler.
// ----------------------------------------------------------------------------

#ifndef __HTSKYMODBASE_META_H__
#define __HTSKYMODBASE_META_H__

#include "htskymodbase.h"

#ifdef __cplusplus
#ifndef _MSC_VER
#warning "cpp api of HTSkyModbase is recommended to compile with MSVC for a better compatibility."
#endif
#include "htskymodbase.hpp"
#include <vector>
#endif

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/types
// ----------------------------------------------------------------------------

#ifndef __cplusplus

typedef struct MetaTypeHandle *PMetaType;
typedef const struct MetaTypeHandle *PCMetaType;

typedef struct MetaClassHandle *PMetaClass;
typedef const struct MetaClassHandle *PCMetaClass;

typedef struct MetaDataClassC *PMetaDataClass;
typedef const struct MetaDataClassC *PCMetaDataClass;

typedef PCMetaType (*PFN_RegisterMetaType)();
typedef PCMetaClass (*PFN_RegisterMetaClass)();

struct MetaTypeVtbl {
  VOID        (*Release)          (PCMetaType, UINT8);
  SIZE_T      (*SizeOfType)       (PCMetaType);
  SIZE_T      (*AlignOfType)      (PCMetaType);
  LPVOID      (*NewByType)        (PCMetaType);
  VOID        (*DeleteByType)     (PCMetaType, LPVOID);
  LPVOID      (*ConstructByType)  (PCMetaType, LPVOID);
  VOID        (*DestructByType)   (PCMetaType, LPVOID);
  VOID        (*DynamicCast)      (PCMetaType, LPVOID, LPVOID, PCMetaType);
  UINT8       (*IsNumber)         (PCMetaType);
  UINT8       (*IsString)         (PCMetaType);
  DOUBLE      (*ToNumber)         (PCMetaType, LPVOID);
  LPCSTR      (*ToString)         (PCMetaType, LPVOID);
  PCMetaClass (*AsClass)          (PCMetaType);
  VOID        (*WriteType)        (PCMetaType, LPVOID, LPVOID);
  VOID        (*ReadType)         (PCMetaType, LPVOID, INT32, LPVOID);
  PMetaType   (*Copy)             (PCMetaType);
  PMetaType   (*SimpleCopy)       (PCMetaType, PMetaType);
};

struct MetaTypeHandle {
  const MetaTypeVtbl *const lpVtbl;
};

struct MetaClassVtbl {
  VOID        (*Release)          (PCMetaClass, UINT8);
  SIZE_T      (*SizeOfType)       (PCMetaClass);
  SIZE_T      (*AlignOfType)      (PCMetaClass);
  LPVOID      (*NewByType)        (PCMetaClass);
  VOID        (*DeleteByType)     (PCMetaClass, LPVOID);
  LPVOID      (*ConstructByType)  (PCMetaClass, LPVOID);
  VOID        (*DestructByType)   (PCMetaClass, LPVOID);
  VOID        (*DynamicCast)      (PCMetaClass, LPVOID, LPVOID, PCMetaClass);
  UINT8       (*IsNumber)         (PCMetaClass);
  UINT8       (*IsString)         (PCMetaClass);
  DOUBLE      (*ToNumber)         (PCMetaClass, LPVOID);
  LPCSTR      (*ToString)         (PCMetaClass, LPVOID);
  PCMetaClass (*AsClass)          (PCMetaClass);
  VOID        (*WriteType)        (PCMetaClass, LPVOID, LPVOID);
  VOID        (*ReadType)         (PCMetaClass, LPVOID, INT32, LPVOID);
  PMetaClass  (*Copy)             (PCMetaClass);
  PMetaClass  (*SimpleCopy)       (PCMetaClass, PMetaClass);
  UINT8       (*IsAbstract)       (PCMetaClass);
  UINT8       (*IsPolymorphic)    (PCMetaClass);
  SIZE_T      (*SizeOfObject)     (PCMetaClass);
  SIZE_T      (*AlignOfObject)    (PCMetaClass);
  LPVOID      (*NewObject)        (PCMetaClass);
  VOID        (*DeleteObject)     (PCMetaClass, LPVOID);
  LPVOID      (*ConstructObject)  (PCMetaClass, LPVOID);
  VOID        (*DestructObject)   (PCMetaClass, LPVOID);
  LPVOID      (*Upcast)           (PCMetaClass, LPVOID *);
  LPVOID      (*Downcast)         (PCMetaClass, LPVOID *);
  LPVOID      (*ResolveMember)    (PCMetaClass, LPVOID, PCMetaClass, int *);
};

struct MetaClassHandle {
  const MetaClassVtbl *const lpVtbl;
};

struct MetaDataClassC {
  LPCSTR name;
  LPVOID param;
  LPVOID next;
};

#else

typedef class IMetaType *PMetaType;
typedef const class IMetaType *PCMetaType;

typedef class IMetaClass *PMetaClass;
typedef const class IMetaClass *PCMetaClass;

typedef struct MetaDataClassC *PMetaDataClass;
typedef const struct MetaDataClassC *PCMetaDataClass;

typedef PCMetaType (*PFN_RegisterMetaType)();
typedef PCMetaClass (*PFN_RegisterMetaClass)();

class IMetaType {
public:
  using PFN_RegisterType = IMetaType *(*);

  IMetaType(
    LPCSTR name,
    IMetaType *prev = nullptr,
    void *fields = nullptr
  ) {
    m_name = name;
    m_fields = fields;
    if (prev)
      prev->m_prev = this;
  }

#ifndef _MSC_VER
  ~IMetaType() = default;

  virtual VOID        Release         (UINT8 _Delete = 0) {
    this->~IMetaType();
    if (_Delete) operator delete(this);
  };
#else
  virtual             ~IMetaType      () = default;
#endif

  virtual SIZE_T      SizeOfType      () const = 0;
  virtual SIZE_T      AlignOfType     () const = 0;
  virtual LPVOID      NewByType       () const = 0;
  virtual VOID        DeleteByType    (LPVOID) const = 0;
  virtual LPVOID      ConstructByType (LPVOID) const = 0;
  virtual VOID        DestructByType  (LPVOID) const = 0;
  virtual VOID        DynamicCast     (LPVOID, LPVOID, PCMetaClass) const = 0;
  virtual UINT8       IsNumber        () const = 0;
  virtual UINT8       IsString        () const = 0;
  virtual DOUBLE      ToNumber        (LPVOID) const = 0;
  virtual LPCSTR      ToString        (LPVOID) const = 0;
  virtual PCMetaClass AsClass         () const = 0;
  virtual VOID        WriteType       (LPVOID, LPVOID) const = 0;
  virtual VOID        ReadType        (LPVOID, INT32, LPVOID) const = 0;
  virtual PMetaType   Copy            () const = 0;
  virtual PMetaType   SimpleCopy      (PMetaType) const = 0;

  PCMetaType GetActivated() {
    return m_active;
  }

  // Name of the object.
  const char *m_name = nullptr;
  // External descriptors.
  void *m_fields = nullptr;
  // Previous object, build a chain list for initialization.
  IMetaType *m_prev = nullptr;
  // Only for padding.
  void *__padding = nullptr;
  // Point to currently activated copy of the type/class.
  IMetaType *m_active = this;
};

class IMetaTypeVoid: public IMetaType {
public:
  IMetaTypeVoid(
    LPCSTR name,
    IMetaType *prev = nullptr
  ): IMetaType(name, prev) { }

#ifndef _MSC_VER
  ~IMetaTypeVoid() = default;

  virtual VOID        Release         (UINT8 _Delete = 0) override {
    this->~IMetaTypeVoid();
    if (_Delete) operator delete(this);
  };
#else
  virtual             ~IMetaTypeVoid  () = default;
#endif

  virtual SIZE_T      SizeOfType      () const { return 0; };
  virtual SIZE_T      AlignOfType     () const { return 0; };
  virtual LPVOID      NewByType       () const { return 0; };
  virtual VOID        DeleteByType    (LPVOID) const { };
  virtual LPVOID      ConstructByType (LPVOID) const { return 0; };
  virtual VOID        DestructByType  (LPVOID) const { };
  virtual VOID        DynamicCast     (LPVOID, LPVOID, PCMetaClass) const { };
  virtual UINT8       IsNumber        () const { return 0; };
  virtual UINT8       IsString        () const { return 0; };
  virtual DOUBLE      ToNumber        (LPVOID) const { return 0; };
  virtual LPCSTR      ToString        (LPVOID) const { return nullptr; };
  virtual PCMetaClass AsClass         () const { return nullptr; };
  virtual VOID        WriteType       (LPVOID, LPVOID) const { };
  virtual VOID        ReadType        (LPVOID, INT32, LPVOID) const { };
  virtual PMetaType   Copy            () const { return nullptr; };
  virtual PMetaType   SimpleCopy      (PMetaType) const { return nullptr; };
};

class IMetaClass: public IMetaType {
public:
  using PFN_RegisterClass = IMetaClass *(*);

  IMetaClass(
    LPCSTR name,
    PFN_RegisterClass parent = nullptr,
    IMetaType *prev = nullptr,
    void *fields = nullptr
  ): IMetaType(name, prev, fields) {
    m_parent = parent;
  }

#ifndef _MSC_VER
  ~IMetaClass() = default;

  virtual VOID        Release         (UINT8 _Delete = 0) override {
    this->~IMetaClass(); if (_Delete) operator delete(this);
  };
#else
  virtual             ~IMetaClass     () = default;
#endif

  virtual UINT8       IsAbstract      () const = 0;
  virtual UINT8       IsPolymorphic   () const = 0;
  virtual SIZE_T      SizeOfObject    () const = 0;
  virtual SIZE_T      AlignOfObject   () const = 0;
  virtual LPVOID      NewObject       () const = 0;
  virtual VOID        DeleteObject    (LPVOID) const = 0;
  virtual LPVOID      ConstructObject (LPVOID) const = 0;
  virtual VOID        DestructObject  (LPVOID) const = 0;
  virtual LPVOID      Upcast          (LPVOID *) const = 0;
  virtual LPVOID      Downcast        (LPVOID *) const = 0;
  virtual LPVOID      ResolveMember   (LPVOID, PCMetaClass, int *) const = 0;

  PCMetaClass GetActivated() {
    return m_active->AsClass();
  }

  // Call the function to get the parent class.
  PFN_RegisterClass m_parent = nullptr;
  // Global id of the metaclass.
  int m_globalId = -1;
  // Topology id of the metaclass.
  int m_topoOrder = -1;

protected:

#ifndef _MSC_VER
  void *__protectedMember[5] = {0};
#else
  // - When not compiled with MSVC, the fields below is not binary compatible
  // with Sky or SkyModBase.
  std::vector<int> m_baseTopoIdList = {};
  void *m_metaDataContainer = nullptr;
  void *m_vtableCache = nullptr;
#endif

};

class IMetaClassVoid: public IMetaClass {
public:
  IMetaClassVoid(
    LPCSTR name,
    PFN_RegisterClass parent = nullptr,
    IMetaType *prev = nullptr
  ): IMetaClass(name, parent, prev) { }

#ifndef _MSC_VER
  ~IMetaClassVoid() = default;

  virtual VOID        Release         (UINT8 _Delete) override {
    this->~IMetaClassVoid(); if (_Delete) operator delete(this);
  };
#else
  virtual             ~IMetaClassVoid () = default;
#endif

  virtual SIZE_T      SizeOfType      () const { return 0; };
  virtual SIZE_T      AlignOfType     () const { return 0; };
  virtual LPVOID      NewByType       () const { return 0; };
  virtual VOID        DeleteByType    (LPVOID) const { };
  virtual LPVOID      ConstructByType (LPVOID) const { return 0; };
  virtual VOID        DestructByType  (LPVOID) const { };
  virtual VOID        DynamicCast     (LPVOID, LPVOID, PCMetaClass) const { };
  virtual UINT8       IsNumber        () const { return 0; };
  virtual UINT8       IsString        () const { return 0; };
  virtual DOUBLE      ToNumber        (LPVOID) const { return 0; };
  virtual LPCSTR      ToString        (LPVOID) const { return nullptr; };
  virtual PCMetaClass AsClass         () const { return nullptr; };
  virtual VOID        WriteType       (LPVOID, LPVOID) const { };
  virtual VOID        ReadType        (LPVOID, INT32, LPVOID) const { };
  virtual PMetaType   Copy            () const { return nullptr; };
  virtual PMetaType   SimpleCopy      (PMetaType) const { return nullptr; };
  virtual UINT8       IsAbstract      () const { return 0; };
  virtual UINT8       IsPolymorphic   () const { return 0; };
  virtual SIZE_T      SizeOfObject    () const { return 0; };
  virtual SIZE_T      AlignOfObject   () const { return 0; };
  virtual LPVOID      NewObject       () const { return nullptr; };
  virtual VOID        DeleteObject    (LPVOID) const { };
  virtual LPVOID      ConstructObject (LPVOID) const { return nullptr; };
  virtual VOID        DestructObject  (LPVOID) const { };
  virtual LPVOID      Upcast          (LPVOID *) const { return nullptr; };
  virtual LPVOID      Downcast        (LPVOID *) const { return nullptr; };
  virtual LPVOID      ResolveMember   (LPVOID, PCMetaClass, int *) const { return nullptr; };
};

#endif

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/functions/MetaType
// ----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// MetaType initializer.
typedef struct {
  // Size of the object/type.
  SIZE_T size;
  // Memory alignment of the object/type.
  SIZE_T align;
  // Constructor of the object/type. If the class is abstract, this field will
  // be ignored.
  VOID (*construct)(LPVOID);
  // Destructor of the object/type.
  VOID (*destruct)(LPVOID);
} MetaTypeInitTypeDef;

// MetaClass initializer.
typedef struct {
  // Size of the object/type.
  SIZE_T size;
  // Memory alignment of the object/type.
  SIZE_T align;
  // Constructor of the object/type. If the class is abstract, this field will
  // be ignored.
  VOID (*construct)(LPVOID);
  // Destructor of the object/type.
  VOID (*destruct)(LPVOID);
} MetaClassInitTypeDef;

// Create a MetaType object.
SMB_API_ATTR PMetaType SMB_API Sky_MetaType_Create(
  LPCSTR name,
  const MetaTypeInitTypeDef *init);

// Destroy a MetaType object. Before doing this, please understand what you are doing.
SMB_API_ATTR PMetaType SMB_API Sky_MetaType_Destroy(
  PMetaType hMetaType);

SMB_API_ATTR LPCSTR SMB_API Sky_MetaType_GetName(
  PCMetaType hMetaType);

SMB_API_ATTR PCMetaType SMB_API Sky_MetaType_GetActive(
  PCMetaType hMetaType);

SMB_API_ATTR VOID SMB_API Sky_MetaType_SetPrev(
  PCMetaType hMetaType,
  PCMetaType hPrevMetaType);

SMB_API_ATTR VOID SMB_API Sky_MetaType_SetParams(
  PCMetaType hMetaType,
  PCMetaDataClass pParams);

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/functions/MetaClass
// ----------------------------------------------------------------------------

// In the underlying C++ implementation, MetaClass inherits from MetaType, so a
// MetaClass handle can be converted to a MetaType handle and reuse MetaType functions.

// Create a MetaClass object.
SMB_API_ATTR PMetaClass SMB_API Sky_MetaClass_Create(
  LPCSTR name,
  PFN_RegisterMetaClass parent,
  const MetaClassInitTypeDef *init);

// Destroy a MetaType object. Before doing this, please understand what you are doing.
SMB_API_ATTR PMetaType SMB_API Sky_MetaClass_Destroy(
  PMetaClass hMetaClass);

SMB_API_ATTR BOOL SMB_API Sky_MetaClass_IsDevivedFrom(
  PCMetaClass hMetaChild,
  PCMetaClass hMetaParent);

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/functions/MetaSystem
// ----------------------------------------------------------------------------

// Submit all MetaType objects in the chain to ModBase.
// This function is asynchronized.
SMB_API_ATTR HTStatus SMB_API Sky_MetaSystem_SubmitChain(
  HMODULE hModuleOwner,
  PMetaType hMetaType);

#ifdef __cplusplus
}
#endif

#endif
