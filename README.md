EditDistance

#TODO:

    1. Правила задания модели ошибок +
    2. Работа с unicode     +
    3. Case-insensitive     +
    4. Фигурные скобки      +
    5. Блок [a-zA-Z]        +
    6. Дополнение в блоке   +
    7. Интерфейс для python +
    8. Хорошая документация
    9. Автоматические тесты +
    10. Документация к функциям в Python +
	11. Тесты регэкспов     +
	12. Тесты простых замен +
	13. Нормальный эпсилон при выводе +
	14. Выпилить кол-во правил в `*.am`+
	15. Выпилить пробелы из путей
	16. Тесты тяжелого символа +
	17. Сохранения seed +
	18. Заставить проходить тесты

Тесты могут падать из-за нехватки паямяти, тк она не высвобождается


#Документация

##Принцип работы

* [Ссылка на статью об автоматах](http://arxiv.org/pdf/0904.4686.pdf)
* [Ссылка на статью о нахождении следующего минимального пути](http://en.wikipedia.org/wiki/K_shortest_path_routing)

##Классы

* [Transducer](#transducer)
* [RegexpParser](#regexpparser)
* [IntervalUnion](#intervalunion)
* [IO](#IO)

###Transducer

Класс для работы с автоматами (преобразователями)
Использует классы `State` и `Edge` для внутреннего представления графа.

###RegexpParser

Класс для разбора регулярного выражения и преобразования его в автомат (`Transducer`).

###IntervalUnion

Класс для работы с системой неперескающихся отрезков (концы включены). Имеет возможность получить
объединение и пересечение систем, получить дополнение к множеству.

###IO

Структура, описывающая ребро в графе автомата. Это символ на ребре автомата в общем виде (может так же представлять собой блок).
Имеет встроенный тип `IO::IOType`. 

Возможные значения:

* `UnionUnion` - блок-блок (в себя)
* `UnionLetter` - блок-буква
* `LetterLetter` - буква-буква

Можно получить композицию двух ребёр с помощью функции `IO::composition`, а так же проверить возможность скомпоновать рёбра.

##Алгоритм нахождения следующего минимального пути

Если хранить `B` из статьи на википедии, то можно выдавать следующий по длине (или равной длины) путь онлайн. Если все пути
были получены, выдается путь с длиной -1.
