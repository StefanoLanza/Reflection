#pragma once

#include "config.h"
#include <core/uncopyable.h>
#include <cstdint>
#include <string>

#include "readObject.h"
#include "writeObject.h"

namespace Typhoon::Reflection {

class ArchiveIterator {
public:
	ArchiveIterator()
	    : node(nullptr)
	    , index(static_cast<size_t>(-1))
	    , key(nullptr) {
	}
	void* getNode() const {
		return node;
	}
	void setNode(void* node_) {
		node = node_;
	}
	size_t getIndex() const {
		return index;
	}
	bool hasValidIndex() const {
		return index != static_cast<size_t>(-1);
	}
	void setIndex(size_t index_) {
		index = index_;
	}
	void reset() {
		node = nullptr;
		index = static_cast<size_t>(-1);
	}
	void setKey(const char* key_) {
		key = key_;
	}
	const char* getKey() const {
		return key;
	}

private:
	void*  node;
	size_t index;
	const char* key;
};

struct ParseResult {
	bool        valid;
	const char* errorMessage;
	int         line;

	explicit operator bool() const {
		return valid;
	}
};

class InputArchive : Uncopyable {
public:
	InputArchive();
	virtual ~InputArchive() = default;

	virtual bool beginElement(const char* name) = 0;
	virtual void endElement() = 0;
	virtual bool beginObject() = 0;
	virtual void endObject() = 0;
	virtual bool beginArray() = 0;
	virtual void endArray() = 0;
	virtual bool iterateChild(ArchiveIterator&) = 0;
	virtual bool beginObject(const char* key) = 0;
	virtual bool beginArray(const char* key) = 0;
	virtual bool readAttribute(const char* name, bool& value) = 0;
	virtual bool readAttribute(const char* name, int& value) = 0;
	virtual bool readAttribute(const char* name, unsigned int& value) = 0;
	virtual bool readAttribute(const char* name, float& value) = 0;
	virtual bool readAttribute(const char* name, double& value) = 0;
	virtual bool readAttribute(const char* name, const char*& str) = 0;
	virtual bool readAttribute(const char* name, std::string_view& sv) = 0;
	// Primitives
	virtual bool read(bool& value) = 0;
	virtual bool read(int& value) = 0;
	virtual bool read(unsigned int& value) = 0;
	virtual bool read(int64_t& value) = 0;
	virtual bool read(uint64_t& value) = 0;
	virtual bool read(float& value) = 0;
	virtual bool read(double& value) = 0;
	virtual bool read(const char*& str) = 0;
	virtual bool read(std::string_view& sv) = 0;

	bool read(const char* key, void* data, TypeId typeId);
	bool read(void* data, TypeId typeId);

	// Read data of user-defined type
	template <class T>
	bool read(T& object);

	template <class T>
	T read(const char* key, T&& defaultValue);

	template <class T>
	bool read(const char* key, T& object);

private:
	Context& context;
};

class OutputArchive : Uncopyable {
public:
	OutputArchive();
	virtual ~OutputArchive() = default;

	virtual bool             saveToFile(const char* filename) = 0;
	virtual bool             saveToString(std::string& string) = 0;
	virtual std::string_view getString() = 0;
	virtual bool beginElement(const char* key) = 0;
	virtual void endElement() = 0;
	virtual bool             beginObject() = 0;
	virtual void             endObject() = 0;
	virtual bool             beginArray() = 0;
	virtual void             endArray() = 0;
	virtual bool write(bool value) = 0;
	virtual bool write(int value) = 0;
	virtual bool write(unsigned int value) = 0;
	virtual bool write(int64_t value) = 0;
	virtual bool write(uint64_t value) = 0;
	virtual bool write(float value) = 0;
	virtual bool write(double value) = 0;
	virtual bool write(const char* str) = 0;
	virtual bool write(std::string_view str) = 0;

	// Serialization of attributes
	virtual void writeAttribute(const char* name, bool value) = 0;
	virtual void writeAttribute(const char* name, int value) = 0;
	virtual void writeAttribute(const char* name, unsigned int value) = 0;
	virtual void writeAttribute(const char* name, float value) = 0;
	virtual void writeAttribute(const char* name, double value) = 0;
	virtual void writeAttribute(const char* name, const char* str) = 0;

	bool write(const char* key, const void* data, TypeId typeId);
	bool write(const void* data, TypeId typeId);

	// Write data of user-defined type
	template <class T>
	bool write(const T& data);

	// Helpers
	bool write(const char* key, const char* str);

	template <class T>
	bool write(const char* key, const T& data);
private:
	Context& context;
};

template <typename T>
struct AttributeNamer {
	const char* name;
	T*          objectPtr;
};

template <typename T>
AttributeNamer<T> NameAttribute(T& object, const char* name) {
	return { name, &object };
}

#define ATTRIBUTE(x) Typhoon::Reflection::NameAttribute(x##, #x)

template <typename T>
bool operator&(InputArchive& archive, const AttributeNamer<T>& namedAttribute) {
	return archive.readAttribute(namedAttribute.name, namedAttribute.objectPtr);
}

class ArchiveElement : Uncopyable {
public:
	ArchiveElement(InputArchive& archive, const char* tag);
	~ArchiveElement();

	explicit operator bool() const;

private:
	InputArchive* archive;
	bool          isValid;
};

class WriteTag : Uncopyable {
public:
	WriteTag(OutputArchive& archive, const char* tag);
	~WriteTag();

	explicit operator bool() const;

private:
	OutputArchive* archive;
	bool           isValid;
};

template <class T>
T InputArchive::read(const char* key, T&& defaultValue) {
	T obj = std::move(defaultValue);
	read(key, obj);
	return obj;
}

template <class T>
bool InputArchive::read(const char* key, T& object) {
	bool res = false;
	if (beginElement(key)) {
		res = read(object);
		endElement();
	}
	return res;
}

template <class T>
bool InputArchive::read(T& object) {
	return read(static_cast<void*>(&object), getTypeId<T>());
}

template <class T>
bool OutputArchive::write(const char* key, const T& data) {
	bool res = false;
	if (beginElement(key)) {
		 res = write(data);
		 endElement();
	}
	return res;
}

template <class T>
bool OutputArchive::write(const T& data) {
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		// TODO Allocate a temporary Type, without registering it ?
		type = detail::autoRegisterHelper<T>::autoRegister(context);
	}
	if (type) {
		return detail::writeData(static_cast<const void*>(&data), *type, *this, context);
	}
	else {
		return false;
	}
}

} // namespace Typhoon::Reflection
