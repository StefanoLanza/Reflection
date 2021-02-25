#include "property.h"
#include "flags.h"
#include "type.h"
#include <cassert>

namespace Typhoon::Reflection {

Property::Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, uint32_t flags, Semantic semantic, Allocator& allocator)
    : setter { std::move(setter) }
    , getter { std::move(getter) }
    , name(name)
    , valueType(valueType)
    , flags(flags)
    , semantic(semantic)
    , attributes(stdAllocator<const Attribute*>(allocator)) {
	assert(valueType);
	// Override flags
	if (! this->setter) {
		flags &= ~Flags::writeable;
		flags &= ~Flags::edit;
	}
	if (! this->getter) {
		flags &= ~Flags::readable;
		flags &= ~Flags::clonable;
		flags &= ~Flags::view;
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

Semantic Property::getSemantic() const {
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
	void*  temporary = std::align(valueType->getAlignment(), valueType->getSize(), dst, size);
	assert(temporary);
	if (temporary) {
		valueType->constructObject(temporary);
		const void* value = getter(srcSelf, temporary);
		setter(dstSelf, value);
		valueType->destructObject(temporary);
	}
}

void Property::addAttribute(const Attribute* attribute) {
	attributes.push_back(attribute);
}

span<const Attribute* const> Property::getAttributes() const {
	return { attributes.data(), attributes.size() };
}

} // namespace Typhoon::Reflection
