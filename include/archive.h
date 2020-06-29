#pragma once

#include <core/uncopyable.h>
#include <string>

namespace Typhoon {

class ArchiveIterator {
public:
	ArchiveIterator()
	    : node(nullptr) {
	}
	explicit ArchiveIterator(void* node)
	    : node(node) {
	}
	explicit operator bool() const {
		return node != nullptr;
	}
	void* getNode() const {
		return node;
	}
	void reset(void* node_ = nullptr) {
		node = node_;
	}

private:
	void* node;
};

class InputArchive {
public:
	virtual ~InputArchive() = default;

	virtual std::string getErrorDesc() const = 0;
	virtual bool        beginElement() = 0;
	virtual bool        beginElement(const char* name) = 0;
	virtual void        endElement() = 0;
	virtual bool        iterateChild(ArchiveIterator&) = 0;
	virtual bool        iterateChild(ArchiveIterator&, const char* name) = 0;
	virtual bool        readAttribute(const char* name, bool& value) = 0;
	virtual bool        readAttribute(const char* name, char& value) = 0;
	virtual bool        readAttribute(const char* name, short& value) = 0;
	virtual bool        readAttribute(const char* name, int& value) = 0;
	virtual bool        readAttribute(const char* name, unsigned char& value) = 0;
	virtual bool        readAttribute(const char* name, unsigned int& value) = 0;
	virtual bool        readAttribute(const char* name, unsigned short& value) = 0;
	virtual bool        readAttribute(const char* name, float& value) = 0;
	virtual bool        readAttribute(const char* name, double& value) = 0;
	virtual bool        readAttribute(const char* name, const char** str) = 0;
	virtual const char* currNodeText() = 0;

	template <class T>
	T readObject(const char* tag, T&& defaultValue);

	template <class T>
	bool readObject(const char* tag, T& object);
};

class OutputArchive {
public:
	virtual ~OutputArchive() = default;

	virtual bool saveToFile(const char* filename) = 0;
	virtual bool saveToString(std::string& string) = 0;
	virtual bool beginElement(const char* name) = 0;
	virtual void endElement() = 0;

	virtual bool write(const char* data) = 0;

	// Serialization of attributes
	virtual bool writeAttribute(const char* name, bool value) = 0;
	virtual bool writeAttribute(const char* name, char value) = 0;
	virtual bool writeAttribute(const char* name, unsigned char value) = 0;
	virtual bool writeAttribute(const char* name, int value) = 0;
	virtual bool writeAttribute(const char* name, unsigned int value) = 0;
	virtual bool writeAttribute(const char* name, unsigned short value) = 0;
	virtual bool writeAttribute(const char* name, short value) = 0;
	virtual bool writeAttribute(const char* name, float value) = 0;
	virtual bool writeAttribute(const char* name, double value) = 0;
	virtual bool writeAttribute(const char* name, const char* str) = 0;
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

#define ATTRIBUTE(x) NameAttribute(x##, #x)

template <typename T>
bool operator&(InputArchive& archive, const AttributeNamer<T>& namedAttribute) {
	return archive.readAttribute(namedAttribute.name, namedAttribute.objectPtr);
}

class ArchiveElement : Uncopyable {
public:
	ArchiveElement(InputArchive& archive, const char* tagName);
	~ArchiveElement();

	explicit operator bool() const;

private:
	InputArchive* archive;
	bool          isValid;
};

class WriteTag : Uncopyable {
public:
	WriteTag(OutputArchive& archive, const char* element);
	~WriteTag();

	explicit operator bool() const;

private:
	OutputArchive* archive;
	bool           isValid;
};

// Specialized
template <class T>
bool serialize(T& object, InputArchive& archive) {
	static_assert(false, "Not implemented");
}

template <class T>
bool serialize(const T& object, OutputArchive& archive) {
	static_assert(false, "Not implemented");
}

template <class T>
bool serialize(const char* tag, const T& object, OutputArchive& archive) {
	bool res = false;
	if (archive.beginElement(tag)) {
		res = serialize(object, archive);
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
		res = serialize(object, *this);
		endElement();
	}
	return res;
}

} // namespace Typhoon
