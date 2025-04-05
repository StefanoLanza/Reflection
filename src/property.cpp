#include "property.h"
#include "flags.h"
#include "type.h"
#include <cassert>

namespace Typhoon::Reflection {

Property::Property(Setter&& setter_, Getter&& getter_, const char* name, const Type* valueType, Allocator& allocator)
    : setter { std::move(setter_) }
    , getter { std::move(getter_) }
    , name { name }
    , prettyName { name }
    , valueType { valueType }
    , flags { Flags::all }
    , semantic { Semantic::none }
    , attributes { stdAllocator<const Attribute*>(allocator) } {
	assert(valueType);
	// Override flags
	if (! setter) {
		flags &= ~Flags::readable;
		flags &= ~Flags::edit;
		flags &= ~Flags::clonable;
	}
	if (! getter) {
		flags &= ~Flags::writeable;
		flags &= ~Flags::clonable;
		flags &= ~Flags::view;
	}
}

const char* Property::getName() const {
	return name;
}

const char* Property::getPrettyName() const {
	return prettyName;
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

Property& Property::setPrettyName(const char* str) {
	assert(str);
	prettyName = str;
	return *this;
}

Property& Property::setFlags(uint32_t value) {
	flags = value;
	return *this;
}

Property& Property::setSemantic(Semantic value) {
	semantic = value;
	return *this;
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

Property& Property::addAttribute(const Attribute* attribute) {
	attributes.push_back(attribute);
	return *this;
}

std::span<const Attribute* const> Property::getAttributes() const {
	return attributes;
}

} // namespace Typhoon::Reflection
