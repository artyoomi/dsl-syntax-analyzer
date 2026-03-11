/*
Template according to: https://se.moevm.info/doku.php/courses:reportrules

Latex reference from one cool guy:
https://github.com/JAkutenshi/eltechLaTeXTemplates/blob/master/LabReports/tex/title.tex
*/

#set page(
  width: 210mm,
  height: 297mm,
  margin: (top: 20mm, bottom: 20mm, left: 30mm, right: 15mm)
)

#set text(
  size: 14pt,
  lang: "ru",
  // Only on local build
  // font: "Times New Roman"
)

#set par(
  leading: 1.5em,
  first-line-indent: 12.5mm,
  justify: true
)

#show heading.where(level: 1): it => [
  #set align(center)
  #set text(weight: "bold", size: 14pt)
  #set par(first-line-indent: 0pt, leading: 1.5em)
  #upper(it.body)
]

#show heading.where(level: 2): it => [
  #set text(weight: "bold", size: 14pt)
  #set par(first-line-indent: 1.25cm, leading: 1.5em, justify: true)
  #it.body
]

#show heading.where(level: 3): it => [
  #set text(weight: "bold", size: 13pt)
  #set par(first-line-indent: 1.25cm, leading: 1.5em, justify: true)
  #it.body
]

#set list(indent: 1.5em)
#set enum(indent: 1.5em)

// Force all raw blocks to have 1em indent between lines
#show raw.where(block: true): set par(leading: 1em)

#align(center)[
  #set text(weight: "semibold")

  #set par(leading: 1em)

  МИНОБРНАУКИ РОССИИ \
  САНКТ-ПЕТЕРБУРГСКИЙ ГОСУДАРСТВЕННЫЙ \
  ЭЛЕКТРОТЕХНИЧЕСКИЙ УНИВЕРСИТЕТ \
  «ЛЭТИ» ИМ. В.И. УЛЬЯНОВА (ЛЕНИНА) \
  Кафедра МО ЭВМ

  #v(54mm)

  ОТЧЕТ \
  по лабораторной работе №2 \
  по дисциплине «Интеллектуальные технологии и компьютерные инструменты передачи и извлечения знаний» \
  Тема: Разработка синтаксического анализатора DSL для доступа к БД

  #v(54mm)

  #table(
    columns: (33%, 33%, 33%),
    inset: 10pt,
    align: horizon,
    stroke: none,
    "Студент гр. 3381",
    "",
    table.hline(start: 1 , end: 2),
    "Иванов А.А.",
    "Преподаватель",
    "",
    table.hline(start: 1 , end: 2),
    "Малютин Е.В."
  )

  #set align(bottom)
  Санкт-Петербург \
  #datetime.today().year()
]

#pagebreak()

// Start numbering here to skip first page numering
#set page(
  numbering: "1"
)

// To make indent before first header
\
== Задание

Разработать формальную грамматику для предметно-ориентированного языка (DSL) запросов к базе данных и реализовать синтаксический анализатор, строящий деревья разбора для корректных выражений.

*Важно*: В работе запрещается использовать готовые статистические парсеры естественного языка (spaCy, Stanza, DeepPavlov). Разбор должен выполняться строго по формальной грамматике, разработанной студентом.

Язык запросов к базе данных какой-либо предметной области, например, «Библиотека». Запросы формулируются на ограниченном естественном языке и должны соответствовать заданной грамматике.

Примеры корректных запросов:
- "найди все книги"
- "покажи статьи 2020 года"
- "найди книги Толстого и Достоевского"
- "выведи все журналы по программированию"
- "найди книги по лингвистике изданные после 2015"
\

*Входные данные*:
Текстовый файл, содержащий список запросов. Запросы могут быть как синтаксически правильными, так и содержать ошибки.

*Выходные данные*:
Для каждого запроса программа должна вывести:
1.	Исходный запрос
2.	Результат синтаксического анализа (успех/неудача)
3.	В случае успеха — дерево разбора в текстовом или графическом виде
4.	В случае неудачи — диагностику ошибки (на каком токене произошла ошибка)


== Выполнение работы

=== Введение

Domain Specific Language (DSL) --- языки, специфичные для какой-либо предметной области, предназначенные, в отличие от языков общего назначения, для решения какой-то конкретной задачи в заданных рамках. Например, к DSL относятся такие языки как LaTeX, Typst, SystemVerilog, SQL и другие.

//В данной работе в качестве предметной области для БД выбраны "Языки программирования".
В данной работе в качестве предметной области не было выбрано ничего конкретного. Это просто SQL-like синтаксис.

=== Этапы создания DSL

Создание DSL состоит из следующих фаз @dsl_dev:
+ Принятие решения о создании DSL
+ Анализ требований
+ Дизайн
+ Реализация
+ Развертывание
В рамках данной лабораторной работы будут рассматриваться только пункты 2, 3 и часть пункта 4 (реализация синтаксического анализатора).

=== Анализ требований
Перед построением грамматики следует определиться с характеристиками DSL. Так как это язык запросов к БД, то он должен поддерживать все основные операции с данными: получить, добавить, удалить, изменить.

=== Дизайн

Примерный вид запросов следующий:
+ Получение данных:  `GET [CONDITION]`;
+ Добавление данных: `ADD [DATA]`;
+ Изменение данных: `CHANGE [NEW_DATA] IF [CONDITION]`;
+ Удаление данных: `DELETE [CONDITION]`.

Отсюда можно составить формальную грамматику для разрабатываемого DSL. Она представлена в @grammar.

#figure(
  block(
    width: 100%,
    stroke: 1pt + black,
    inset: 10pt,
    
    align(left)[
    ```bnf
    S ::=
      GET <CONDITION>
    | ADD <DATA>
    | CHANGE TO <DATA> IF <CONDITION>
    | DELETE <CONDITION>
    
    CONDITION ::=
      <BASE_CONDITION>
    | <BASE_CONDITION> AND <CONDITION>
    | <BASE_CONDITION> OR  <CONDITION>
  
    BASE_CONDITION ::=
      property == value
    | property != value
    
    DATA ::=
      property = value
    | property = value, <DATA>
    ```
    ]
  ),
  caption: [
    Формальная грамматика разрабатываемого DSL.
  ]
) <grammar>

\
=== Реализация
Данный пункт рассматривает только вопрос реализации синтаксического анализатора для языка и построение дерева разбора без дальнейшей имплементации интерпретатора.

Синтаксический анализатор написан на языке C++ с использованием библиотеки Boost.Parser @boost_parser. С помощью библиотеки определяются правила парсинга, а внутри `semantic actions` (своего рода `callback` - функция, которая вызывается при отрабатывании правила) строятся узлы абстрактного синтаксического дерева.

Для построения графиков используется Graphviz. Написанный в ходе работы код предоставляет функционал для перевода AST в файл формата `.dot`, который понимает Graphviz.

Из интересных моментов можно выделить generic функцию на @generic_bfs для обхода дерева в BFS стиле. Она позволяет избегать дублирование кода, когда написать несколько функций с разной логикой, но каждая из которых требует при своей работе обход дерева в ширину.

#figure(
  block(
    width: 100%,
    stroke: 1pt + black,
    inset: 10pt,
    align(left)[
      ```cpp
      template<
          typename Preprocess,
          typename ProcessNode,
          typename OnChildEnqueue,
          typename Postprocess>
      static void nodeTraverse(
          const NodePtr& root,
          Preprocess preprocess,
          ProcessNode processNode,
          OnChildEnqueue onChildEnqueue,
          Postprocess postprocess)
      {
          std::queue<NodePtr> q;
          q.push(root);
      
          preprocess(root);
      
          while (!q.empty()) {
              NodePtr node = q.front();
              q.pop();
      
              processNode(node);
      
              for (auto const& child : node->children) {
                  onChildEnqueue(node, child);
                  q.push(child);
              }
          }
      
          postprocess(root);
      }
      ```
    ]
  ),
  caption: [
    Шаблонный код обхода дерева в ширину.
  ]
) <generic_bfs>

\
=== Примеры работы
Программа в качестве входа получает запрос и файл, в который нужно сохранить полученный граф разбора.

#block(
  width: 100%,
  stroke: 1pt + black,
  inset: 10pt,
  ```sh
  $ ./main
  > CHANGE TO a = "5", b = "6", c = "9" IF c == "10"
  Filename of .dot: graph.dot

  # Для создания .png на основе .dot
  $ dot -Tpng graph.dot -o graph.png
  ```
)

Результатом разбора данного запроса представлен на @graph.

#figure(
  image("images/graph.png"),
  caption: [
    Результат разбора
  ]
) <graph>


#pagebreak()
#bibliography("refs.bib")

