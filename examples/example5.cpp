// Example showing reflection and serialization of containers: arrays and maps

#include "customAllocator.h"
#include <include/reflection.h>
#include <include/version.h>
#include <iostream>
#include <map>
#include <string>

#define XML          0
#define JSON         1
#define ARCHIVE_TYPE XML

struct Person {
	std::string name;
	int         age;
};

// Needed for std::map
bool operator==(const Person& a, const Person& b) {
	return a.name == b.name && a.age == b.age;
}

struct City {
	std::string name;
	std::string country;
};

// Needed for std::map
bool operator==(const City& a, const City& b) {
	return a.name == b.name && a.country == b.country;
}

using PersonId = uint32_t;
using PersonMap = std::map<PersonId, Person>;
using CityVector = std::vector<City>;

void        registerUserTypes();
PersonMap   buildPersonMap();
CityVector  buildCities();
std::string writeToArchive(const PersonMap& obj, const CityVector& cities, const char* name, const char* citiesName);
void        readFromArchive(PersonMap& obj, CityVector& cities, const std::string& archiveContent, const char* name, const char* citiesName);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	CustomAllocator customAllocator;
	refl::initReflection(customAllocator);
	registerUserTypes();

	PersonMap  persons = buildPersonMap();
	CityVector cities = buildCities();

	const char* name = "Persons";
	const char* citiesName = "Cities";
	std::string archiveContent = writeToArchive(persons, cities, name, citiesName);
	PersonMap   otherPersons;
	CityVector  otherCities;
	readFromArchive(otherPersons, otherCities, archiveContent, name, citiesName);

	std::cout << "Total allocated memory " << customAllocator.getTotalAlloc() << " bytes" << std::endl;
	refl::deinitReflection();
	return 0;
}

void registerUserTypes() {
	BEGIN_REFLECTION()

	BEGIN_STRUCT(Person);
	FIELD(name);
	FIELD(age);
	END_STRUCT();

	BEGIN_STRUCT(City);
	FIELD(name);
	FIELD(country);
	END_STRUCT();

	END_REFLECTION();
}

PersonId nextId() {
	static PersonId id = 1;
	return id++;
}

PersonMap buildPersonMap() {
	PersonMap persons;
	persons.emplace(nextId(), Person { "Stefano", 41 });
	persons.emplace(nextId(), Person { "Enrico", 12 });
	persons.emplace(nextId(), Person { "Oriana", 7 });
	return persons;
}

CityVector buildCities() {
	CityVector cities;
	cities.push_back(City { "Mantova", "Italy" });
	cities.push_back(City { "Barcelona", "Spain" });
	cities.push_back(City { "Dublin", "Ireland" });
	cities.push_back(City { "Cambridge", "UK" });
	return cities;
}

std::string writeToArchive(const PersonMap& obj, const CityVector& cities, const char* name, const char* citiesName) {
	std::string archiveContent;
#if ARCHIVE_TYPE == XML
	refl::XMLOutputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONOutputArchive archive;
#endif
	archive.beginRoot();
	writeObject(obj, name, archive);
	writeObject(cities, citiesName, archive);
	archive.writeAttribute("bool", true);
	archive.writeAttribute("int", 13);
	archive.writeAttribute("float", 3.141);
	archive.writeAttribute("double", 31.1312312312312312);
	archive.endRoot();
	archive.saveToString(archiveContent);
	return archiveContent;
}

void readFromArchive(PersonMap& obj, CityVector& cities, const std::string& archiveContent, const char* name, const char* citiesName) {
#if ARCHIVE_TYPE == XML
	refl::XMLInputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONInputArchive  archive;
#endif
	if (archive.initialize(archiveContent.data())) {
		readObject(&obj, name, archive);
		readObject(&cities, citiesName, archive);
	}
	bool b = false;
	int i = 0;
	float f = 0.f;
	double d = 0.;
	archive.readAttribute("bool", b);
	archive.readAttribute("int", i);
	archive.readAttribute("float", f);
	archive.readAttribute("double", d);
}
