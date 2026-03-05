#include "IValidate.hpp"

class simpleMem : IValidate {
   public:
    bool validate(IProtocol* protocol);
    const char* getValidationName() { return "Simple Memory Read/Write"; }
};
