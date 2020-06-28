#include "property.h"
#include "flags.h"
#include "type.h"
#include <cassert>

namespace Typhoon {

Property::Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, uint32_t flags, ReflSemantic semantic)
    : setter { std::move(setter) }
    , getter { std::move(getter) }
    , name(name)
    , valueType(valueType)
    , flags(flags)
    , semantic(semantic) {
	assert(valueType);
	// Override flags
	if (! this->setter) {
		flags &= ~ReflFlags::writeable;
		flags &= ~ReflFlags::edit;
	}
	if (! this->getter) {
		flags &= ~ReflFlags::readable;
		flags &= ~ReflFlags::clonable;
		flags &= ~ReflFlags::view;
	}
}

const char* Property::getName() const {
	return name;
}

const Type& Property::getValueType() const {
	return *valueType;
}

uint32_t Property::getFlags() const {
	return flags;
}

ReflSemantic Property::getSemantic() const {
	return semantic;
}

void Property::setValue(void* self, const void* value) const {
	setter(self, value);
}

void Property::getValue(const void* self, void* value) const {
	getter(self, value);
}

void Property::copyValue(void* dstSelf, const void* srcSelf) const {
	char   stack[256];
	void*  dst = stack;
	size_t size = sizeof stack;
	void*  temporary = std::align(valueType->alignment, valueType->size, dst, size);
	assert(temporary);
	if (temporary) {
		valueType->constructObject(temporary);
		const void* value = getter(srcSelf, temporary);
		setter(dstSelf, value);
		valueType->destructObject(temporary);
	}
}

} // namespace Typhoon
