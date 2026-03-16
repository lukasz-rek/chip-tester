#pragma once
#include <stdint.h>

#include "IProtocol.hpp"

class IValidate {
   public:
    virtual ~IValidate() = default;
    virtual bool validate(IProtocol* protocol) = 0;
    virtual const char* getValidationName() = 0;
};
