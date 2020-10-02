#include "stdVectorType.h"

namespace Typhoon::Reflection::detail {

const char* decorateTypeName(const char* typeName, const char* prefix, const char* suffix, ScopedAllocator& alloc) {
	assert(typeName);
	assert(prefix);
	assert(suffix);
	size_t tl = std::strlen(typeName);
	size_t pl = std::strlen(prefix);
	size_t sl = std::strlen(suffix);
	char*  str = static_cast<char*>(alloc.alloc(pl + sl + tl + 1, alignof(char)));
	size_t offs = 0;
	memcpy(str + offs, prefix, pl);
	offs += pl;
	memcpy(str + offs, typeName, tl);
	offs += tl;
	memcpy(str + offs, suffix, sl);
	offs += sl;
	str[offs] = 0; // null terminate
	return str;
}

const char* buildTemplateTypeName(const char* typeNames[], const char* prefix, const char* suffix, ScopedAllocator& alloc) {
	size_t tl = 0;
	
	for (int i = 0; typeNames[i]; ++i) {
		tl += std::strlen(typeNames[i]) + 1; // + 1: ,
	}
	--tl; // remove last comma
	size_t pl = std::strlen(prefix);
	size_t sl = std::strlen(suffix);
	char*  str = static_cast<char*>(alloc.alloc(pl + sl + tl + 1, alignof(char)));
	size_t offs = 0;
	memcpy(str + offs, prefix, pl);
	offs += pl;
	for (int i = 0; typeNames[i]; ++i) {
		size_t l = strlen(typeNames[i]);
		memcpy(str + offs, typeNames[i], l);
		str[offs + l] = ',';
		offs += l + 1;
	}
	--offs;
	memcpy(str + offs, suffix, sl);
	offs += sl;
	str[offs] = 0; // null terminate
	return str;
}

} // namespace Typhoon::Reflection::detail
