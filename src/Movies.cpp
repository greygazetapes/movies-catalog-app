#include "Movies.h"

void Movies::addMovie(const Movie& movie) {
	movieList.push_back(movie);
}

const std::vector<Movie>& Movies::getAllMovies() const {
	return movieList;
}

std::vector<Movie> Movies::getMoviesByGenre(const std::wstring& genre) const {
    std::vector<Movie> filtered;
    for (const auto& movie : movieList) {
        if (movie.hasGenre(genre)) {
            filtered.push_back(movie);
        }
    }
    return filtered;
}