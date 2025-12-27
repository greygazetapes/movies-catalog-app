#include "Movie.h"
#include <algorithm>
#include <locale>
#include <cwctype>

// конструктор и его параметры
Movie::Movie(const std::wstring& title,
	const std::wstring& director,
	int year,
	const std::vector<std::wstring>& genres,
	double rating,
	const std::string& posterPath,
	const std::wstring& description) 
	: title(title),
	director(director),
	year(year),
	genres(genres),
	rating(rating),
	posterPath(posterPath),
	description(description) {}

// геттеры
std::wstring Movie::getTitle() const {
	return title;
}
std::wstring Movie::getDirector() const {
	return director;
}
int Movie::getYear() const {
	return year;
}
std::vector<std::wstring> Movie::getGenres() const {
	return genres;
}
double Movie::getRating() const {
	return rating;
}
std::string Movie::getPosterPath() const {
	return posterPath;
}
std::wstring Movie::getDescription() const {
	return description;
}

// сеттеры
void Movie::setTitle(const std::wstring& t) {
	title = t;
}
void Movie::setDirector(const std::wstring& d) {
	director = d;
}
void Movie::setYear(int y) {
	year = y;
}
void Movie::setGenres(const std::vector<std::wstring>& g) {
	genres = g;
}
void Movie::setRating(double r) {
	rating = r;
}
void Movie::setPosterPath(const std::string& path) {
	posterPath = path;
}
void Movie::setDescription(const std::wstring& desc) {
	description = desc;
}

bool Movie::hasGenre(const std::wstring& genre) const {
	auto toLower = [](std::wstring str) {
		std::wstring res;
		std::locale loc("");
		for (wchar_t c : str) {
			res += std::towlower(c);
		}
		return res;
		};

	std::wstring gLower = toLower(genre);

	for (const auto& g : genres) {
		if (toLower(g) == gLower) {
			return true;
		}
	}
	return false;
}