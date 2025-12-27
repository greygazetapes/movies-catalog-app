#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "MovieData.h"
#include "Movie.h"
#include "Movies.h"
#include "sql_database.h"
#include <cwctype>
#include <locale>

using namespace sf; 

// (1.f) функция окна загрузки (пер-е: load_scr_win, font2)
static void show_load_scr(sf::RenderWindow& load_scr_win, sf::Font& font1) {
	sf::Text load_scr_text;

	load_scr_text.setFont(font1);
	load_scr_text.setString(L"Загрузка...");
	load_scr_text.setCharacterSize(36);
	load_scr_text.setFillColor(Color::White);
	load_scr_text.setPosition(100, 100);

    // центрирование текста по центру окна
    sf::FloatRect text_rectangle = load_scr_text.getLocalBounds();
        //задаем опорную точку по центру текста
    load_scr_text.setOrigin(text_rectangle.left + text_rectangle.width / 2.0f, text_rectangle.top + text_rectangle.height / 2.0f);
        //задаем позицию по центру окна, сдвинутую вверх на 30 пикселей
    load_scr_text.setPosition(load_scr_win.getSize().x / 2.0f, load_scr_win.getSize().y / 2.0f - 30.f);

	load_scr_win.clear(Color(30, 30, 30));
	load_scr_win.draw(load_scr_text); 
	load_scr_win.display();

	// задержка на 2 сек
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

// (2.f) функция для переноса текста по ширине
std::wstring wrapText(const std::wstring& str, Font& font2, unsigned int char_size, float max_width) {
	sf::Text text;
	std::wstring result;
	std::wstring cur_line;
	std::wstring cur_word;

	text.setFont(font2);
	text.setCharacterSize(char_size);

	for (size_t i = 0; i < str.size(); ++i) {
		// c - каждый символ строки
		wchar_t c = str[i];
		// если символ - либо пробел, либо перевод строки, либо последний символ строки
		if (c == L' ' || c == L'\n' || i == str.size() - 1) {

			// ПРОВЕРКА: если символ - последняя строка, не пробел и не перевод строки, то добавить в текущее слово
			if (i == str.size() - 1 && c != L' ' && c != L'\n')
				cur_word += c;

			// ПОСЛЕ ПРОВЕРКИ: временная строка (влезет ли слово)
			std::wstring temp_line = cur_line + cur_word + L' ';
			// добавляем в text строку
			text.setString(temp_line);

			// ЕСЛИ СТРОКА не помещается в ограниченную ширину
			if (text.getLocalBounds().width > max_width) {

				// слово не влезло, но в строке что-то есть --- перенос
				if (!cur_line.empty()) {
					result += cur_line + L'\n';
					cur_line = cur_word + L' ';
				}
				// слово само по себе не влезло - перенос и очистка временной строки
				else {
					result += cur_word + L'\n';
					cur_line.clear();
				}
			}

			else {
			// ВЛЕЗЛО: просто запомнить временную строку как текущую
				cur_line = temp_line;
			}
			cur_word.clear();

			// ПРОВЕРКА: наличие в символе уже готового переноса строки
			if (c == L'\n') {
				result += cur_line;
				cur_line.clear();
			}
		}
		// ЕСЛИ: символ не пробел и не перенос строки
		else {
			cur_word += c;
		}
	}
	// ДОБАВЛЕНИЕ последней строки в текущую
	result += cur_line;
	return result;
}


int main() {
    // подключение к базе данных
    Database db("ПУТЬ", "ИМЯ", "ВАШ_ПАРОЛЬ", "ИМЯ_БАЗЫ_ДАННЫХ");
    // проверка подключения
    if (!db.isConnected()) {
        std::cerr << "База данных MySQL недоступна" << std::endl;
        return 1; // завершить
    }

    // создание объекта для работы с фильмами
    Movies movies;

    // загрузка фильмов из db
    fillMovies(movies, db);

// предзагрузка постеров с фильмами
	// получение всех данных о фильмах
	std::vector<Movie> all_movies = movies.getAllMovies();
	// создание вектора с текстурами (постерами)
	std::vector<Texture> poster_textures(all_movies.size());
	// загрузка постеров фильмов в текстуру
    for (size_t i = 0; i < all_movies.size(); ++i) {
        poster_textures[i].loadFromFile(all_movies[i].getPosterPath());
    }

	// установка локали для поддержки кириллицы
    std::locale::global(std::locale(""));

    //установка окна
	sf::RenderWindow window(VideoMode(1400, 900), "Film Catalog", Style::Close);
	window.setPosition(Vector2i(260, 50));

    // шрифт 1 -- основной для текста
	sf::Font font1;
	if (!font1.loadFromFile("C:/Users/neilp_57eokcq/OneDrive/Documents/Fonts/Helvetica-Neue Cond.ttf")) {
		return -1;
	}
    // шрифт 2 -- для заголовков
	sf::Font font2;
	if (!font2.loadFromFile("C:/Users/neilp_57eokcq/OneDrive/Documents/Fonts/MTSWide-Medium-MH5A5AEJ.ttf")) {
		return -1;
	} 

	//ВЕРХНЯЯ ПАНЕЛЬ
    sf::VertexArray top_pan(sf::Quads, 4);
    top_pan[0].position = Vector2f(0, 0);
    top_pan[1].position = Vector2f(1400, 0);
    top_pan[2].position = Vector2f(1400, 45);
    top_pan[3].position = Vector2f(0, 45);
    top_pan[0].color = Color(23, 23, 23);
    top_pan[1].color = Color(40, 40, 40);
    top_pan[2].color = Color(23, 23, 23);
    top_pan[3].color = Color(27, 27, 27);

    // кнопки сортировки
    sf::Text sort_year("Сортировать по году", font1, 21);
    sort_year.setPosition(1200, 8);
    sort_year.setFillColor(Color::White);
    sf::Text sort_rating("Сортировать по рейтингу", font1, 21);
    sort_rating.setPosition(950, 8);
    sort_rating.setFillColor(Color::White);

	// отслеживание типа сортировки
	enum class sort_type { None, year, rating };
	sort_type current_sort = sort_type::None;

	// сохранение оригинального порядка фильмов
	std::vector<Movie> original_movies = all_movies;
	std::vector<Texture> original_textures = poster_textures;

    //ЛЕВАЯ ПАНЕЛЬ
    sf::VertexArray left_pan(sf::Quads, 4);
    left_pan[0].position = Vector2f(0, 0);
    left_pan[1].position = Vector2f(200, 0);
    left_pan[2].position = Vector2f(200, 900);
    left_pan[3].position = Vector2f(0, 900);
    left_pan[0].color = Color(23, 23, 23);
    left_pan[1].color = Color(40, 40, 40);
    left_pan[2].color = Color(23, 23, 23);
    left_pan[3].color = Color(27, 27, 27);
	// поле поиска
	sf::RectangleShape search_box(Vector2f(160, 30));
	search_box.setFillColor(Color(50, 50, 50));
	search_box.setOutlineColor(Color(100, 100, 100));
	search_box.setOutlineThickness(1);
	search_box.setPosition(20, 85);
	// текст поля поиска
	sf::Text search_text;
	search_text.setFont(font1);
	search_text.setCharacterSize(16);
	search_text.setFillColor(Color(200, 200, 200));
	search_text.setPosition(25, 88);
	// куда записывается ВВЕДЕННЫЙ ТЕКСТ
	std::wstring search_query;

    // настройка жанров
	std::vector<std::wstring> genres = { L"Все фильмы", L"Драма", L"Комедия", L"Боевик", L"Ужасы", L"Фэнтези", L"Саспенс", L"Мистерия", L"Нуар", L"Вестерн", L"Детектив", L"Мелодрама" };
	std::vector<sf::Text> genre_texts;
	std::wstring selected_genre = L"Все фильмы";
	float scroll_offset = 0.f;
	
	for (int i = 0; i < (int)genres.size(); ++i) {
		Text text;
		text.setFont(font1);
		text.setString(genres[i]);
		text.setCharacterSize(18);
		text.setFillColor(Color::White);
		text.setPosition(20, 160 + i * 30);
		genre_texts.push_back(text);
	}
   
    //КНОПКА "ФИЛЬМ НА ВЕЧЕР"
    bool show_rand_movie = false;
    int random_movie_index = -1;

    sf::Text random_movie("Фильм на вечер", font2, 17);
    random_movie.setPosition(20, 858);
    random_movie.setFillColor(Color(40, 40, 40));

    float x_rand_mov_but = 0.f;
    float y_rand_mov_but = 840.f;
    float rand_mov_width = 200.f;
    float rand_mov_height = 100.f;

    sf::VertexArray random_movie_button(sf::Quads, 4);
    random_movie_button[0].position = Vector2f(x_rand_mov_but, y_rand_mov_but);
    random_movie_button[1].position = Vector2f(x_rand_mov_but + rand_mov_width, y_rand_mov_but);
    random_movie_button[2].position = Vector2f(x_rand_mov_but + rand_mov_width, y_rand_mov_but + rand_mov_height);
    random_movie_button[3].position = Vector2f(x_rand_mov_but, y_rand_mov_but + rand_mov_height);
    random_movie_button[0].color = Color(247, 221, 118);
    random_movie_button[1].color = Color(251, 231, 153);
    random_movie_button[2].color = Color(185, 170, 113);
    random_movie_button[3].color = Color(251, 236, 179);

    sf::RectangleShape random_movie_box(Vector2f(200, 100));
    random_movie_box.setPosition(0, 840);
    random_movie_box.setFillColor(Color(247, 221, 118));

	// ОКНО ЗАГРУЗКИ
	show_load_scr(window, font2); 

    // переменные для скроллбара
    bool is_dragging_scrollbar = false;
    float drag_start_y = 0.f;
    float scroll_start_offset = 0.f; 

//ГЛАВНЫЙ ЦИКЛ

        while (window.isOpen()) {
            Event event;

            // фильтрация фильмов по жанру и поисковому запросу
            std::vector<Movie> filtered_movies;
            std::vector<const Texture*> filtered_textures;

            for (size_t i = 0; i < all_movies.size(); ++i) {
                const Movie& movie = all_movies[i];

                if (!selected_genre.empty() && !movie.hasGenre(selected_genre) && selected_genre != L"Все фильмы")
                    continue;

                if (!search_query.empty()) {
                    std::wstring title = movie.getTitle();
                    std::wstring title_lower = title;
                    std::wstring query_lower = search_query;

                    // перевод в нижний регистр
                    for (auto& ch : title_lower) ch = std::towlower(ch);
                    for (auto& ch : query_lower) ch = std::towlower(ch);

                    if (title_lower.find(query_lower) == std::wstring::npos)
                        continue;
                }

                filtered_movies.push_back(movie);
                filtered_textures.push_back(&poster_textures[i]);
            }


            // подсчет общей высоты для скроллбара — симулируем позиционирование элементов
            const float start_y = 75.f; // начальная позиция для первой карточки
            const float poster_h = 330.f;
            const float desc_h = 90.f;
            const float spacing = 30.f;
            const float bottom_padding = 60.f; // доп. пространство внизу

            float content_bottom = start_y;
            for (size_t i = 0; i < filtered_movies.size(); ++i) {
                // для каждой карточки добавляем высоту постера, описание и отступ
                content_bottom += poster_h + desc_h + spacing;
            }

            float total_height = content_bottom + bottom_padding;
            // максимальная прокрутка: на сколько пикселей можно сдвинуть контент, чтобы увидеть его до конца
            float max_scroll = std::max(0.f, content_bottom - window.getSize().y + bottom_padding);

            while (window.pollEvent(event)) {

                // закрытие окна (если просто закрыть приложение)
                if (event.type == Event::Closed) {
                    window.close();
                }

                // Если сейчас показано окно случайного фильма — любые клавиша или клик должны закрыть оверлей
                else if (show_rand_movie) {
                    if (event.type == Event::KeyPressed || (event.type == Event::MouseButtonPressed)) {
                        show_rand_movie = false;
                    }
                    // пропускаем остальную обработку событий пока открыт оверлей
                    continue;
                }

                // закрытие приложения по Escape (только если оверлей не показан)
                else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    window.close();
                }

                // прокрутка колесиком мыши
                else if (event.type == Event::MouseWheelScrolled) {
                    if (event.mouseWheelScroll.wheel == Mouse::VerticalWheel) {
                        scroll_offset -= event.mouseWheelScroll.delta * 60.f;
                        if (scroll_offset < 0.f)
                            scroll_offset = 0.f;
                        if (scroll_offset > max_scroll)
                            scroll_offset = max_scroll;
                    }
                }

                // клик по жанру и сортировке
                else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mouse_pos = window.mapPixelToCoords(Mouse::getPosition(window));

                    // жанры
                    for (size_t i = 0; i < genre_texts.size(); ++i) {
                        if (genre_texts[i].getGlobalBounds().contains(mouse_pos)) {
                            selected_genre = genres[i];
                            scroll_offset = 0.f; // при смене жанра возвращаемся наверх
                        }
                    }

                    // сортировка по году
                    if (sort_year.getGlobalBounds().contains(mouse_pos)) {
                        if (current_sort == sort_type::year) {
                            current_sort = sort_type::None;
                            all_movies = original_movies;
                            poster_textures = original_textures;

                        }
                        else {
                            current_sort = sort_type::year;
                            std::sort(all_movies.begin(), all_movies.end(), [](const Movie& a, const Movie& b) { return a.getYear() < b.getYear(); });
                            std::vector<Texture> new_textures(all_movies.size());

                            for (size_t i = 0; i < all_movies.size(); ++i)
                                new_textures[i].loadFromFile(all_movies[i].getPosterPath());
                            poster_textures = std::move(new_textures);
                        }
                        scroll_offset = 0.f;
                    }

                    // сортировка по рейтингу
                    if (sort_rating.getGlobalBounds().contains(mouse_pos)) {

                        if (current_sort == sort_type::rating) {
                            current_sort = sort_type::None;
                            all_movies = original_movies;
                            poster_textures = original_textures;

                        }
                        else {
                            current_sort = sort_type::rating;
                            std::sort(all_movies.begin(), all_movies.end(), [](const Movie& a, const Movie& b) { return a.getRating() > b.getRating(); });
                            std::vector<Texture> new_textures(all_movies.size());

                            for (size_t i = 0; i < all_movies.size(); ++i)
                                new_textures[i].loadFromFile(all_movies[i].getPosterPath());
                            poster_textures = std::move(new_textures);
                        }
                        scroll_offset = 0.f;
                    }

                    // обработка клика по полосе прокрутки
                    float view_height = window.getSize().y;
                    float scrollbar_height = (total_height > 0) ? (view_height * view_height / total_height) : view_height;
                    float scrollbar_track_height = std::max(0.f, view_height - scrollbar_height);
                    float scrollbar_y = (max_scroll > 0.f) ? (scroll_offset / max_scroll) * scrollbar_track_height : 0.f;
                    FloatRect scrollbar_rect(window.getSize().x - 12.f, scrollbar_y, 8.f, scrollbar_height);

                    if (scrollbar_rect.contains(mouse_pos)) {
                        is_dragging_scrollbar = true;
                        drag_start_y = mouse_pos.y;
                        scroll_start_offset = scroll_offset;
                    }

                    // обработка клика по кнопке "Фильм на вечер"
                    if (random_movie.getGlobalBounds().contains(mouse_pos)) {
                        if (!all_movies.empty()) {
                            show_rand_movie = true;
                            random_movie_index = rand() % all_movies.size();
                        }
                    }
                }

                // отпускание кнопки мыши
                if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
                    is_dragging_scrollbar = false;
                }

                // движение мыши при перетаскивании
                if (event.type == Event::MouseMoved && is_dragging_scrollbar) {
                    float view_height = window.getSize().y;
                    float deltaY = static_cast<float>(event.mouseMove.y) - drag_start_y;

                    // Используем тот же max_scroll, который учитывает bottom_padding
                    float total_height_scrollable = max_scroll;
                    scroll_offset = scroll_start_offset + (deltaY / view_height) * total_height_scrollable;

                    if (scroll_offset < 0.f) scroll_offset = 0.f;
                    if (scroll_offset > total_height_scrollable) scroll_offset = total_height_scrollable;
                }

                // ввод текста в строку поиска
                if (event.type == Event::TextEntered) {
                    if (event.text.unicode == 8) { // backspace
                        if (!search_query.empty())
                            search_query.pop_back();
                    }
                    else if (event.text.unicode == 13) {
                        // enter — можно будет использовать позже
                    }
                    else if (event.text.unicode >= 32 && event.text.unicode != 128) {
                        search_query += static_cast<wchar_t>(event.text.unicode);
                    }

                    search_text.setString(search_query);
                }
            }
            window.clear(Color(85, 86, 91));

            // левая панель и поиск
            window.draw(left_pan);
            window.draw(search_box);
            window.draw(search_text);

            // подсветка кнопок сортировки
            sort_year.setFillColor(current_sort == sort_type::year ? Color(247, 221, 118) : Color::White);
            sort_rating.setFillColor(current_sort == sort_type::rating ? Color(247, 221, 118) : Color::White);
            window.draw(sort_year);
            window.draw(sort_rating);

            // подсвечивание жанров
            for (size_t gi = 0; gi < genre_texts.size(); ++gi) {
                genre_texts[gi].setFillColor(genres[gi] == selected_genre ? Color(247, 221, 118) : Color::White);
                window.draw(genre_texts[gi]);
            }

            // отрисовка вертикального скроллбара
            float view_height = window.getSize().y;        // видимая высота
            float scrollbar_height = (total_height > 0) ? (view_height * view_height / total_height) : view_height;
            float scrollbar_track_height = std::max(0.f, view_height - scrollbar_height);
            float scrollbar_y = (max_scroll > 0.f) ? (scroll_offset / max_scroll) * scrollbar_track_height : 0.f;

            // фон полосы прокрутки
            RectangleShape scroll_track(Vector2f(8.f, view_height));
            scroll_track.setFillColor(Color(50, 50, 50));
            scroll_track.setPosition(window.getSize().x - 12.f, 0);
            window.draw(scroll_track);

            // ползунок
            RectangleShape scroll_thumb(Vector2f(8.f, scrollbar_height));
            scroll_thumb.setFillColor(is_dragging_scrollbar ? Color(247, 221, 118) : Color(251, 236, 179));
            scroll_thumb.setPosition(window.getSize().x - 12.f, scrollbar_y);
            window.draw(scroll_thumb);

            // отрисовка фильмов
            float movie_y = 75 - scroll_offset; // начальная позиция по y с учетом прокрутки
            const float card_x = 220.f; // позиция по х для карточек фильмов
			const float overlay_h = 60.f; // высота подложки для текста на постере

        // отрисовка карточек фильмов
            for (size_t i = 0; i < filtered_movies.size(); ++i) {
                const Movie& movie = filtered_movies[i];
                const Texture& tex = *filtered_textures[i];
                Vector2u ts = tex.getSize();
                if (ts.x == 0 || ts.y == 0)
                    continue;

                float card_w = window.getSize().x - card_x - 20.f; // 1400 - 220 - 20
                float s = card_w / static_cast<float>(ts.x); // масштабирование по ширине

                std::setlocale(LC_NUMERIC, "C"); // локаль для отображения точки в числе

                wchar_t buf[10];
                swprintf(buf, 10, L"%.1f", movie.getRating());
                std::wstring ratingStr(buf);

                // постер
                sf::Sprite poster_sprite(tex);
                poster_sprite.setScale(s, s); // одинаковое масштабирование по х и у
                poster_sprite.setPosition(card_x, movie_y);
                poster_sprite.setTextureRect(IntRect(0, 0, ts.x, static_cast<int>(poster_h / s)));
                window.draw(poster_sprite);

                // подложка 
                sf::RectangleShape overlay(Vector2f(card_w, overlay_h));
                overlay.setPosition(card_x, movie_y + poster_h - overlay_h);
                overlay.setFillColor(Color(0, 0, 0, 180));
                window.draw(overlay);

                // название фильма
                sf::Text title_text;
                title_text.setFont(font2);
                title_text.setCharacterSize(31);
                title_text.setFillColor(Color::White);
                title_text.setString(movie.getTitle() + L" (" + std::to_wstring(movie.getYear()) + L")");
                title_text.setPosition(card_x + 31.f, overlay.getPosition().y + 9.f);
                window.draw(title_text);

                // рейтинг
                sf::RectangleShape rating_box;
                rating_box.setSize(Vector2f(60.f, 40.f));
                rating_box.setFillColor(Color(247, 221, 118, 220));
                rating_box.setPosition(card_x + card_w - 70.f, poster_sprite.getPosition().y + 10.f);
                window.draw(rating_box);

                sf::Text rating_text;
                rating_text.setFont(font1);
                rating_text.setCharacterSize(30);
                rating_text.setFillColor(Color(0, 0, 0, 240));
                rating_text.setString(ratingStr);
                rating_text.setPosition(card_x + card_w - 58.f, poster_sprite.getPosition().y + 10.f);
                window.draw(rating_text);

                // описание
                auto clamp_desc = [&](std::wstring s)->std::wstring {
                    if (s.size() > 500)
                        s = s.substr(0, 197) + L"...";
                    return s;
                    };

                sf::Text desc_text;
                desc_text.setFont(font1);
                desc_text.setCharacterSize(20);
                desc_text.setFillColor(Color(220, 220, 220));
                std::wstring wrapped_desc = wrapText(
                    clamp_desc(movie.getDescription()),
                    font1, 20, card_w - 20.f
                );
                desc_text.setString(wrapped_desc);
                desc_text.setPosition(card_x + 20.f, movie_y + poster_h + 10.f);
                window.draw(desc_text);

                movie_y += poster_h + desc_h + 30.f;
            }

            if (show_rand_movie && random_movie_index >= 0 && random_movie_index < (int)all_movies.size()) {
                const Movie& movie = all_movies[random_movie_index];
                const Texture& tex = poster_textures[random_movie_index];
                // затемняем фон
                sf::RectangleShape overlay_bg(Vector2f((float)window.getSize().x, (float)window.getSize().y));
                overlay_bg.setFillColor(Color(85, 86, 91));
                window.draw(overlay_bg);

                // размеры постера
                const float card_x = 240.f;
                float card_w = window.getSize().x - card_x - 20.f;
                const float poster_h = 330.f;
                Vector2u tex_size = tex.getSize();

                // отрисовка предлагаемого фильма
                if (tex_size.x > 0 && tex_size.y > 0) {
                    sf::VertexArray wind(sf::Quads, 4);
                    wind[0].position = Vector2f(90, 160);
                    wind[1].position = Vector2f(90 + 1220, 160);
                    wind[2].position = Vector2f(90 + 1220, 160 + 600);
                    wind[3].position = Vector2f(90, 160 + 600);
                    wind[0].color = Color(23, 23, 23);
                    wind[1].color = Color(40, 40, 40);
                    wind[2].color = Color(23, 23, 23);
                    wind[3].color = Color(27, 27, 27);

                    window.draw(wind);

                    float s = card_w / static_cast<float>(tex_size.x);
                    int srcH = static_cast<int>(std::round(poster_h / s));
                    if (srcH > (int)tex_size.y) srcH = tex_size.y;

                    // текстурный прямоугольник сверху-центр
                    int srcX = 0;
                    int srcY = std::max(0, (int)tex_size.y / 2 - srcH / 2);

                    Sprite post_sprite(tex);
                    post_sprite.setTextureRect(IntRect(srcX, srcY, (int)tex_size.x, srcH));
                    post_sprite.setScale(s, s);

                    // wентрируем постер по X и устанавливаем позицию по у
                    FloatRect spriteBounds = post_sprite.getGlobalBounds();
                    float posX = (window.getSize().x - spriteBounds.width) / 2.f;
                    float posY = window.getSize().y - 700.f; 
                    post_sprite.setPosition(posX, posY);
                    window.draw(post_sprite);

                    sf::Text title(movie.getTitle() + L" (" + std::to_wstring(movie.getYear()) + L") ", font2, 40);
                    title.setFillColor(Color::White);
                    title.setPosition(130.f, 550.f);
                    window.draw(title);

                    wchar_t buf0[10];
                    swprintf(buf0, 10, L"%.1f", movie.getRating());
                    std::wstring ratingstr0(buf0);

                    // рейтинг
                    sf::RectangleShape rating_box;
                    rating_box.setSize(Vector2f(60.f, 40.f));
                    rating_box.setFillColor(Color(247, 221, 118, 220));
                    rating_box.setPosition(1200, post_sprite.getPosition().y + 10.f);
                    window.draw(rating_box);

                    sf::Text rating_text;
                    rating_text.setFont(font1);
                    rating_text.setCharacterSize(30);
                    rating_text.setFillColor(Color(0, 0, 0, 240));
                    rating_text.setString(ratingstr0);
                    rating_text.setPosition(1212, post_sprite.getPosition().y + 10.f);
                    window.draw(rating_text);

                    sf::Text description;
                    description.setFont(font1);
                    description.setFillColor(Color(220, 220, 220));
                    description.setCharacterSize(20);

                    auto clamp_desc0 = [&](std::wstring s)->std::wstring {
                        if (s.size() > 500)
                            s = s.substr(0, 197) + L"...";
                        return s;
                        };

                    std::wstring desc = wrapText(
                        clamp_desc0(movie.getDescription()),
                        font1, 20, card_w - 20.f
                    );

                    description.setString(desc);
                    description.setPosition(135, 615);
                    window.draw(description);
                }
             }
            window.draw(top_pan);
            window.draw(sort_year);
            window.draw(sort_rating);
            window.draw(random_movie_button);
            window.draw(random_movie);
            window.display();
        }
}