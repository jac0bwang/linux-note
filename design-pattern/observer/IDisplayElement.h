#ifndef _IDISPLAYELEMENT_H
#define _IDISPLAYELEMENT_H

namespace designpattern {
namespace observer {

class IDisplayElement {
public:
    virtual void display() = 0;
};

} // observer
} // designpattern
#endif //_IDISPLAYELEMENT_H
