#include <core/typeId.h>

void printRegisteredType(Typhoon::TypeId typeId);

template <class T>
inline void printRegisteredType() {
	printRegisteredType(Typhoon::getTypeId<T>());
}
