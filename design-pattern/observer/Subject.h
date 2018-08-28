#ifndef _SUBJECT_H
#define _SUBJECT_H

#include "IObserver.h"
#include <vector>

namespace designpattern {
namespace observer {

class Subject {
private:
	std::vector<IObserver*> observers_;
	bool changed_;

protected:
	Subject();
	virtual ~Subject();

public:
	void registerObserver(IObserver* observer);
	void removeObserver(IObserver* observer);
	void notifyObservers();

	void setChanged();
	void clearChanged();
	bool hasChanged() const;
};

} // observer
} // designpattern
#endif //_SUBJECT_H
