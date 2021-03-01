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

void Property::setValue(DataPtr self, ConstDataPtr value) const {
	setter(self, value);
}

void Property::getValue(ConstDataPtr self, DataPtr value) const {
	getter(self, value);
}

void Property::copyValue(DataPtr dstSelf, ConstDataPtr srcSelf, LinearAllocator& alloc) const {
	void* allocOffs = alloc.getOffset();
	void* temporary = alloc.alloc(valueType->getSize(), valueType->getAlignment());
	assert(temporary);
	if (temporary) {
		valueType->constructObject(temporary);
		const void* value = getter(srcSelf, temporary);
		setter(dstSelf, value);
		valueType->destructObject(temporary);
		alloc.rewind(allocOffs);
	}
}

void Property::addAttribute(const Attribute* attribute) {
	attributes.push_back(attribute);
}

span<const Attribute* const> Property::getAttributes() const {
	return { attributes.data(), attributes.size() };
}

} // namespace Typhoon::Reflection
