#pragma once

#include "config.h"
#include <core/uncopyable.h>
#include <cstdint>
#include <string>

namespace Typhoon::Reflection {

class ArchiveIterator {
public:
	ArchiveIterator()
	    : node(nullptr)
	    , index(static_cast<size_t>(-1)) {
	}
	void* getNode() const {
		return node;
	}
	size_t getIndex() const {
		return index;
	}
	void reset() {
		node = nullptr;
		index = static_cast<size_t>(-1);
	}
	void reset(void* node_) {
		node = node_;
	}
	void reset(size_t index_) {
		index = index_;
	}

private:
	void*  node;
	size_t index;
};

struct ParseResult {
	bool        valid;
	const char* errorMessage;
	int         line;

	explicit operator bool() const {
		return valid;
	}
};

class InputArchive {
public:
	virtual ~InputArchive() = default;

	virtual bool beginElement(const char* name) = 0;
	virtual void endElement() = 0;
	virtual bool beginObject() = 0;
	virtual void endObject() = 0;
	virtual bool beginArray() = 0;
	virtual void endArray() = 0;
	virtual bool iterateChild(ArchiveIterator&) = 0;
#if TY_REFLECTION_DEPRECATED
	virtual bool iterateChild(ArchiveIterator&, const char* name) = 0; // TODO remove
#endif
	virtual bool        beginObject(const char* key) = 0;
	virtual bool        beginArray(const char* key) = 0;
	virtual bool        readAttribute(const char* name, bool& value) = 0;
	virtual bool        readAttribute(const char* name, int& value) = 0;
	virtual bool        readAttribute(const char* name, unsigned int& value) = 0;
	virtual bool        readAttribute(const char* name, float& value) = 0;
	virtual bool        readAttribute(const char* name, double& value) = 0;
	virtual bool        readAttribute(const char* name, const char*& str) = 0;
	virtual const char* currNodeText() = 0;

	// New API
	virtual bool readBool(bool& value) = 0;
	virtual bool readInt(const char* key, int& value) = 0;
	virtual bool readUInt(const char* key, unsigned int& value) = 0;
	virtual bool readInt64(const char* key, int64_t& value) = 0;
	virtual bool readUInt64(const char* key, uint64_t& value) = 0;
	virtual bool readFloat(const char* key, float& value) = 0;
	virtual bool readDouble(const char* key, double& value) = 0;
	virtual bool readString(const char* key, const char*& str) = 0;

	template <class T>
	T readObject(const char* tag, T&& defaultValue);

	template <class T>
	bool readObject(const char* tag, T& object);

	template <class T>
	bool readObject(T& object);
};

class OutputArchive {
public:
	virtual ~OutputArchive() = default;

	virtual bool             saveToFile(const char* filename) = 0;
	virtual bool             saveToString(std::string& string) = 0;
	virtual std::string_view getString() = 0;
	virtual bool             beginElement(const char* name) = 0;
	virtual void             endElement() = 0;
	virtual bool             beginObject() = 0;
	virtual void             endObject() = 0;
	virtual bool             beginArray() = 0;
	virtual void             endArray() = 0;
	virtual void             write(const char* data) = 0;

	// New API
	virtual void writeBool(bool value) = 0;
	virtual void writeInt(const char* key, int value) = 0;
	virtual void writeUInt(const char* key, unsigned int value) = 0;
	virtual void writeInt64(const char* key, int64_t value) = 0;
	virtual void writeUInt64(const char* key, uint64_t value) = 0;
	virtual void writeFloat(const char* key, float value) = 0;
	virtual void writeDouble(const char* key, double value) = 0;
	virtual void writeString(const char* key, const char* str) = 0;

	// Serialization of attributes
	virtual void writeAttribute(const char* name, bool value) = 0;
	virtual void writeAttribute(const char* name, int value) = 0;
	virtual void writeAttribute(const char* name, unsigned int value) = 0;
	virtual void writeAttribute(const char* name, float value) = 0;
	virtual void writeAttribute(const char* name, double value) = 0;
	virtual void writeAttribute(const char* name, const char* str) = 0;
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

// Specialized
template <class T>
bool read(T& object, InputArchive& archive) {
	static_assert(false, "Not implemented");
}

template <class T>
bool write(const T& object, OutputArchive& archive) {
	static_assert(false, "Not implemented");
}

template <class T>
bool write(const char* tag, const T& object, OutputArchive& archive) {
	bool res = false;
	if (archive.beginElement(tag)) {
		res = write(object, archive);
		archive.endElement();
	}
	return res;
}

template <class T>
T InputArchive::readObject(const char* tag, T&& defaultValue) {
	T obj = std::move(defaultValue);
	readObject(tag, obj);
	return obj;
}

template <class T>
bool InputArchive::readObject(const char* tag, T& object) {
	bool res = false;
	if (beginElement(tag)) {
		res = read(object, *this);
		endElement();
	}
	return res;
}

template <class T>
bool InputArchive::readObject(T& object) {
	return read(object, *this);
}

} // namespace Typhoon::Reflection
