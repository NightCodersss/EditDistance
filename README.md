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
	18. Заставить проходить тесты +
	19. Умные указатели +

#Документация

##Принцип работы

* [Ссылка на статью об автоматах](http://arxiv.org/pdf/0904.4686.pdf)
* [Ссылка на статью о нахождении следующего минимального пути](http://en.wikipedia.org/wiki/K_shortest_path_routing)

Для работы с юникодом (`utf-8`) используется [ICU](http://site.icu-project.org/). Строки конвертируются в `vector<char_type>` (`char_type = int`) (с заменой `ε` на -1) и обрабатываются в таком виде.

Реализация работы с `Python` в `extern.cpp`.

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

###Заметка
Ограничения в `k` путей убирать нельзя, ибо асимптотика выраждается до порядка полного перебора всех путей в графе (что плохо).

#Использование

##C++
Заголовки, с которыми это работает

```
#include <iostream>
#include <fstream>
#include "transducer.hpp"
#include "intervalunion.hpp"

#include "icu.hpp"

#include "transduceroptimizer.hpp"
```

Следует понимать, что функция `isEmpty()` возращает `true` если автомат пустой и `false` если он не пуст или пуст, но не тривиален. Им с чистой совестью можно проверять пустоту композицию.

###Ввод
####Чтение трансдьюсера из файла.

```
Transducer X;

std::ifstream in1("x.trans");

X.readFromFile(in1);
X.visualize(std::cout);
```

Формат файла:
```
number_of_edges id_of_final_state
start in out weight end
start in out weight end
...
start in out weight end
```
 * `start` - ID вершины начала ребра
 * `in` - блок или символ принятия ребром (`a` или `[^bcy-z]`)
 * `out` - блок или символ отдачи ребром (если блок, то должен совпадать с блоком входа, преобразование будет тождественным на указаных смволах)
 * `wieght` - (целое число) вес ребра
 * `end` - ID вершины конца ребра

 Initial state всегда имеет ID = 0

####Из regexp'а
```
auto A = Transducer::fromRegexp(convertUnicode(UnicodeString::fromUTF8(StringPiece("aaatbbbaaababbabb"))));
```

####Модель ошибок
```
std::ifstream in('tbig.am');
auto A = Transducer::fromAlignmentModel(in)
```

Формат файла:
Множество строк вида: 
```
regex string weight
```

Где каждая строка описывает правило замены регэкспа на строку с зданым весом.

Такой файл можно нагенерировать при помощи `gen_alignment_model.py` (создает по алфавиту модель ошибок как у левинштейна).

###Оптимизация
В результате операций ввода автоматы получаются рабочими, корректными  хорошими, но большими. Их можно пожать. Для этого можно использовать `TransducerOptimizer`.

```
auto A = Transducer::fromRegexp(convertUnicode(UnicodeString::fromUTF8(StringPiece("aaatbbbaaababbabb"))));
TransducerOptimizer to2(A);
to2.optimize();
```

Оптимизация основана на сжатии линейных участков (соответсвенно удаление эпсилон переходов, которые можно удалить). Это позволяет сжать введеный автомат в 10-15 раз.

Разумность идеи оптимизировать композицию оптимизорованых автоматов сомнительна.
###Нахождение минимального пути
Можно сбрасывать пути на начало и получать каждый следующий путь (в разумных пределах их количество ограничено константой в `char_type.hpp`)

Если пути заканчиваются, возвращается пустой путь с весом `-1`.

Использование смотри в примере.

###Пример
```
    auto X = Transducer::fromRegexp(convertUnicode(UnicodeString::fromUTF8(StringPiece("abababababtaaaba"))));
    TransducerOptimizer to1(X);
    to1.optimize();
    
    auto A = Transducer::fromRegexp(convertUnicode(UnicodeString::fromUTF8(StringPiece("aaatbbbaaababbabb"))));
    TransducerOptimizer to2(A);
    to2.optimize();

    std::ifstream in("tweight.am");
    auto T = Transducer::fromAlignmentModel(in);
    TransducerOptimizer to3(T);
    to3.optimize();

    auto XTA = X.composition(std::move(T)).composition(std::move(A));
    XTA.removeEpsilonEdges();
    XTA.visualize(std::cout);

    XTA.resetMinPaths();

    auto path = XTA.getNextMinPath();
    std::string result;

    for ( const auto& edge : path.path )
    {
        std::string str_;
        auto str = convertFromStringType(edge -> io.toString()).toUTF8String(str_);
        result += "(" + str_ + " " + std::to_string(edge -> weight) + ")";
    }

    std::cout << "Path cost: " << path.cost << '\n';
    std::cout << "Path: " << result << '\n';
```

##Python
Import:
```
from trans import Transducer, screen, pathsFromWordToRegexp
```

Вообще говоря из питона следует использовать только `pathsFromWordToRegexp` возвращающую итератор по путям.

```
paths = pathsFromWordToRegexp(s1.encode('utf-8'), s2.encode('utf-8'), 'tbig.am')
(word, weight, pathinfo) = paths.next()
```

Где:
 * `word` - слово из языка, к которому привели исходное
 * `weight` - стоимость перевода
 * `pathinfo` - дополнительная информацию о пути (список IO ребер пути) 

 Про остальные функции можно почитать в хелпе Python'a, а так же при помощи файла `extern.cpp` поставить в соответсвие функциям Python'a функции C++.

 Как пример (может не самый лучший, в связи с доработками) можно изучить `test_regex.py`
