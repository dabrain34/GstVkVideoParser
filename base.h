/* VideoParser
 * Copyright (C) 2022 Igalia, S.L.
 *     Author: Víctor Jáquez <vjaquez@igalia.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You
 * may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
*/

#pragma once

#include <cinttypes>
#include <cassert>

class VkParserVideoRefCountBase {
public:
    virtual int32_t AddRef() = 0;
    virtual int32_t Release() = 0;
protected:
    virtual ~VkParserVideoRefCountBase() { }
};

template<class VkBaseObjType>
class VkSharedBaseObj
{
public:
  VkSharedBaseObj<VkBaseObjType>& Reset(VkBaseObjType* const newObjectPtr)
  {
    if (newObjectPtr != m_sharedObject) {
      int refCount;
	    if (m_sharedObject != nullptr) {
        refCount = m_sharedObject->Release();
        assert (refCount >= 0);
	    }
	    m_sharedObject = newObjectPtr;
	    if (newObjectPtr != nullptr) {
        refCount = newObjectPtr->AddRef();
        assert (refCount > 0);
	    }
    }
    return *this;
  }

  // Constructors increment the refcount of the provided object if non-nullptr
  explicit VkSharedBaseObj(VkBaseObjType* const newObjectPtr = nullptr)
    : m_sharedObject(nullptr)
  {
    Reset(newObjectPtr);
  }

  VkSharedBaseObj(const VkSharedBaseObj<VkBaseObjType>& newObject)
    : m_sharedObject(nullptr)
  {
    Reset(newObject.Get());
  }

  ~VkSharedBaseObj() { Reset(nullptr); }

  // Assignment from another smart pointer maps to raw pointer assignment
  VkSharedBaseObj<VkBaseObjType>& operator=(const VkSharedBaseObj<VkBaseObjType>& sharedObject)
  {
    return Reset(sharedObject.Get());
  }

  VkSharedBaseObj<VkBaseObjType>& operator=(VkBaseObjType* const newObjectPtr)
  {
    return Reset(newObjectPtr);
  }

  template <class VkBaseObjType2> const VkSharedBaseObj<VkBaseObjType>& operator=(const VkSharedBaseObj<VkBaseObjType2>& otherSharedObject)
  {
    return Reset(otherSharedObject.Get());
  }

  // Comparison operators can be used with any compatible types
  inline bool operator==(const VkSharedBaseObj<VkBaseObjType>& otherObject)
  {
    return (this->Get() == otherObject.Get());
  }

  inline bool operator!=(const VkSharedBaseObj<VkBaseObjType>& otherObject)
  {
    return !(*this == otherObject);
  }

  bool operator!() const { return m_sharedObject == nullptr; }

  // Exchange
  void Swap(VkSharedBaseObj<VkBaseObjType>& sharedObject)
  {
    VkSharedBaseObj<VkBaseObjType> tmp(m_sharedObject);
    m_sharedObject = sharedObject.m_sharedObject;
    sharedObject.m_sharedObject = tmp;
  }

  // Non ref-counted access to the underlying object
  VkBaseObjType* Get(void) const
  {
    return m_sharedObject;
  }

  // Cast to a raw object pointer
  operator VkBaseObjType*() const
  {
    return m_sharedObject;
  }

  VkBaseObjType* operator->() const
  {
    return m_sharedObject;
  }

  VkBaseObjType& operator*() const
  {
    return *m_sharedObject;
  }

private:
  VkBaseObjType* m_sharedObject;
};
