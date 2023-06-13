#include "property.h"
#include "flags.h"
#include "type.h"
#include <cassert>

namespace Typhoon::Reflection {

Property::Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, uint32_t flags, Semantic semantic, Allocator& allocator)
    : setter { std::move(setter) }
    , getter { std::move(getter) }
    , name { name }
    , valueType { valueType }
    , flags { flags }
    , semantic { semantic }
    , attributes { stdAllocator<const Attribute*>(allocator) } {
	assert(valueType);
	// Override flags
	if (! this->setter) {
		this->flags &= ~Flags::readable;
		this->flags &= ~Flags::edit;
		this->flags &= ~Flags::clonable;
	}
	if (! this->getter) {
		this->flags &= ~Flags::writeable;
		this->flags &= ~Flags::clonable;
		this->flags &= ~Flags::view;
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
	assert(setter);
	setter(self, value);
}

void Property::getValue(ConstDataPtr self, DataPtr value) const {
	assert(getter);
	getter(self, value);
}

void Property::copyValue(DataPtr dstSelf, ConstDataPtr srcSelf, LinearAllocator& alloc) const {
	assert(setter);
	assert(getter);
	void* allocOffs = alloc.getOffset();
	if (void* temporary = alloc.alloc(valueType->getSize(), valueType->getAlignment()); temporary) {
		valueType->constructObject(temporary);
		ConstDataPtr value = getter(srcSelf, temporary);
		setter(dstSelf, value);
		valueType->destructObject(temporary);
	}
	alloc.rewind(allocOffs);
}

void Property::addAttribute(const Attribute* attribute) {
	attributes.push_back(attribute);
}

span<const Attribute* const> Property::getAttributes() const {
	return { attributes.data(), attributes.size() };
}

} // namespace Typhoon::Reflection
