#include "MovieData.h"
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>


void fillMovies(Movies& movies, Database& db) {
    sql::Connection* conn = db.getConnection();

    try {
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());

        // собираем жанры через GROUP_CONCAT и используем LEFT JOIN чтобы не потерять фильмы без жанров
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT m.id, m.title, m.director, m.year, m.rating, m.description, m.poster_path, "
            "GROUP_CONCAT(g.name SEPARATOR ',') AS genres "
            "FROM movies m "
            "LEFT JOIN movie_genres mg ON m.id = mg.movie_id "
            "LEFT JOIN genres g ON mg.genre_id = g.id "
            "GROUP BY m.id, m.title, m.director, m.year, m.rating, m.description, m.poster_path"
        ));

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        int loaded = 0;

        while (res->next()) {
            std::string title = res->getString("title");
            std::string director = res->getString("director");
            int year = res->getInt("year");
            double rating = res->getDouble("rating");
            std::string poster = res->getString("poster_path");
            std::string description = res->getString("description");

            std::string genres_s;
            try {
                // GROUP_CONCAT может вернуть NULL если жанров нет - getString вернёт empty string или бросит,
                // поэтому ловим исключение на всякий случай
                genres_s = res->getString("genres");
            }
            catch (...) {
                genres_s.clear();
            }

            std::wstring wtitle = conv.from_bytes(title);
            std::wstring wdirector = conv.from_bytes(director);
            std::wstring wdescription = conv.from_bytes(description);

            std::vector<std::wstring> genres_vec;
            if (!genres_s.empty()) {
                std::stringstream ss(genres_s);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    size_t start = item.find_first_not_of(" \t");
                    size_t end = item.find_last_not_of(" \t");
                    if (start == std::string::npos) continue;
                    std::string trimmed = item.substr(start, end - start + 1);
                    genres_vec.push_back(conv.from_bytes(trimmed));
                }
            }

            Movie m(
                wtitle,
                wdirector,
                year,
                genres_vec,
                rating,
                poster,
                wdescription
            );

            movies.addMovie(m);
            ++loaded;
        }

        std::cout << "fillMovies: loaded " << loaded << " movies\n";

    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL error in fillMovies: " << e.what()
            << " (code: " << e.getErrorCode()
            << ", state: " << e.getSQLState() << ")\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in fillMovies: " << e.what() << std::endl;
    }
}