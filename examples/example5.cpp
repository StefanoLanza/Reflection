// Example showing reflection and serialization of containers: array, vector and map

#include "customAllocator.h"
#include "utils.h"
#include <reflection/reflection.h>
#include <reflection/version.h>
#include <iostream>
#include <map>
#include <string>

#define XML  0
#define JSON 1
#if TY_REFLECTION_JSON
#define ARCHIVE_TYPE JSON
#elif TY_REFLECTION_XML
#define ARCHIVE_TYPE XML
#else
#error "No supported archive types"
#endif

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
using NumberVector = std::array<int, 6>;

void         registerUserTypes();
PersonMap    buildPersonMap();
CityVector   buildCities();
NumberVector buildPrimeNumbers();
std::string  writeToArchive(const PersonMap& persons, const CityVector& cities, const NumberVector& numbers, const char* personsKey,
                            const char* citiesKey, const char* numbersKey);
void         readFromArchive(PersonMap& persons, CityVector& cities, NumberVector& numbers, const std::string& archiveContent, const char* personsKey,
                             const char* citiesKey, const char* numbersKey);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	CustomAllocator customAllocator;
	refl::initReflection(customAllocator);
	registerUserTypes();
	printNamespace(refl::getGlobalNamespace());

	PersonMap    persons = buildPersonMap();
	CityVector   cities = buildCities();
	NumberVector primeNumbers = buildPrimeNumbers();

	const char* personsKey = "Persons";
	const char* citiesKey = "Cities";
	const char* numbersKey = "PrimeNumbers";

	// Write containers
	const std::string archiveContent = writeToArchive(persons, cities, primeNumbers, personsKey, citiesKey, numbersKey);

	// Read containers
	PersonMap    otherPersons;
	CityVector   otherCities;
	NumberVector otherNumbers;
	readFromArchive(otherPersons, otherCities, otherNumbers, archiveContent, personsKey, citiesKey, numbersKey);

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

NumberVector buildPrimeNumbers() {
	return { 2, 3, 5, 7, 11, 13 };
}

std::string writeToArchive(const PersonMap& persons, const CityVector& cities, const NumberVector& numbers, const char* personsKey,
                           const char* citiesKey, const char* numbersKey) {
	std::string archiveContent;
#if ARCHIVE_TYPE == XML
	refl::XMLOutputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONOutputArchive archive;
#endif
	archive.write(personsKey, persons);
	archive.write(citiesKey, cities);
	archive.write(numbersKey, numbers);
	archive.writeAttribute("bool", true);
	archive.writeAttribute("int", 13);
	archive.writeAttribute("float", 3.141);
	archive.writeAttribute("double", 31.1312312312312312);
	archive.saveToString(archiveContent);
	return archiveContent;
}

void readFromArchive(PersonMap& persons, CityVector& cities, NumberVector& numbers, const std::string& archiveContent, const char* personsKey,
                     const char* citiesKey, const char* numbersKey) {
#if ARCHIVE_TYPE == XML
	refl::XMLInputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONInputArchive  archive;
#endif
	if (archive.initialize(archiveContent.data())) {
		archive.read(personsKey, persons);
		archive.read(citiesKey, cities);
		archive.read(numbersKey, numbers);
	}
	bool   b = false;
	int    i = 0;
	float  f = 0.f;
	double d = 0.;
	archive.readAttribute("bool", b);
	archive.readAttribute("int", i);
	archive.readAttribute("float", f);
	archive.readAttribute("double", d);
}
