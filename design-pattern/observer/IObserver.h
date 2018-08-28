#ifndef _IOBSERVER_H
#define _IOBSERVER_H

namespace designpattern {
namespace observer {

class Subject;

class IObserver {
public:
    virtual void update(const Subject& subject) = 0;
};

} // observer
} // designpattern
#endif //_IOBSERVER_H
