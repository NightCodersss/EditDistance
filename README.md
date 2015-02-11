EditDistance

#TODO:

    1. Правила задания модели ошибок
    2. Работа с unicode
    3. Case-insensitive
    4. Фигурные скобки
    5. bash-скрипт
    6. Блок [a-zA-Z]      +
    7. Дополнение в блоке +



#Документация

##Принцип работы

[Ссылка на статью об автоматах](http://arxiv.org/pdf/0904.4686.pdf)
[Ссылка на статью о нахождении следующего минимального пути](http://en.wikipedia.org/wiki/K_shortest_path_routing)

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
