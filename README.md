# Змейка 🐍

Классическая «Змейка» на TypeScript + HTML5 Canvas.

Статус: этап проектирования. Требования и архитектура зафиксированы,
код появится следующим шагом.

## Документация

| Документ                              | Что внутри                                     |
| ------------------------------------- | ---------------------------------------------- |
| [docs/requirements.md](docs/requirements.md)                 | Требования к продукту: FR/NFR, приёмка v1 |
| [docs/architecture/overview.md](docs/architecture/overview.md) | Принципы, слои, технические решения (ADR) |
| [docs/architecture/modules.md](docs/architecture/modules.md) | Структура кода, контракты ядра, границы модулей |
| [docs/architecture/engineering.md](docs/architecture/engineering.md) | Процесс: коммиты, тесты, цикл задачи |

## Стек

- TypeScript (strict) + HTML5 Canvas 2D
- Vite — сборка и dev-сервер
- Vitest — юнит-тесты игровой логики (без браузера)

## Скиллы разработки

В `.agents/skills/` живут проектные скиллы:

- `/snake-implement` — реализация фич по требованиям и архитектуре
- `/snake-verify` — сборка, тесты, смоук-проверка игры
- `/snake-review` — ревью изменений против архитектуры

## План v1

- [ ] Каркас проекта (Vite + TS + Vitest, ESLint/Prettier)
- [ ] Core-логика + тесты (FR-1…FR-8)
- [ ] Ввод и рендер (FR-3…FR-5, NFR-1, NFR-5)
- [ ] Экраны, пауза, сложность, рекорды (FR-9…FR-15)
- [ ] Приёмка по чек-листу из requirements.md §6
