#pragma once
#include "Movie.h"
#include <vector>

class Movies {
private:
	std::vector<Movie> movieList;

public:
	void addMovie(const Movie& movie);
	const std::vector<Movie>& getAllMovies() const;
	const std::vector<Movie>& getAll() const {
		return movieList;
	}

	std::vector<Movie> getMoviesByGenre(const std::wstring& genre) const;
};

