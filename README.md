# Змейка 🐍

Классическая «Змейка» на C11 + Raylib. Нативное desktop-приложение, хардкор-режим:
минимум зависимостей, максимум контроля, ноль магии фреймворков.

Статус: этап проектирования. Требования и архитектура зафиксированы,
код появится следующим шагом.

## Документация

| Документ                              | Что внутри                                     |
| ------------------------------------- | ---------------------------------------------- |
| [docs/requirements.md](docs/requirements.md)                 | Требования к продукту: FR/NFR, приёмка v1 |
| [docs/architecture/overview.md](docs/architecture/overview.md) | Принципы, слои, технические решения (ADR) |
| [docs/architecture/modules.md](docs/architecture/modules.md) | Структура кода, контракты ядра на C, границы модулей |
| [docs/architecture/engineering.md](docs/architecture/engineering.md) | Процесс: сборка, тесты, коммиты, цикл задачи |

## Стек

- C11 (без расширений) + Raylib — графика/окно/шрифты
- GCC (MinGW-w64, MSYS2), сборка CMake + Ninja
- Тесты: Unity (ThrowTheSwitch) + CTest — логика ядра без окна
- Флаги: `-Wall -Wextra -Wpedantic -Werror -std=c11` — ноль предупреждений

## Настройка окружения (Windows)

1. Установить [MSYS2](https://www.msys2.org) (по умолчанию `C:\msys64`).
2. В оболочке «MSYS2 UCRT64»:

   ```bash
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc cmake ninja mingw-w64-ucrt-x86_64-raylib
   ```

3. Сборка и тесты:

   ```bash
   cmake --preset default
   cmake --build build
   ctest --test-dir build
   ./build/snake.exe
   ```

Важно: raylib в MSYS2 есть только в окружении UCRT64. Сборку запускать
из «MSYS2 UCRT64»-оболочки или из терминала с `C:\msys64\ucrt64\bin` в PATH —
без этого gcc не находит собственные DLL и падает без диагностики.

## Скиллы разработки

В `.agents/skills/` живут проектные скиллы:

- `/snake-implement` — реализация фич по требованиям и архитектуре
- `/snake-verify` — сборка, тесты, смоук-проверка игры
- `/snake-review` — ревью изменений против архитектуры

## План v1

- [x] Каркас проекта (CMake + raylib + Unity, пустое окно)
- [x] Core-логика + тесты (FR-1…FR-8)
- [ ] Ввод и рендер (FR-3…FR-5, NFR-1, NFR-5)
- [ ] Экраны, пауза, сложность, рекорды (FR-9…FR-15)
- [ ] Приёмка по чек-листу из requirements.md §6 (+ прогон ASan/Valgrind)
