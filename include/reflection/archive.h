#pragma once

#include "config.h"
#include "readObject.h"
#include "writeObject.h"
#include <core/uncopyable.h>

#include <cstdint>
#include <string>

namespace Typhoon::Reflection {

class ArchiveIterator {
public:
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
	void*       node = nullptr;
	size_t      index = (size_t)-1;
	const char* key = nullptr;
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

	virtual bool beginElement(const char* name) const = 0;
	virtual void endElement() const = 0;
	virtual bool isObject() const = 0;
	virtual bool isArray() const = 0;
	virtual bool iterateChild(ArchiveIterator& it) const = 0;
	virtual bool iterateChild(ArchiveIterator& it, const char* name) const = 0;
	virtual bool readAttribute(const char* name, bool& value) const = 0;
	virtual bool readAttribute(const char* name, int& value) const = 0;
	virtual bool readAttribute(const char* name, unsigned int& value) const = 0;
	virtual bool readAttribute(const char* name, float& value) const = 0;
	virtual bool readAttribute(const char* name, double& value) const = 0;
	virtual bool readAttribute(const char* name, const char*& str) const = 0;
	virtual bool readAttribute(const char* name, std::string_view& sv) const = 0;
	// Primitives
	virtual bool read(bool& value) const = 0;
	virtual bool read(int& value) const = 0;
	virtual bool read(unsigned int& value) const = 0;
	virtual bool read(int64_t& value) const = 0;
	virtual bool read(uint64_t& value) const = 0;
	virtual bool read(float& value) const = 0;
	virtual bool read(double& value) const = 0;
	virtual bool read(const char*& str) const = 0;
	virtual bool read(std::string_view& sv) const = 0;

	//  Helpers
	bool beginObject(const char* key) const;
	bool beginArray(const char* key) const;
	bool read(const char* key, void* data, TypeId typeId) const;
	bool read(void* data, TypeId typeId) const;

	// Read data of user-defined type
	template <class T>
	bool read(T& object) const;

	template <class T>
	T read(const char* key, T&& defaultValue) const;

	template <class T>
	T read(T&& defaultValue) const;

	template <class T>
	bool read(const char* key, T& object) const;

private:
	bool readAny(void* data, const Type& type) const;

private:
	Context& context;
};

class OutputArchive : Uncopyable {
public:
	OutputArchive();
	virtual ~OutputArchive() = default;

	virtual bool        saveToFile(const char* filename) = 0;
	virtual std::string saveToString() = 0;
	virtual void        setKey(const char* key) = 0;
	virtual bool        beginObject() = 0;
	virtual void        endObject() = 0;
	virtual bool        beginArray() = 0;
	virtual void        endArray() = 0;
	virtual void        write(bool value) = 0;
	virtual void        write(int value) = 0;
	virtual void        write(unsigned int value) = 0;
	virtual void        write(int64_t value) = 0;
	virtual void        write(uint64_t value) = 0;
	virtual void        write(float value) = 0;
	virtual void        write(double value) = 0;
	virtual void        write(const char* str) = 0;
	virtual void        write(std::string_view str) = 0;

	// Serialization of attributes
	virtual void writeAttribute(const char* name, bool value) = 0;
	virtual void writeAttribute(const char* name, int value) = 0;
	virtual void writeAttribute(const char* name, unsigned int value) = 0;
	virtual void writeAttribute(const char* name, float value) = 0;
	virtual void writeAttribute(const char* name, double value) = 0;
	virtual void writeAttribute(const char* name, const char* str) = 0;

	void write(const char* key, const void* data, TypeId typeId);
	void write(const void* data, TypeId typeId);

	// Write data of user-defined type
	template <class T>
	void write(const T& data);

	// Helpers
	void write(const char* key, const char* str);

	template <class T>
	void write(const char* key, const T& data);

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

template <class T>
T InputArchive::read(const char* key, T&& defaultValue) const {
	T obj = std::move(defaultValue);
	read(key, obj);
	return obj;
}

template <class T>
T InputArchive::read(T&& defaultValue) const {
	T obj = std::move(defaultValue);
	read(obj);
	return obj;
}

template <class T>
bool InputArchive::read(const char* key, T& object) const {
	bool res = false;
	if (beginElement(key)) {
		res = read(object);
		endElement();
	}
	return res;
}

template <class T>
bool InputArchive::read(T& object) const {
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		type = detail::autoRegisterHelper<T>::autoRegister(context);
	}
	if (type) {
		return readAny(static_cast<void*>(&object), *type);
	}
	return false;
}

template <class T>
void OutputArchive::write(const char* key, const T& data) {
	setKey(key);
	write(data);
}

template <class T>
void OutputArchive::write(const T& data) {
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		type = detail::autoRegisterHelper<T>::autoRegister(context);
	}
	assert(type);
	detail::writeData(static_cast<const void*>(&data), *type, *this, context);
}

class ArrayReadScope : Uncopyable {
public:
	ArrayReadScope(InputArchive& archive, const char* key);
	ArrayReadScope(InputArchive& archive);
	~ArrayReadScope();
	operator bool() const;

private:
	InputArchive& archive;
	bool          hasKey;
	bool          isValid;
};

class ObjectReadScope : Uncopyable {
public:
	ObjectReadScope(const InputArchive& archive, const char* key);
	ObjectReadScope(const InputArchive& archive);
	~ObjectReadScope();
	operator bool() const;

private:
	const InputArchive& archive;
	bool                hasKey;
	bool                isValid;
};

class ArrayWriteScope : Uncopyable {
public:
	ArrayWriteScope(OutputArchive& archive, const char* key = nullptr);
	~ArrayWriteScope();

private:
	OutputArchive& archive;
};

class ObjectWriteScope : Uncopyable {
public:
	ObjectWriteScope(OutputArchive& archive, const char* key = nullptr);
	~ObjectWriteScope();

private:
	OutputArchive& archive;
};

} // namespace Typhoon::Reflection
