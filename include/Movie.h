#pragma once
#include <string>
#include <iostream>
#include <vector>

class Movie{
// приватный модификатор доступа
private:
	std::wstring title; // название фильма
	std::wstring director; // режиссер
	int year; // год выпуска
	std::vector<std::wstring> genres; // жанр
	double rating; // рейтинг
	std::string posterPath; // путь к изображению
	std::wstring description; // описание

// публичный модификатор доступа
public:
	// конструкторы
	Movie() = default;
	Movie(const std::wstring& title, 
		const std::wstring& director, 
		int year, 
		const std::vector<std::wstring>& genres, 
		double rating, 
		const std::string& posterPath,
		const std::wstring& description);

	// геттеры - получать
	std::wstring getTitle() const;
	std::wstring getDirector() const;
	int getYear() const;
	std::vector<std::wstring> getGenres() const;
	double getRating() const;
	std::string getPosterPath() const;
	std::wstring getDescription() const;

	// сеттеры - устанавливать
	void setTitle(const std::wstring& title);
	void setDirector(const std::wstring& director);
	void setYear(int year);
	void setGenres(const std::vector<std::wstring>& genres);
	void setRating(double rating);
	void setPosterPath(const std::string& path);
	void setDescription(const std::wstring& description);

	bool hasGenre(const std::wstring& genre) const;
};

